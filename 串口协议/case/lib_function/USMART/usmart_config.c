#include "usmart.h"
#include "usmart_str.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
#include "delay.h" 
#include "sys.h"
#include "debug_pid.h"

//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
    (void*)GMP_PID_PLUS,"void GMP_PID_PLUS(int x,int y);",
    (void*)GMP_PID_MIN,"void GMP_PID_MIN(int x,int y);",
    (void*)GMY_PID_PLUS,"void GMY_PID_PLUS(int x,int y);",
    (void*)GMY_PID_MIN,"void GMY_PID_MIN(int x,int y);",
    (void*)RAMMER_PID_PLUS,"void RAMMER_PID_PLUS(int x,int y);",
    (void*)RAMMER_PID_MIN,"void RAMMER_PID_MIN(int x,int y);"
        
};
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������

struct _m_usmart_dev usmart_dev=
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
    0,          //��������
    0,         //����ID
    1,        //������ʾ����,0,10����;1,16����
    0,        //��������.bitx:,0,����;1,�ַ���
    0,          //ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
    0,        //�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};

