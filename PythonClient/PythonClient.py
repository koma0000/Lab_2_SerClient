# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

import socket, struct
from enum import IntEnum
from threading import Thread
from time import sleep
from datetime import datetime
import sys


class Addresses(IntEnum):
    MIDDLEWARE = 0,
    BROADCAST = -1,


class MessageTypes(IntEnum):
    MT_INIT = 0,
    MT_EXIT = 1,
    MT_CONFIRM = 2,
    MT_GETDATA = 3,
    MT_DATA = 4,
    MT_NODATA = 5


class MsgHeader:
    def __init__(self, msgFrom=0, msgTo=0, msgType=0, msgSize=0):
        self.msgFrom = msgFrom
        self.msgTo = msgTo
        self.msgType = msgType
        self.msgSize = msgSize

    def HeaderInit(self, header):
        self.msgFrom = header[0]
        self.msgTo = header[1]
        self.msgType = header[2]
        self.msgSize = header[3]


# from - инструкция, поэтому пишем с большой

class Message:

    
    # конструктор
    def __init__(self, From=0, To=0, Type=MessageTypes.MT_DATA, sdata=''):
        self.msgHeader = MsgHeader()
        self.msgHeader.msgFrom = From;
        self.msgHeader.msgTo = To;
        self.msgHeader.msgType = Type;
        self.msgHeader.msgSize = int(len(sdata))
        self.msgData = sdata

    # отправка данных
    def SendData(self, s):
        # struct.pack упаковывает данные в байты,i -  int
        s.send(struct.pack('i', self.msgHeader.msgFrom))
        s.send(struct.pack('i', self.msgHeader.msgTo))
        s.send(struct.pack('i', self.msgHeader.msgType))
        s.send(struct.pack('i', self.msgHeader.msgSize))
        if self.msgHeader.msgSize>0:
            # Это «форматированные строковые литералы», f-strings являются строковыми
            # литералами с «f» в начале и фигурные скобки,
            # содержащие выражения, которые в дальнейшем будут
            # заменены своими значениями.
            # Динамическая подстановка данных
            #l=len(self.msgData)
            s.send(struct.pack(f'{self.msgHeader.msgSize}s', self.msgData.encode('utf-8')))

    def ReceiveData(self, s):
        try:
            self.msgHeader = MsgHeader()
            # i - распаковка 1 целого числа в байты, 4i-целых числа
            self.msgHeader.HeaderInit(struct.unpack('iiii', s.recv(16)))
            if self.msgHeader.msgSize>0:
                self.msgData = struct.unpack(f'{self.msgHeader.msgSize}s', s.recv(self.msgHeader.msgSize))[0]
            return self.msgHeader.msgType
        except Exception as e:
            pass;


# отправляем сообщения
def SendMessage(Socket, From, To, Type=MessageTypes.MT_DATA, sdata=""):
    m = Message(From, To, Type, sdata)
    m.SendData(Socket)


# принимаем сообщения
def Receive(Socket):
    m = Message()
    res = m.ReceiveData(Socket)
    return res

id_client=0
#Port = 12345
# устанавливаем соединение
def connect(Socket):
    Socket.connect(('127.0.0.1', 12345))


# прерываем соединение
def disconnect(Socket):
    Socket.close()


def GetData(ID):
    while True:
        # создает INET, STREAM сокет(AF_INET-протокол IPv4, SOCK_STREAM- потоковый сокет)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as  clientSock:
            connect(clientSock)
            SendMessage(clientSock, ID, 0, MessageTypes.MT_GETDATA)
            msg = Message()
            if (msg.ReceiveData(clientSock) == MessageTypes.MT_DATA):
                print('Message from client ', msg.msgHeader.msgFrom, msg.msgData.decode('utf-8'))
            disconnect(clientSock)

#новый сокет
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    #устанавливаем соединенеи
    sleep(1) # установка таймаута
    connect(s)
    SendMessage(s, 0, 0, MessageTypes.MT_INIT)#подтверждение
    msg = Message()
    if msg.ReceiveData(s) == MessageTypes.MT_CONFIRM:
        id_client= msg.msgHeader.msgTo
        print('Client is starting\n')
        print('ID is ', id_client)
        #подключились,создаем id, выводим
        #target=GetData -вызываемая функция
        #кортеж аргументов для вызываемого объекта (аргумент target)
        # daemon=True фон
        t = Thread(target=GetData, args=(id_client,), daemon=True)
        t.start()
        disconnect(s)
    else:
        print('error')
        sys.exit(0)
while True:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        ch = int(input('1. Send Message\n2. Exit\n'))
        if ch == 1:
            FORClientID = Addresses.BROADCAST
            print ('1. Only for one client\n2. For All Clients\n')
            ch2 = int(input())
            if ch2 == 1:
                print ('\nEnter id\n')
                FORClientID = int(input())
            print ('\nEnter your Message\n')
            mess = input()
            connect(s)
            SendMessage(s, id_client,FORClientID, MessageTypes.MT_DATA, mess)
            if msg.ReceiveData(s) == MessageTypes.MT_CONFIRM:
                print('\nsuccess\n')
            else:
                print('\nerror\n')
            disconnect(s)
        elif ch == 2:
            connect(s)
            SendMessage(s, id_client, 0, MessageTypes.MT_EXIT)
            if msg.ReceiveData(s) == MessageTypes.MT_CONFIRM:
                print('\nsuccess\n')
                sys.exit(0)
            else:
                print('\nerror\n')
            disconnect(s)
           