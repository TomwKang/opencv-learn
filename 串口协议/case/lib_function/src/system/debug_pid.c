#include "debug_pid.h"
#include "control_task.h"
#include "sys.h"
#include "usart3.h"

void Monitor_Pitch_PID(void)
{
    printf("PKP1:%3.2f,PKI1:%1.2f,PKD1:%3.2f,VKP1:%3.2f,VKI1:%1.2f,VKD1:%3.2f\r\n",
    GMPPositionPID[0].kp,GMPPositionPID[0].ki,GMPPositionPID[0].kd,GMPSpeedPID[0].kp,GMPSpeedPID[0].ki,GMPSpeedPID[0].kd);	
    printf("PKP2:%3.2f,PKI2:%1.2f,PKD2:%3.2f,VKP2:%3.2f,VKI2:%1.2f,VKD2:%3.2f\r\n",
    GMPPositionPID[1].kp,GMPPositionPID[1].ki,GMPPositionPID[1].kd,GMPSpeedPID[1].kp,GMPSpeedPID[1].ki,GMPSpeedPID[1].kd);	

}
void Monitor_Yaw_PID(void)
{
    printf("PKP:%3.2f,PKI:%1.2f,PKD:%3.2f,VKP:%3.2f,VKI:%1.2f,VKD:%3.2f\r\n",
    GMYPositionPID.kp,GMYPositionPID.ki,GMYPositionPID.kd,GMYSpeedPID.kp,GMYSpeedPID.ki,GMYSpeedPID.kd);
}
void Monitor_Rammer_PID(void)
{
    printf("VKP1:%3.2f,VKI:%1.2f,VKD:%3.2f\r\n",RAMMERSpeedPID[0].kp,RAMMERSpeedPID[0].ki,RAMMERSpeedPID[0].kd);
	printf("VKP2:%3.2f,VKI:%1.2f,VKD:%3.2f\r\n",RAMMERSpeedPID[1].kp,RAMMERSpeedPID[1].ki,RAMMERSpeedPID[1].kd);
}
void GMP_PID_PLUS(int x,int y)
{
	for(int i=0;i<2;i++)
    switch(x){
    case 0x1:
        GMPPositionPID[i].kp += (float)y/100;
        break;
    case 0x2:
        GMPPositionPID[i].ki += (float)y/100;
        break;
    case 0x3:
        GMPPositionPID[i].kd += (float)y/100;
        break;
    case 0x4:
        GMPSpeedPID[i].kp += (float)y/100;
        break;
    case 0x5:
        GMPSpeedPID[i].ki += (float)y/100;
        break;
    case 0x6:
        GMPSpeedPID[i].kd += (float)y/100;
				break;
    default :
        ;
    }
    Monitor_Pitch_PID();
		
		
}
void GMP_PID_MIN(int x,int y)
{
		for(int i=0;i<2;i++)
    switch(x){
    case 0x1:
        GMPPositionPID[i].kp -= (float)y/100;
        break;
    case 0x2:
        GMPPositionPID[i].ki -= (float)y/100;
        break;
    case 0x3:
        GMPPositionPID[i].kd -= (float)y/100;
        break;
    case 0x4:
        GMPSpeedPID[i].kp -= (float)y/100;
        break;
    case 0x5:
        GMPSpeedPID[i].ki -= (float)y/100;
        break;
    case 0x6:
        GMPSpeedPID[i].kd -= (float)y/100;
        break;
    default :
        ;
    }

    Monitor_Pitch_PID();
}
void GMY_PID_PLUS(int x,int y)
{
    switch(x){
    case 0x1:
        GMYPositionPID.kp += (float)y/100;
        break;
    case 0x2:
        GMYPositionPID.ki += (float)y/100;
        break;
    case 0x3:
        GMYPositionPID.kd += (float)y/100;
        break;
    case 0x4:
        GMYSpeedPID.kp += (float)y/100;
        break;
    case 0x5:
        GMYSpeedPID.ki += (float)y/100;
        break;
    case 0x6:
        GMYSpeedPID.kd += (float)y/100;
        break;
    default :
        ;
    }
    Monitor_Yaw_PID();
}
void GMY_PID_MIN(int x,int y)
{
    switch(x){
    case 0x1:
        GMYPositionPID.kp -= (float)y/100;
        break;
    case 0x2:
        GMYPositionPID.ki -= (float)y/100;
        break;
    case 0x3:
        GMYPositionPID.kd -= (float)y/100;
        break;
    case 0x4:
        GMYSpeedPID.kp -= (float)y/100;
        break;
    case 0x5:
        GMYSpeedPID.ki -= (float)y/100;
        break;
    case 0x6:
        GMYSpeedPID.kd -= (float)y/100;
        break;
    default :
        ;
    }
    Monitor_Yaw_PID();
}

void RAMMER_PID_PLUS(int x,int y)
{
		for(int i=0;i<2;i++)
    switch(x){
    case 0x1:
        RAMMERSpeedPID[i].kp += (float)y/100;
        break;
    case 0x2:
        RAMMERSpeedPID[i].ki += (float)y/100;
        break;
    case 0x3:
        RAMMERSpeedPID[i].kd += (float)y/100;
        break;
    default :
        ;
    }
    Monitor_Rammer_PID();
}
void RAMMER_PID_MIN(int x,int y)
{
	for(int i=0;i<2;i++)
    switch(x){
    case 0x1:
        RAMMERSpeedPID[i].kp -= (float)y/100;
        break;
    case 0x2:
        RAMMERSpeedPID[i].ki -= (float)y/100;
        break;
    case 0x3:
        RAMMERSpeedPID[i].kd -= (float)y/100;
        break;
    default :
        ;
    }
    Monitor_Rammer_PID();
}
