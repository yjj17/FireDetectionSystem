import socket 
import numpy as np
import cv2

import tensorflow as tf
from tensorflow import keras


#모델 불러오기
def loss_fn(y_true,y_pred):
    return keras.losses.MeanSquaredError()(y_true,y_pred)

def local_loss_fn(y_true,y_pred):
    return keras.losses.MeanSquaredError()(y_true,y_pred)

def class_loss_fn(y_true,y_pred):
    return keras.losses.SparseCategoricalCrossentropy()(y_true,y_pred)

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

model = MyModel()

#TCP 소켓으로부터 메시지 수신
def recvall(sock, count):
    buf = b''
    while count:
        newbuf = sock.recv(count)
        if not newbuf: return '0'
        buf += newbuf
        count -= len(newbuf)
    return buf


IMG_SIZE = 224

#서버의 IP및 포트번호
HOST = '192.168.219.104'
PORT = 9999
#TCP 소켓 연결
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind((HOST,PORT))
server.listen()
client, addr = server.accept()


print('Connected by', addr)

while True:
    #서버가 클라이언트로 메시지를 보내면 클라이언트가 영상(문자열) 한 장을 보냄
    message = '1'
    client.send(message.encode()) 

    #영상(문자열)을 이미지(opencv)로 변환
    length = recvall(client,16)
    stringData = recvall(client, int(length))
    data = np.frombuffer(stringData, dtype='uint8')
    decimg=cv2.imdecode(data,1)

    #이미지(opencv)를 모델에 맞는 데이터로 변환
    decimg = cv2.resize(decimg,(IMG_SIZE, IMG_SIZE))
    image = decimg[tf.newaxis,...]

    #데이터 입력및 분류
    prediction = model.predict(image)
    pred_label = int(tf.argmax(prediction[0][:3]))
    pred_local = prediction[0][3:]

    #원본 이미지에 분류 결과 정보(화재여부, 화재 지역) 합성
    image = image[0]
    if pred_label==0:
        image = cv2.rectangle(image,(0,0),(150,40),(255,255,255),cv2.FILLED)
        image=cv2.putText(image,"Non-Fire", (0, 30), cv2.FONT_ITALIC, 1, (0, 0, 255),3)
    elif pred_label == 1:
        image = cv2.rectangle(image,(0,0),(80,40),(255,255,255),cv2.FILLED)
        image = cv2.putText(image,"Fire!!", (0, 30), cv2.FONT_ITALIC, 1, (0, 0, 255),3)
        pred_x = pred_local[0]
        pred_y = pred_local[1]
        pred_w = pred_local[2]
        pred_h = pred_local[3]
            
        pred_xmin = int((pred_x - pred_w/2.))*IMG_SIZE
        pred_ymin = int((pred_y - pred_h/2.))*IMG_SIZE
        pred_xmax = int((pred_x + pred_w/2.))*IMG_SIZE
        pred_ymax = int((pred_y + pred_h/2.))*IMG_SIZE
            
        image = cv2.rectangle(image,(pred_xmin,pred_ymin),(pred_xmax,pred_ymax),(0,0,255),2)

    elif pred_label== 2:
        image = cv2.rectangle(image,(0,0),(120,40),(255,255,255),cv2.FILLED)
        image = cv2.putText(image,"SMOKE!", (0, 30), cv2.FONT_ITALIC, 1, (0, 0, 255),3)

    #합성 이미지 출력
    cv2.imshow('video', image)

   
    key = cv2.waitKey(1)
    if key == 27:
        break

client.close()
server.close()
