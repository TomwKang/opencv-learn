#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_
#include "ModuleSerial.h"
extern Serial myserial;
#if useObject==0
bool serialChangeData(uint8_t in_nums,char* strData);
bool serialChangeData(uint8_t in_nums,int numData);	
void serialAddNewSendData(const char * in_name,           //��������
	char   in_flag = 'G',       //��������
	char   in_type = 'G',       //��������
	void*  in_data = nullptr); //ʵ������
void serialAddNewSendData(const char * in_name,           //��������
	char   in_flag,       //��������
	int  	 in_data); //ʵ������
void initSerial(ui08 in_port,
	ui32 in_bbps ,
	ui08 in_send ,
	ui08 in_dest ,
	ui16 in_nums ,
	ui16 in_time );

void initSerial(int in_port,
	int in_bbps ,
	int in_send ,
	int in_dest ,
	int in_time );

void initSerial(int in_port,
	int in_bbps,
	int in_send,
	int in_dest);
#ifdef __cplusplus
extern "C" {
#endif




	

bool serialDeleteSendData(uint8_t in_nums = 0);
bool serialChangeSendNum(uint8_t newSendNum);
bool serialChangeRevNum(uint8_t newRevNum);
bool serialChangeFlag(uint8_t in_nums,char newFlag );
bool serialChangeName(uint8_t in_nums,char* newName);
bool serialChangeTypeToStr(uint8_t in_nums,char* strData);
bool serialChangeTypeToInt(uint8_t in_nums,int numData);

bool serialDeleteSendDataByName(const char * in_name);
bool serialDeleteSendDataAll();
bool serialSendData();
bool serialRecvData(char* data, uint32_t len);

int32_t serialGetRecvInt(uint8_t in_nums);
char* serialGetRecvStr(uint8_t in_nums);
char serialGetRecvFlag(uint8_t in_nums);
char serialGetRecvType(uint8_t in_nums);
char* serialGetRecvName(uint8_t in_nums);
uint8_t serialGetRecvSendNum(uint8_t in_nums);
uint8_t serialGetRecvRevNum(uint8_t in_nums);
uint32_t serialGetSeq();
uint8_t isLatestPack();
void renewACK();

int serialGetStrLength(uint8_t in_nums);
uint8_t serialGetNums();
bool freeBuffer();
bool serialRevNumIsvaild(uint8_t in_nums);

void serialStop();
void serialRestart();
#ifdef __cplusplus
}
#endif

#endif
#endif
