from module.ModuleSerial import Serial
import time

# 创建串口对象A，初始化：串口为COM10，波特率为115200
A = Serial('COM10', 115200, in_time=1, in_dest=1, in_send=2, in_nums=0)
# A.comLoad()

while True:
    Atime = int(time.time())                    # 当前时间
    # 添加两个数据： time、 echo
    if A.getSend('time') == None:
        A.addSend('time', 'P', "I", Atime)      #添加发送数据'time'，data：当前时间
    else:
        A.putSend('time', Atime, "I")           #如果已添加过发送数据'time'，则修改时间为当前时间
    if A.getSend('echo') == None:
        A.addSend('echo', 'P', "S", "hello")    #添加数据'echo'，data："SCURM"

    # 打印已添加的数据
    # print(A.lstSend(True))
    print(A.getSend('time'))                    #以名称name查看
    print(A.getSend('echo'))
    # print(A.getSend(0))                       #以id查看
    # print(A.getSend(1))

    A.putSend('echo', "RMNB!!", "S")            # 删除'echo'的data:'hello'，添加data:'RMNB!!'

    A.comSend()                                 #A发送串口数据

    A.comRecv()                                 #A接收串口数据
    if A.getRecv('time'):                       #接收成功，'time'不为空
        Btime = A.getRecv('time')['data'][0]    #获取'time':data
        print(Atime)
        print(Btime)
        print(Btime-Atime)                      #打印差值

    time.sleep(0.5)                             #等待0.5s


