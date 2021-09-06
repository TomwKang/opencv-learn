#include "auto_aim.h"
#include "sys.h"
/***************************����Ԥ�⺯��**************************/
GMAngle_t aimProcess(float yaw,float pit,int16_t *tic){
/*@������������Ԥ�⼰��׹����
    ���������ԽǶ�yaw��pit����ʱ����ַ
    ����˼�룺
    1.�Ӿ�������Ҫ����ʵ�Ƕȱ궨���������ΪĿ����ԽǶȣ���ֳ��ٶ�
    2.�Ӿ����ݴ�������һ��ʱ��������(��������ʵ��ѧ���ģ��߿��ؿ�����)
    3.��ѭ�����ٶ������ۼ���ָ��˥���Կ�
    4.����pitch��������׹����
*/
    #define amt 3                //��������amount������amtʹʱ������ԼΪ50ms���� amt=50ms/1000ms*fps
    static int8_t i, lock;        //���������״ν��뱣����
    static float y[amt], p[amt],  //yaw,pit��ʷ
                 tSum, t[amt],    //���ʱ��,tic��ʷ
                 wy, wp,          //yaw,pit��ʷ
                 wySum, wpSum;    //���ٶ��ۼӶԿ�
    static GMAngle_t in, out;     //��һ��ֵ������ֵ�Ƕ�
    
    tSum += *tic - t[i];          //��pid��i�������һ�ޣ����ϱ��β���ȥamt����ǰ��ʱ�������õ���Ƶ��ļ��
    if(*tic > 150){               //if��������ʱ��������100*2ms�������ʷ�����뱣����
        lock = amt;
        wy = 0; wp = 0;
        in.yaw = yaw;in.pitch = pit;
        out.yaw = yaw;out.pitch = pit;
    }
    in.yaw = (yaw + in.yaw) / 2;       //����ֵ�˲�
    in.pitch = (pit + in.pitch) / 2;
    if(lock){
        lock--;
    }            //�����״ν��뱣����ֻ��¼���ݲ�Ԥ��
    else{
        wy = (wy + (in.yaw - y[i]) / tSum) / 2;    //�ٶ��˲�
        wp = (wp + (in.pitch - p[i]) / tSum) / 2;
        wySum += wy;      //���ٶ��ۼ���ָ��˥���Կ�
        wpSum += wp;
        wySum *= 0.9f;    //ָ��˥�������ۼ�,ʧȥ��������
        wpSum *= 0.9f;
    }
    y[i] = in.yaw;        //yaw��ʷ
    p[i] = in.pitch;      //pit��ʷ
    t[i] = *tic;          //tic��ʷ
    i = (i + 1) % amt;    //amt��֮��ѭ��
    out.yaw = (in.yaw + wySum * 20 + out.yaw) / 2;        //ʵ��Ԥ��
    out.pitch = (in.pitch + wpSum * 10 + out.pitch) / 2;
//    angle.pit- = 40/angle.pit-0.4;//������׹����
    *tic = 1;            //ʱ���жϼ�ʱ�����¿�ʼ
    return out;
}
/***************************************************************************************/
