#if useObject==0
#include "communication.h"
Serial *serial;
uint8_t numOfSendData = 0;
uint32_t ackNum=0;
void initSerial(ui08 in_port,
	ui32 in_bbps,
	ui08 in_send,
	ui08 in_dest,
	ui16 in_nums,
	ui16 in_time ) {
	//初始化串口类指针，该指针用来承担接下来一系列的活动
	serial = new Serial(in_port,
		in_bbps,
		in_send,
		in_dest,
		in_nums,
		in_time);
}
	

void initSerial(int in_port,
	int in_bbps,
	int in_send,
	int in_dest,
	int in_time ) {
	//初始化串口类指针，该指针用来承担接下来一系列的活动
	serial = new Serial((ui08)in_port,
		(ui32)in_bbps,
		(ui08)in_send,
		(ui08)in_dest,
		0x00,
		(ui16)in_time);
}

void initSerial(int in_port,
	int in_bbps,
	int in_send,
	int in_dest) {
	//初始化串口类指针，该指针用来承担接下来一系列的活动
	serial = new Serial((ui08)in_port,
		(ui32)in_bbps,
		(ui08)in_send,
		(ui08)in_dest,
		0x00,
		(ui16)0x00);
}
	
void serialAddNewSendData(const char * in_name,           //变量名称
	char   in_flag,       //传输类型
	char   in_type,       //数据类型
	void*  in_data) {//实际内容
	//添加一个需要发送的数据的对象
	serial->addSend(in_name, in_flag, in_type, in_data);
	numOfSendData++;
}
void serialAddNewSendData(const char * in_name,           //变量名称
	char   in_flag,       //传输类型
	int in_data){//实际内容
	serial->addSend(in_name, in_flag, (uint32_t)in_data);
	numOfSendData++;
}
	
bool serialChangeData(uint8_t in_nums,char* strData){
	return serial->changeData(in_nums,strData);
}

bool serialChangeData(uint8_t in_nums,int numData){
		return serial->changeData(in_nums,(uint32_t)numData);
}


bool serialChangeSendNum(uint8_t newSendNum){
	return serial->changeSendNum(newSendNum);
}
bool serialChangeRevNum(uint8_t newRevNum){
	return serial->changeRevNum(newRevNum);
}
bool serialChangeFlag(uint8_t in_nums,char newFlag ){
	return serial->changeFlag(in_nums,newFlag);
}
bool serialChangeName(uint8_t in_nums,char* newName){
	return serial->changeName(in_nums,newName);
}
bool serialChangeTypeToStr(uint8_t in_nums,char* strData){
	return serial->changeTypeToStr(in_nums,strData);
}
bool serialChangeTypeToInt(uint8_t in_nums,int numData){
	//changeTypeToInt
	return serial->changeTypeToInt(in_nums,numData);
}

bool serialDeleteSendData(uint8_t in_nums) {
	//根据要发送的数据的编号删除数据
	if (serial->delSend(in_nums)) {
		numOfSendData--;
		return true;
	}
	else
		return false;

}
bool serialDeleteSendDataByName(const char * in_name) {
	//根据要发送的数据的名称删除数据
	if (serial->delSend(in_name)) {
		numOfSendData--;
		return true;
	}
	else
		return false;

}


bool serialDeleteSendDataAll() {
	//删除所有要发送的数据

	serial->delAllSend();
	numOfSendData=0;
	return true;
}

bool serialSendData() {
	//一次性全发送数据
	return serial->comSend();
}
bool serialRecvData(char* data, uint32_t len) {
	//接收数据
	return serial->comRecv(data, len);
}
int32_t serialGetRecvInt(uint8_t in_nums) {
	//获取该次传输获得的第i个元素的数据(若该数据为整型)
	return serial->getRecv_int((char)in_nums);
	//getRecv_int
}
char* serialGetRecvStr(uint8_t in_nums){ 
	//获取该次传输获得的第i个元素的数据(若该数据为字符串)
	return serial->getRecv_str((char)in_nums);
}

char serialGetRecvFlag(uint8_t in_nums){
	return serial->getRecvFlag(in_nums);
}
char serialGetRecvType(uint8_t in_nums){
	return serial->getRecvType(in_nums);
}
char* serialGetRecvName(uint8_t in_nums){
	//getRecvName
	return serial->getRecvName(in_nums);
}
bool freeBuffer() {
	return serial->swpFree();
}
int serialGetStrLength(uint8_t in_nums){
	//获取第i个元素的长度 
	//会判断是否是字符串
	//若是字符串，直接返回长度，否则返回负1
	return serial->getRecv_str_length((char)in_nums);
}
uint8_t serialGetNums(){
	//返回收到总元素的个数
	return serial->getRecv_num();
}
bool serialRevNumIsvaild(uint8_t in_nums){
	//检查接收元素的索引是否合法
	return serial->revNumIsvaild(in_nums);
}
void serialStop(){
	//关闭串口
	//虽然destroy是摧毁的意思，但是确实只是关闭串口
	serial->Destroy();
}
void serialRestart(){
	//重启串口
	serial->Restart();
}
uint32_t serialGetSeq(){
	//获取当前发送方的序列号
	return serial->getRecvRenm();
}
uint8_t isLatestPack(){
	//判断当前的包是否比上次调用该函数时新
	/*
	if(serial->getRecvRenm()>=ackNum&&serial->getRecv_num()>0){
		ackNum=serial->getRecvRenm()+1;
		return 1;
	}else
		return 0;
	*/
	return serial->isLatestPack();
}
void renewACK(){
	ackNum=serial->getRecvRenm()+1;
}
#endif
