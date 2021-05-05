import tensorflow as tf
from tensorflow import keras
from PIL import Image
import numpy as np
import cv2

IMG_SIZE = 224

#Localization Model의 오차함수
def local_loss_fn(y_true,y_pred):
    return keras.losses.MeanSquaredError()(y_true,y_pred)

#Classification Model의 오차함수
def class_loss_fn(y_true,y_pred):
    return keras.losses.SparseCategoricalCrossentropy()(y_true,y_pred)

#Localization + Classification Model
class MyModel(tf.keras.Model):
    def __init__(self):
        super(MyModel, self).__init__()
        self.classification = tf.keras.models.load_model("./model/Classification_Model",custom_objects={'loss_fn': local_loss_fn})
        self.classification.load_weights('./model/Classification_Model_weights.h5')
        self.localization = tf.keras.models.load_model("./model/Localization_Model",custom_objects={'loss_fn': class_loss_fn})
        self.localization.load_weights('./model/Localization_Model_weights.h5')
        self.con = tf.keras.layers.Concatenate(axis=-1)

    def call(self, x, training=False, mask=None):
        a = self.classification(x)
        b = self.localization(x)

        return self.con([a,b])

#모델 생성
model = MyModel()

#모델 테스트
cap = cv2.VideoCapture('./data/video/CCTV/fire and smoke.mp4')
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
print(height, width)
i = 0
SPEED = 3  # 영상 재생 속도
while True:
    if cap.grab():
        ret, frame = cap.retrieve()
        i += 1
        if i % SPEED != 0:
            continue
        if ret:
            # 영상을 읽어서 모델에 입력할 수 있도록 자료형 변환
            frame2 = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
            image = Image.fromarray(frame2)
            image = image.resize((IMG_SIZE, IMG_SIZE))
            image = tf.cast(np.array(image), tf.float32) / 255.
            image = image[tf.newaxis, ...]

            # 영상 인식
            prediction = model.predict(image)
            pred_label = int(tf.argmax(prediction[0][:3]))
            pred_local = prediction[0][3:]

            # 원본 영상에 인식결과 합성
            if pred_label == 0:
                frame = cv2.rectangle(frame, (0, 0), (150, 40), (255, 255, 255), cv2.FILLED)
                frame = cv2.putText(frame, "Non-Fire", (0, 30), cv2.FONT_ITALIC, 1, (0, 0, 255), 3)
            elif pred_label == 1:
                frame = cv2.rectangle(frame, (0, 0), (80, 40), (255, 255, 255), cv2.FILLED)
                frame = cv2.putText(frame, "Fire!!", (0, 30), cv2.FONT_ITALIC, 1, (0, 0, 255), 3)
                pred_x = pred_local[0]
                pred_y = pred_local[1]
                pred_w = pred_local[2]
                pred_h = pred_local[3]

                pred_xmin = int((pred_x - pred_w / 2.) * cap.get(cv2.CAP_PROP_FRAME_WIDTH))
                pred_ymin = int((pred_y - pred_h / 2.) * cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
                pred_xmax = int((pred_x + pred_w / 2.) * cap.get(cv2.CAP_PROP_FRAME_WIDTH))
                pred_ymax = int((pred_y + pred_h / 2.) * cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

                frame = cv2.rectangle(frame, (pred_xmin, pred_ymin), (pred_xmax, pred_ymax), (0, 0, 255), 2)

            elif pred_label == 2:
                frame = cv2.rectangle(frame, (0, 0), (120, 40), (255, 255, 255), cv2.FILLED)
                frame = cv2.putText(frame, "SMOKE!", (0, 30), cv2.FONT_ITALIC, 1, (0, 0, 255), 3)

            # 합성 영상 출력
            cv2.imshow('video', frame)
            k = cv2.waitKey(1) & 0xFF
            if k == 27:
                cap.release()
                cv2.destroyWindow('video')
                break
        else:
            print('error')
    else:
        break

# 영상 출력 윈도우 제거
if cv2.getWindowProperty('video', 0) != -1:
    cap.release()
    cv2.destroyWindow('video')
