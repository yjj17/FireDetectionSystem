import os
import tensorflow as tf
from tensorflow import keras
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
import random

IMG_SIZE = 224

#데이터 준비
cur_dir = os.getcwd()
data_dir = os.path.join(cur_dir,'data')
camera_dir = os.path.join(data_dir,'crawled_images')
camera0_dir = os.path.join(camera_dir,'0')
camera1_dir = os.path.join(camera_dir,'1')
camera2_dir = os.path.join(camera_dir,'2')

normal_img_files = os.listdir(camera0_dir)
fire_img_files = os.listdir(camera1_dir)
smoke_img_files = os.listdir(camera2_dir)

#클래스 별 학습할 이미지 개수 통일 (일반:화재:연기 = 1:1:1 비율로 학습하기 위해)
m = min(len(normal_img_files),len(fire_img_files))
m = min(m,len(smoke_img_files))
normal_img_files = normal_img_files[0:m]
fire_img_files = fire_img_files[0:m]
smoke_img_files = smoke_img_files[0:m]

# 학습 데이터를 용량이 적은 RAM 대신에 디스크에서 필요한 만큼 불러가면서 학습하기 위해 tfrecord를 사용함
# 학습 데이터 디스크에 저장
###############tfrecord 저장
tfr_dir = os.path.join(camera_dir, 'tfrecord')
os.makedirs(tfr_dir, exist_ok=True)

train_tfr_dir = os.path.join(tfr_dir, 'train.tfr')
val_tfr_dir = os.path.join(tfr_dir, 'val.tfr')

train_tfr_writer = tf.io.TFRecordWriter(train_tfr_dir)
val_tfr_writer = tf.io.TFRecordWriter(val_tfr_dir)


def _bytes_feature(value):
    if isinstance(value, type(tf.constant(0))):
        value = value.numpy()
    return tf.train.Feature(bytes_list=tf.train.BytesList(value=[value]))


def _float_feature(value):
    return tf.train.Feature(float_list=tf.train.FloatList(value=[value]))


def _int64_feature(value):
    return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))


N_N_TRAIN = 3 * (len(normal_img_files) // 4)
for i, file in enumerate(normal_img_files):
    img_path = os.path.join(camera0_dir, file)
    image = Image.open(img_path)
    image = image.resize((IMG_SIZE, IMG_SIZE))
    bimage = image.tobytes()

    example = tf.train.Example(features=tf.train.Features(feature={
        'image': _bytes_feature(bimage),
        'cls_num': _int64_feature(0)
    }))
    if i < N_N_TRAIN:
        train_tfr_writer.write(example.SerializeToString())
    else:
        val_tfr_writer.write(example.SerializeToString())

N_F_TRAIN = 3 * (len(fire_img_files) // 4)
for i, file in enumerate(fire_img_files):
    img_path = os.path.join(camera1_dir, file)
    image = Image.open(img_path)
    image = image.resize((IMG_SIZE, IMG_SIZE))
    bimage = image.tobytes()

    example = tf.train.Example(features=tf.train.Features(feature={
        'image': _bytes_feature(bimage),
        'cls_num': _int64_feature(1)
    }))

    if i < N_F_TRAIN:
        train_tfr_writer.write(example.SerializeToString())
    else:
        val_tfr_writer.write(example.SerializeToString())

N_S_TRAIN = 3 * (len(smoke_img_files) // 4)
for i, file in enumerate(smoke_img_files):
    img_path = os.path.join(camera2_dir, file)
    image = Image.open(img_path)
    image = image.resize((IMG_SIZE, IMG_SIZE))
    bimage = image.tobytes()

    example = tf.train.Example(features=tf.train.Features(feature={
        'image': _bytes_feature(bimage),
        'cls_num': _int64_feature(2)
    }))

    if i < N_S_TRAIN:
        train_tfr_writer.write(example.SerializeToString())
    else:
        val_tfr_writer.write(example.SerializeToString())

train_tfr_writer.close()
val_tfr_writer.close()
###############/tfrecord 저장

#데이터 수 계산
N_TRAIN = N_F_TRAIN+N_N_TRAIN+N_S_TRAIN #train 데이터의 총 개수
N_VAL = len(fire_img_files)-N_F_TRAIN + len(normal_img_files)-N_N_TRAIN + len(smoke_img_files)-N_S_TRAIN #vlaidation 데이터의 총 개수
N_BATCH = 32 #batch size


# 디스크에서 학습 데이터를 필요한 만큼 불러오는 인스턴스 생성
###############tfrecord 인스턴스 생성
def _parse_function(tfreced_serialized):
    features = {
        'image': tf.io.FixedLenFeature([], tf.string),
        'cls_num': tf.io.FixedLenFeature([], tf.int64)
    }

    parsed_features = tf.io.parse_single_example(tfreced_serialized, features)

    image = tf.io.decode_raw(parsed_features['image'], tf.uint8)
    image = tf.reshape(image, [IMG_SIZE, IMG_SIZE, 3])
    image = tf.cast(image, tf.float32) / 255.

    cls_label = tf.cast(parsed_features['cls_num'], tf.int64)

    return image, cls_label


train_dataset = tf.data.TFRecordDataset(train_tfr_dir)
train_dataset = train_dataset.map(_parse_function, num_parallel_calls=tf.data.experimental.AUTOTUNE)
train_dataset = train_dataset.shuffle(buffer_size=N_TRAIN).prefetch(tf.data.experimental.AUTOTUNE).batch(
    N_BATCH).repeat()

val_dataset = tf.data.TFRecordDataset(val_tfr_dir)
val_dataset = val_dataset.map(_parse_function, num_parallel_calls=tf.data.experimental.AUTOTUNE)
val_dataset = val_dataset.batch(N_BATCH).repeat()
###############/tfrecord 인스턴스 생성

#모델 생성
def create_model():
    model = tf.keras.Sequential()
    model.add(tf.keras.layers.Conv2D(filters=32, kernel_size=3, activation='relu',padding='SAME', input_shape=(IMG_SIZE,IMG_SIZE,3)))
    model.add(tf.keras.layers.MaxPool2D(padding='SAME'))
    model.add(tf.keras.layers.Conv2D(filters=64, kernel_size=3, activation='relu',padding='SAME'))
    model.add(tf.keras.layers.MaxPool2D(padding='SAME'))
    model.add(tf.keras.layers.Conv2D(filters=128, kernel_size=3, activation='relu',padding='SAME'))
    model.add(tf.keras.layers.MaxPool2D(padding='SAME'))
    model.add(tf.keras.layers.Conv2D(filters=256, kernel_size=3, activation='relu',padding='SAME'))
    model.add(tf.keras.layers.MaxPool2D(padding='SAME'))
    model.add(tf.keras.layers.Conv2D(filters=256, kernel_size=3, activation='relu',padding='SAME'))
    model.add(tf.keras.layers.MaxPool2D(padding='SAME'))
    model.add(tf.keras.layers.Flatten())
    model.add(tf.keras.layers.Dense(1024, activation='relu'))
    model.add(tf.keras.layers.Dropout(0.4))
    model.add(tf.keras.layers.Dense(3,activation='softmax'))
    return model

model = create_model()
model.summary()

#하이퍼 파라미터 설정
N_EPOCHS = 10
learning_rate = 0.0001
steps_per_epoch = N_TRAIN/N_BATCH
validation_step = int(np.ceil(N_VAL / N_BATCH))

#오차 함수
def loss_fn(y_true,y_pred):
    return keras.losses.SparseCategoricalCrossentropy()(y_true,y_pred)

lr_schedule = tf.keras.optimizers.schedules.ExponentialDecay(initial_learning_rate=learning_rate,
                                                         decay_steps=steps_per_epoch*2,
                                                         decay_rate=0.5,
                                                         staircase=True)
model.compile(tf.keras.optimizers.RMSprop(lr_schedule),loss=loss_fn, metrics = ['accuracy'])

#모델 학습
model.fit(train_dataset, steps_per_epoch=steps_per_epoch,
         epochs=N_EPOCHS,
         validation_data=val_dataset,
         validation_steps=validation_step)


#모델 테스트는 .ipynb 파일에서 진행

#모델 저장
model.save('./model/Classification_Model')
model.save_weights('./model/Classification_Model_weights.h5')