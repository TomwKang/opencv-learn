// ----------------------------------------
//   SCURM-CPP控制组-通用模组-串口通信模块
//             By Pikachu
//       Last-Update:DEC22/2020
// ----------------------------------------

#ifndef MODULE_SERIAL_H
#define MODULE_SERIAL_H

//---------------全局配置-------------------
//在MDK下编译,部分设置需要改变，此选项请开启
#define  stms    true  //是否在MDK下编译

//在MDK下编译没有printf,应当关闭下列开关选项
#define  dbug    false  //是否启用调试输出

//在单IDE下调试的时候需要关闭 否则无收接收包
#define  pnew    false  //是否只录入新数据

//自行设置的缓冲区大小， 取决于STM32芯片缓存
#if stms==false
#define  size    8192    //临时缓冲区的大小
#else
#define  size    1024    //临时缓冲区的大小
#endif

//是否启用STM32上的串口延迟， 便于接收多字节
#define enableDelay false
//---------------全局配置-------------------

//---------------路由配置-------------------
// 发送方和接收方定义都是个char类型8bits数据
// 接收方数据定义
//----------------------------------------
//           0   1   2   3   4   5   6   7
// 步兵1-MPC 1   1   0   0   0   0   0   0
// 步兵1-STM 0   1   0   0   0   0   0   0
// 步兵2-MPC 1   0   1   0   0   0   0   0
// 步兵2-STM 0   0   1   0   0   0   0   0
// 步兵3-MPC 1   0   0   1   0   0   0   0
// 步兵3-STM 0   0   0   1   0   0   0   0
// 英雄--NX1 1   0   0   0   1   0   0   0
// 英雄--STM 0   0   0   0   1   0   0   0
// 工程--NX1 1   0   0   0   0   1   0   0
// 工程--STM 0   0   0   0   0   1   0   0
// 哨兵--NX1 1   0   0   0   0   0   1   0
// 哨兵--STM 0   0   0   0   0   0   1   0
// 无人机TX2 1   0   0   0   0   0   0   1
// 无人机STM 0   0   0   0   0   0   0   1
// 雷达BigPC 1   0   0   0   0   0   0   0
// 单播模式下，仅能选择上述条件的一项来传输
// 多播模式下，可以自由组合上述多个项目数值
// 但是请注意，接收方不可以是0xff的这个数值
//----------------------------------------

// 发送方数据定义
//----------------------------------------
//           0   1   2   3   4   5   6   7
// 信息传输方式---
// 单播      0   0
// 多播      0   1
// 广播      1   0
// 错误      1   1
// 组播协议参数------------
// 单播/广播情况下   0    0
// 多播-只发给STM:   0    0
// 多播-只发给MPC:   0    1
// 多播MPC+STM32:    1    0
// 多播--保留参数:   1    1
// 发送方的编号----------------------------
// 步兵1-MPC                  1   0   0   1
// 步兵1-STM                  0   0   0   1
// 步兵2-MPC                  1   0   1   0
// 步兵2-STM                  0   0   1   0
// 步兵3-MPC                  1   0   1   1
// 步兵3-STM                  0   0   1   1
// 英雄--NX1                  1   1   0   0
// 英雄--STM                  0   1   0   0
// 工程--NX1                  1   1   0   1
// 工程--STM                  0   1   0   1
// 哨兵--NX1                  1   1   1   0
// 哨兵--STM                  0   1   1   0
// 无人机TX2                  1   1   1   1
// 无人机STM                  0   1   1   1
// 雷达BigPC                  1   0   0   0
// 保留位置                   1   1   1   1
// ----------------------------------------
#define id_recv_bb1_stm 01000000b
#define id_recv_bb1_mpc 11000000b
//---------------路由配置------------------

//---------------定义头部------------------
#define ui32 unsigned int
#define ui16 unsigned short
#define ui08 char

        #define sendNumLimit      10
		#define nullptr           NULL
			#define includeHead "com_usart.h"												//引用的头文件(除非你将头文件改名，否则不用改)
		#define initMySerial(a)   Com_USART_Configuration(a)		//初始化串口(不用改)
		#define sendThePack(a,b)  Com_USART_Print(a,b)			    //发包的函数(不用改)
		
		#define whichUSART   1
		#if whichUSART == 1
		#define communicationUart USART1						//使用的串口(注意是USART还是UART)
		#define Com_UseUART_IRQHandler   USART1_IRQHandler()
		#elif whichUSART == 6
		#define communicationUart USART6						//使用的串口(注意是USART还是UART)
		#define Com_UseUART_IRQHandler   USART6_IRQHandler()
		#endif
		
		#define useObject 1
		#define useMd5 0
		
		#if useObject==1
		#define serialObject	myserial
		#endif
		
		#if enableDelay==true
            #define limitDelayTime        1000					//延迟时间限制
            #define includeTimerHead      "timer.h"				//定时器头文件
            #define getNowTimeForSerial() getClockTick()		//获取当前计数
            #define MaxTimeCount	      65535			        //计数值最大值
		#endif 





#if stms==true
    #include	"stdio.h"
#else
    #include	<iostream>
#endif

#include <cstring>
#include "MSMD5Encoder.h"
#include "MSB64Encoder.h"
#include "MSB64Decoder.h"
//---------------定义头部------------------

ui32* Pack(ui32 ); // 将uint整数转换字符串
ui32  Undo(char*); // 将字符串还原uint整数


#if stms==true
#ifdef __cplusplus
extern "C" {
#endif
#endif
ui08 toRecv(ui08); // 将发送ID转换为接收ID
ui08 toSend(ui08); // 将接收ID转换为发送ID
#if stms==true
#ifdef __cplusplus
}
#endif
#endif


class ItemPlus{	
public:
    ui08    flag;
    ui08    type;
    char   	name[5];
    //char*   data;
		char   	data[500];
    int     data_num;
    ui16    lens;
            ItemPlus();
    void 	renewItemForProc(ui08 in_flag,ui08 in_type,char *in_name,char *in_dat,int length);
		void 	deleteItemForProc();
};
		
ui08 ptrItemPlus(ItemPlus*       in_data,
                 ui08         in_nums,
                 const char * in_text);


class Item{
public:
    ui08    flag;
    ui08    type;
//#if stms==true
    char   	name[5];
//#else
//    char   	*name;
//#endif
    void*   data;
    ui32*   theNumble;
    ui16    lens;
            Item();
            Item(const char[],ui08,ui08,void*);
            Item(const char[],ui08,ui32);
    void    Edit(const char[],ui08,ui08,void*);
    bool    Ifis(const char[],ui08,ui08)const;
    void*   Text();
};

class Serial {
private:
public:
	    // -----------外部数据记录区----------
    ui16  head; //数据头=0xffff（CPY通用）
    ui32  bbps; //串口通信波特率（CPY通用）
    ui32  nums; //发送的数据包号（CPY通用）
    ui08  dest; //接收方设备编号（CPY通用）
    ui08  send; //发送方设备编号（CPY通用）
    ui08  port; //串口通信的端口（CPY通用）
    ui08  code; //串口通信的编码（CPY通用）
    ui32  time; //延时，建议为0 （CPY通用）
		#if enableDelay==true
		ui32  lastTime;
		#endif
    // -----------系统数据记录区----------
    ui32  lens; //发送的数据总长（仅限C++）
    ui32  tpln; //缓冲区数据总成（仅限C++）
    ui08  lsen; //待发送数据个数（仅限C++）
    ui08  lrec; //待接收数据个数（仅限C++）
    ui32  renm; //当前接收的包号（CPY通用）
		ui32  ackNum;  //确认号
    // -----------内部数据记录区----------
    bool  flag; //缓冲区占用标识（CPY通用）
    bool  init; //串口初始化标识（CPY通用）
    char* sedt; //待发送包头信息（CPY通用）
    char* redt; //已接受包头信息（CPY通用）
    char* buff; //串口接收缓冲区（CPY通用）
    Item**seit; //待发送对象指针（CPY通用）
		ItemPlus reitPlus[sendNumLimit];	    //接收的对象指针（CPY通用）
		
		
    // 创建:端口  速率  发送  接收 包号  延时
    Serial(ui08,ui32,ui08,ui08,ui16,ui16);

    // ------------串口控制函数------------
    char* comList(); //串口的信息（CPY通用）
    bool  comInit(); //信息初始化（CPY通用）
    bool  comLoad(); //硬件初始化（CPY通用）
    bool  comChek(); //是否被占用（CPY通用）
    bool  comSend(); //发送数据!!（CPY通用）
    bool  comRecv(char* data, uint32_t dataLength);//接收数据!!（CPY通用）
    bool  swpFree(); //清空缓冲区（CPY通用）
    bool  Restart(); //重启此串口（CPY通用）
    bool  Destroy(); //销毁此串口（CPY通用）
    // ------------解析串口获取的数据-------------
    bool  readStr(char*,ui32);
    // -------更新串口信息并且重新软初始化---------
    bool  comInfo(ui08,ui08,bool);
    // --------------用户操作函数----------------

    //展示所有等待发送对象（CPY通用）
    char* lstSend(bool);

    // --------------添加发送对象----------------
    //通过参数（CPY通用）
    void  addSend(const char[],ui08,ui08,void*);
    void  addSend(const char[],ui08,ui32);
    //通过指针（仅限C++）
    void  addSend(Item*);

    // --------------修改发送对象----------------
    //通过参数（CPY通用）
    bool  putSend(const char[],void*,ui08);

    //通过对象（CPY通用）
    bool  putSend(Item*,ui08);

    // --------------删除发送对象----------------
    //通过名称（CPY通用）
    bool  delSend(const char[]);

    //通过序列（CPY通用）
    bool  delSend(ui08);

    //通过指针（仅限C++）
    bool  delSend(Item*);
    void  delAllSend();
		
    //修改信息
    bool  changeData(uint8_t in_nums,char* strData);
    bool  changeData(uint8_t in_nums,uint32_t numData);
    bool  changeSendNum(uint8_t newSendNum);
    bool  changeRevNum(uint8_t newRevNum);
    bool  changeFlag(uint8_t in_nums,char newFlag );
    bool  changeName(uint8_t in_nums,char* newName);
    bool  changeTypeToStr(uint8_t in_nums,char* strData);
    bool  changeTypeToInt(uint8_t in_nums,uint32_t numData);
		

    // --------------获取发送对象----------------
    //通过名称（CPY通用）
    Item* getSend(ui08);             //返回对象
    // C++ STM32 Only
    ui32  getSend_int(ui08);         //返回数字
    char* getSend_str(ui08);         //返回文本
		char getRecvFlag(uint8_t in_nums);			//返回传输方式
		char getRecvType(uint8_t in_nums);			//返回数据类型
		char* getRecvName(uint8_t in_nums);			//返回数据名称
		uint8_t getRecvSendNum(uint8_t in_nums);	//返回发送方编号
		uint8_t getRecvRevNum(uint8_t in_nums);		//返回接收方编号


    //通过序列（CPY通用）
    Item* getSend    (const char[]); //返回对象
    // C++ STM32 Only
    ui32  getSend_int(const char[]); //返回数字
    char* getSend_str(const char[]); //返回文本

    // --------------接收对象操作----------------
    char* lstRecv(bool);             //展示所有

    //通过序列（CPY通用）
		ItemPlus getRecv(ui08);             //返回对象		

    // C++ STM32 Only
    ui32  getRecv_int(int);         //返回数字
    char* getRecv_str(int);         //返回文本
		
		int getRecv_str_length(char in_nums);			//返回某一文本的长度
		uint8_t getRecv_num();					//返回该次接收到多少个元素
		bool revNumIsvaild(uint8_t in_nums);			//确认获取的元素是否合法
		ui32 getRecvRenm();												//获取发送方的序列号
		uint8_t isLatestPack();
		void renewACK();
    //通过名称（CPY通用）
		ItemPlus getRecv    (const char[]); //返回对象	
		
    // C++ STM32 Only
    ui32  getRecv_int(const char[]); //返回数字
    char* getRecv_str(const char[]); //返回文本
    //-----------------------------------------

    // --------------查找某个对象---------------
    ui08  ptrItem(Item**,ui08,const char[]);
    ui08  ptrItem(Item**,ui08,Item*);
    char* lstName(Item**,ui08,bool);

    #if stms==true
        bool testcomRecv(char* tt,uint8_t tt2);
		#if enableDelay==true
		    bool SerialCouldSend();
		#endif
    #endif
		
};


#endif //MODULE_SERIAL_H
