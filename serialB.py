from module.ModuleSerial import Serial
import time

# 创建串口对象B，初始化：串口为COM9，波特率为115200
B = Serial('COM9', 115200, in_time=1, in_dest=2, in_send=1, in_nums=1)
# B.comLoad()

while True:
    B.comRecv()                                 #B接收串口数据
    if B.lstRecv():
        print(B.lstRecv(True))                  #打印所有接收到的包

    Btime = int(time.time())                    #当前时间
    if B.getSend('time') == None:          
        B.addSend('time', 'P', "I", Btime)      #添加发送数据'time'，data：当前时间
    else:
        B.putSend('time', Btime, "I")           #如果已添加过发送数据'time'，则修改时间为当前时间
    if B.getSend('echo') == None:
        B.addSend('echo', 'P', "S", "SCURM")    #添加发送数据'echo'，data："SCURM"

    B.comSend()                                 #B发送串口数据

    time.sleep(0.5)                               #等待0.5s