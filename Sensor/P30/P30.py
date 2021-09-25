import time
import serial
import threading

FrameState = 0
Bytenum = 0
CheckSum = 0

depth = 0
confidence = 0
Depdata = [0.0]*11

def DueData(inputdata):
    global  FrameState
    global  Bytenum
    global  CheckSum
    global  depth
    global  confidence
    global  Depdata

    for data in inputdata:
        if FrameState==0:
            if data==0x42 and Bytenum==0:
                CheckSum=data
                Bytenum=1
                continue
            elif data==0x52 and Bytenum==1:
                CheckSum+=data
                Bytenum=2
                FrameState=1
                continue

        elif FrameState==1:

            if Bytenum<13:
                Depdata[Bytenum-2]=data
                CheckSum+=data
                Bytenum+=1
            else:
                if data == (CheckSum&0xff):
                    depth,confidence = get_depth(Depdata)
                    #print("depth:{}m confidence:{}%".format(depth,confidence))
                CheckSum=0 
                Bytenum=0
                FrameState=0




def get_depth(datahex):
    dep1 = datahex[6]
    dep2 = datahex[7]
    dep3 = datahex[8]
    dep4 = datahex[9]
    conf = datahex[10]
    num = (hex(dep4) + hex(dep3) + hex(dep2) + hex(dep1)).replace("0x","")
    num = "0x"+num
    depth = int(num,16)
    return depth/1000,conf


port = "/dev/ttyUSB0"
baud = 115200
ser = serial.Serial(port, baud, timeout=0.5)  # ser = serial.Serial('com7',115200, timeout=0.5) 

def main():
    send_start=bytes.fromhex('42 52 02 00 78 05 00 00 BB 04 D2 01')
    ser.write(send_start)
    while(1):
        datahex = ser.read(33)
        DueData(datahex)


th = threading.Thread(target=main, args=())
th.start()
