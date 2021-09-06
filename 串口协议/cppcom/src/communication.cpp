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
	//��ʼ��������ָ�룬��ָ�������е�������һϵ�еĻ
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
	//��ʼ��������ָ�룬��ָ�������е�������һϵ�еĻ
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
	//��ʼ��������ָ�룬��ָ�������е�������һϵ�еĻ
	serial = new Serial((ui08)in_port,
		(ui32)in_bbps,
		(ui08)in_send,
		(ui08)in_dest,
		0x00,
		(ui16)0x00);
}
	
void serialAddNewSendData(const char * in_name,           //��������
	char   in_flag,       //��������
	char   in_type,       //��������
	void*  in_data) {//ʵ������
	//���һ����Ҫ���͵����ݵĶ���
	serial->addSend(in_name, in_flag, in_type, in_data);
	numOfSendData++;
}
void serialAddNewSendData(const char * in_name,           //��������
	char   in_flag,       //��������
	int in_data){//ʵ������
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
	//����Ҫ���͵����ݵı��ɾ������
	if (serial->delSend(in_nums)) {
		numOfSendData--;
		return true;
	}
	else
		return false;

}
bool serialDeleteSendDataByName(const char * in_name) {
	//����Ҫ���͵����ݵ�����ɾ������
	if (serial->delSend(in_name)) {
		numOfSendData--;
		return true;
	}
	else
		return false;

}


bool serialDeleteSendDataAll() {
	//ɾ������Ҫ���͵�����

	serial->delAllSend();
	numOfSendData=0;
	return true;
}

bool serialSendData() {
	//һ����ȫ��������
	return serial->comSend();
}
bool serialRecvData(char* data, uint32_t len) {
	//��������
	return serial->comRecv(data, len);
}
int32_t serialGetRecvInt(uint8_t in_nums) {
	//��ȡ�ôδ����õĵ�i��Ԫ�ص�����(��������Ϊ����)
	return serial->getRecv_int((char)in_nums);
	//getRecv_int
}
char* serialGetRecvStr(uint8_t in_nums){ 
	//��ȡ�ôδ����õĵ�i��Ԫ�ص�����(��������Ϊ�ַ���)
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
	//��ȡ��i��Ԫ�صĳ��� 
	//���ж��Ƿ����ַ���
	//�����ַ�����ֱ�ӷ��س��ȣ����򷵻ظ�1
	return serial->getRecv_str_length((char)in_nums);
}
uint8_t serialGetNums(){
	//�����յ���Ԫ�صĸ���
	return serial->getRecv_num();
}
bool serialRevNumIsvaild(uint8_t in_nums){
	//������Ԫ�ص������Ƿ�Ϸ�
	return serial->revNumIsvaild(in_nums);
}
void serialStop(){
	//�رմ���
	//��Ȼdestroy�Ǵݻٵ���˼������ȷʵֻ�ǹرմ���
	serial->Destroy();
}
void serialRestart(){
	//��������
	serial->Restart();
}
uint32_t serialGetSeq(){
	//��ȡ��ǰ���ͷ������к�
	return serial->getRecvRenm();
}
uint8_t isLatestPack(){
	//�жϵ�ǰ�İ��Ƿ���ϴε��øú���ʱ��
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
