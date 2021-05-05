import socket
import cv2
import numpy
from queue import Queue
from _thread import *

enclosure_queue = Queue()

#webcam으로부터 영상을 읽어서 queue에 넣는 쓰레드가 수행하는 함수
def webcam_input(queue):
    capture = cv2.VideoCapture('./fire and smoke.mp4')
    speed=6
    i = 0
    while True:
        
        ret, frame = capture.read()
        
        if ret == False:
            continue
        i+=1
        if (i % speed) != 0 :
            continue
        encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 90]
        result, imgencode = cv2.imencode('.jpg', frame, encode_param)

        #영상을 문자열 자료형으로 변환 후 저장
        data = numpy.array(imgencode)
        stringData = data.tostring()
        queue.put(stringData)

        #영상 출력
        cv2.imshow('image', frame)
        key = cv2.waitKey(1)
        if key == 27:
            return

#queue에서 영상을 읽어서 socket을 이용해 서버로 전송하는 쓰레드가 수행하는 함수
def webcam_transmit(client_socket, queue):
    while True:
        try:
            #서버로 부터 메시지를 받을 때마다 영상을 한 장씩 전송
            data = client_socket.recv(1024)
            if not data:
                print('Disconnected')
                break

            # 영상(문자열) 전송
            stringData = queue.get()
            client_socket.send(str(len(stringData)).ljust(16).encode())
            client_socket.send(stringData)

        except ConnectionResetError as e:

            print('Disconnected')
            break

    client_socket.close()

#서버의 IP주소및 포트번호
HOST = '192.168.219.104'
PORT = 9999
#소켓 생성및 연결
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST,PORT))

print('client start')
#영상 읽기 쓰레드 생성
i = start_new_thread(webcam_input, (enclosure_queue,))
#영상 전송 쓰레드 생성
t = start_new_thread(webcam_transmit, (client_socket, enclosure_queue,))
