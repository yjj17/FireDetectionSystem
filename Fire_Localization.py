import os

import numpy as np
from PIL import Image
import random
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import xml.etree.ElementTree as et

import tensorflow as tf
from tensorflow import keras
import re
import shutil

#데이터 준비
cur_dir = os.getcwd()
data_dir = os.path.join(cur_dir, 'data/crawled_images/annotated')
train_dir = os.path.join(data_dir, 'train')
test_dir = os.path.join(data_dir, 'test')
train_img_files = [imgname for imgname in os.listdir(train_dir) if os.path.splitext(imgname)[-1] == '.jpg']
train_ano_files = [anoname for anoname in os.listdir(train_dir) if os.path.splitext(anoname)[-1] == '.xml']
test_img_files = [imgname for imgname in os.listdir(test_dir) if os.path.splitext(imgname)[-1] == '.jpg']
test_ano_files = [anoname for anoname in os.listdir(test_dir) if os.path.splitext(anoname)[-1] == '.xml']

#데이터 수 계산
IMG_SIZE = 224
N_BBOX = len(train_ano_files)
N_TRAIN = len(train_img_files)
N_VAL = len(test_img_files)


# 학습 데이터를 용량이 적은 RAM 대신에 디스크에서 필요한 만큼 불러가면서 학습하기 위해 tfrecord를 사용함
# 학습 데이터 디스크에 저장
###############tfrecord 저장
tfr_dir = os.path.join(data_dir, 'tfrecord')
os.makedirs(tfr_dir, exist_ok=True)

tfr_train_dir = os.path.join(tfr_dir, 'loc_train.tfr')
tfr_val_dir = os.path.join(tfr_dir, 'loc_val.tfr')

writer_train = tf.io.TFRecordWriter(tfr_train_dir)
writer_val = tf.io.TFRecordWriter(tfr_val_dir)


def _bytes_feature(value):
    if isinstance(value, type(tf.constant(0))):
        value = value.numpy()
    return tf.train.Feature(bytes_list=tf.train.BytesList(value=[value]))


def _float_feature(value):
    return tf.train.Feature(float_list=tf.train.FloatList(value=[value]))


def _int64_feature(value):
    return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))


for ano_file in train_ano_files:
    ano_path = os.path.join(train_dir, ano_file)
    tree = et.parse(ano_path)
    width = float(tree.find('./size/width').text)
    height = float(tree.find('./size/height').text)
    xmin = float(tree.find('./object/bndbox/xmin').text)
    ymin = float(tree.find('./object/bndbox/ymin').text)
    xmax = float(tree.find('./object/bndbox/xmax').text)
    ymax = float(tree.find('./object/bndbox/ymax').text)
    xc = (xmin + xmax) / 2
    yc = (ymin + ymax) / 2
    x = xc / width
    y = yc / height
    w = (xmax - xmin) / width
    h = (ymax - ymin) / height

    img_file = os.path.splitext(ano_file)[0] + '.jpg'
    img_path = os.path.join(train_dir, img_file)
    image = Image.open(img_path)
    image = image.resize((IMG_SIZE, IMG_SIZE))
    bimage = image.tobytes()

    cls_num = 1

    example = tf.train.Example(features=tf.train.Features(feature={
        'image': _bytes_feature(bimage),
        'cls_num': _int64_feature(cls_num),
        'x': _float_feature(x),
        'y': _float_feature(y),
        'w': _float_feature(w),
        'h': _float_feature(h)
    }))

    writer_train.write(example.SerializeToString())

writer_train.close()

for ano_file in test_ano_files:
    ano_path = os.path.join(test_dir, ano_file)
    tree = et.parse(ano_path)
    width = float(tree.find('./size/width').text)
    height = float(tree.find('./size/height').text)
    xmin = float(tree.find('./object/bndbox/xmin').text)
    ymin = float(tree.find('./object/bndbox/ymin').text)
    xmax = float(tree.find('./object/bndbox/xmax').text)
    ymax = float(tree.find('./object/bndbox/ymax').text)
    xc = (xmin + xmax) / 2
    yc = (ymin + ymax) / 2
    x = xc / width
    y = yc / height
    w = (xmax - xmin) / width
    h = (ymax - ymin) / height

    img_file = os.path.splitext(ano_file)[0] + '.jpg'
    img_path = os.path.join(test_dir, img_file)
    image = Image.open(img_path)
    image = image.resize((IMG_SIZE, IMG_SIZE))
    bimage = image.tobytes()

    cls_num = 1

    example = tf.train.Example(features=tf.train.Features(feature={
        'image': _bytes_feature(bimage),
        'cls_num': _int64_feature(cls_num),
        'x': _float_feature(x),
        'y': _float_feature(y),
        'w': _float_feature(w),
        'h': _float_feature(h)
    }))

    writer_val.write(example.SerializeToString())

writer_val.close()
###############/tfrecord 저장


#하이퍼파라미터 설정
N_CLASS = 3
N_EPOCHS = 30
N_BATCH = 5
IMG_SIZE = 224
learning_rate = 0.0001
steps_per_epoch = N_TRAIN/N_BATCH
validation_step = int(np.ceil(N_VAL / N_BATCH))

# 디스크에서 학습 데이터를 필요한 만큼 불러오는 인스턴스 생성
###############tfrecord 인스턴스 생성
def _parse_function(tfrecord_serialized):
    features = {
        'image': tf.io.FixedLenFeature([], tf.string),
        'cls_num': tf.io.FixedLenFeature([], tf.int64),
        'x': tf.io.FixedLenFeature([], tf.float32),
        'y': tf.io.FixedLenFeature([], tf.float32),
        'w': tf.io.FixedLenFeature([], tf.float32),
        'h': tf.io.FixedLenFeature([], tf.float32)
    }

    parsed_features = tf.io.parse_single_example(tfrecord_serialized, features)

    image = tf.io.decode_raw(parsed_features['image'], tf.uint8)
    image = tf.reshape(image, [IMG_SIZE, IMG_SIZE, 3])
    image = tf.cast(image, tf.float32) / 255.

    cls_label = tf.cast(parsed_features['cls_num'], tf.int64)

    x = tf.cast(parsed_features['x'], tf.float32)
    y = tf.cast(parsed_features['y'], tf.float32)
    w = tf.cast(parsed_features['w'], tf.float32)
    h = tf.cast(parsed_features['h'], tf.float32)
    gt = tf.stack([x, y, w, h], -1)

    return image, gt


train_dataset = tf.data.TFRecordDataset(tfr_train_dir)
train_dataset = train_dataset.map(_parse_function, num_parallel_calls=tf.data.experimental.AUTOTUNE)
train_dataset = train_dataset.shuffle(buffer_size=N_TRAIN).prefetch(tf.data.experimental.AUTOTUNE).batch(
    N_BATCH).repeat()

val_dataset = tf.data.TFRecordDataset(tfr_val_dir)
val_dataset = val_dataset.map(_parse_function, num_parallel_calls=tf.data.experimental.AUTOTUNE)
val_dataset = val_dataset.batch(N_BATCH).repeat()
###############/tfrecord 인스턴스 생성


#모델 생성
def create_model():
    model = tf.keras.Sequential()
    model.add(keras.layers.Conv2D(filters=32, kernel_size=3, activation='relu',padding='SAME', input_shape=(IMG_SIZE,IMG_SIZE,3)))
    model.add(keras.layers.MaxPool2D(padding='SAME'))
    model.add(keras.layers.Conv2D(filters=64, kernel_size=3, activation='relu',padding='SAME'))
    model.add(keras.layers.MaxPool2D(padding='SAME'))
    model.add(keras.layers.Conv2D(filters=128, kernel_size=3, activation='relu',padding='SAME'))
    model.add(keras.layers.MaxPool2D(padding='SAME'))
    model.add(keras.layers.Conv2D(filters=256, kernel_size=3, activation='relu',padding='SAME'))
    model.add(keras.layers.MaxPool2D(padding='SAME'))
    model.add(keras.layers.Conv2D(filters=256, kernel_size=3, activation='relu',padding='SAME'))
    model.add(keras.layers.MaxPool2D(padding='SAME'))
    model.add(keras.layers.Flatten())
    model.add(keras.layers.Dense(1024, activation='relu'))
    model.add(keras.layers.Dropout(0.4))
    model.add(keras.layers.Dense(4,activation='sigmoid'))
    return model

model = create_model()
model.summary()

#하이퍼파라미터 설정2
def loss_fn(y_true,y_pred):
    return keras.losses.MeanSquaredError()(y_true,y_pred)

lr_schedule = keras.optimizers.schedules.ExponentialDecay(initial_learning_rate=learning_rate,
                                                         decay_steps=steps_per_epoch*2,
                                                         decay_rate=0.5,
                                                         staircase=True)
model.compile(keras.optimizers.RMSprop(lr_schedule),loss=loss_fn)

#모델 학습
model.fit(train_dataset, steps_per_epoch=steps_per_epoch,
         epochs=N_EPOCHS,
         validation_data=val_dataset,
         validation_steps=validation_step)

#모델 테스트는 .ipynb 파일에서 진행

#모델 저장
model.save('./model/Localization_Model')
model.save_weights('./model/Localization_Model_weights.h5')