import socket
import cv2
import numpy
from queue import Queue
from _thread import *

enclosure_queue = Queue()


def webcam_transmit(client_socket, queue):
    while True:
        try:
            data = client_socket.recv(1024)
            if not data:
                print('Disconnected')
                break

            stringData = queue.get()
            client_socket.send(str(len(stringData)).ljust(16).encode())
            client_socket.send(stringData)

        except ConnectionResetError as e:

            print('Disconnected')
            break

    client_socket.close()


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

        data = numpy.array(imgencode)
        stringData = data.tostring()

        queue.put(stringData)

        cv2.imshow('image', frame)
        key = cv2.waitKey(1)
        if key == 27:
            return


HOST = '192.168.219.104'
PORT = 9999

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST,PORT))

print('client start')
i = start_new_thread(webcam_input, (enclosure_queue,))
t = start_new_thread(webcam_transmit, (client_socket, enclosure_queue,))
