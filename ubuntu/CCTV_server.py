import socket 
import numpy as np
import cv2

def recvall(sock, count):
    buf = b''
    while count:
        newbuf = sock.recv(count)
        if not newbuf: return '0'
        buf += newbuf
        count -= len(newbuf)
    return buf


HOST = '192.168.219.104'
PORT = 9999
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind((HOST,PORT))
server.listen()
client, addr = server.accept()

print('Connected by', addr)

while True:
    message = '1'
    client.send(message.encode()) 
  
    length = recvall(client,16)
    stringData = recvall(client, int(length))
    data = np.frombuffer(stringData, dtype='uint8') 

    decimg=cv2.imdecode(data,1)
    cv2.imshow('Image',decimg)
    
    key = cv2.waitKey(1)
    if key == 27:
        break

client.close()
server.close()
