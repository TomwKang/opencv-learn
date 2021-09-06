#ifndef _IMU_H_
#define _IMU_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"

/* Adjust the Kp and Ki of IMU module
   This can change the convergence speed of the IMU output */
#define BOARD_DOWN                  1
#define sampleFreq                  200.0f                  // sample frequency in Hz
#define twoKpDef                    (40.0f * 0.5f)          // 2 * proportional gain
#define twoKiDef                    (2.0f * 0.005f)         // 2 * integral gain
#define AXIS_6                      1                       // 0--9axis   1--6axis
#define IMU_TEMPERATURE_CONTROL     1                       //imu�¿أ����ܵ���Ư�ƣ�����Ժ�ʹ��
#define IMU_CONTROL_TEMPERATURE     40                      //imu�����¶�

#define ACC_X_OFFSET    -33   //
#define ACC_Y_OFFSET    77
#define ACC_Z_OFFSET    -43

#define GYRO_X_OFFSET    23     //
#define GYRO_Y_OFFSET    77
#define GYRO_Z_OFFSET    26

#define MAG_SEN         0.1f    //ת���� uT
#define MAG_X_OFFSET    -10     //�ų�ƫ�ƣ������ռ�ų���matlab����õ�
#define MAG_Y_OFFSET    1
#define MAG_Z_OFFSET    12

#define MPU6500_NSS_Low() GPIO_WriteBit(GPIOF, GPIO_Pin_6, Bit_RESET)
#define MPU6500_NSS_High() GPIO_WriteBit(GPIOF, GPIO_Pin_6, Bit_SET)

#define MPU6500_TEMP_PWM_MAX 5000 //mpu6500�����¶ȵ�����TIM������ֵ������PWM���Ϊ MPU6500_TEMP_PWM_MAX - 1
#define GYRO_CONST_MAX_TEMP 45 //�����ǿ��ƺ��� �������¶�

#define MPU6500_TEMPERATURE_FACTOR 0.002f
#define MPU6500_TEMPERATURE_OFFSET 23.0f

#define MPU6500_TEMPERATURE_PID_KP 1600.0f //�¶ȿ���PID��kp
#define MPU6500_TEMPERATURE_PID_KI 0.2f    //�¶ȿ���PID��ki
#define MPU6500_TEMPERATURE_PID_KD 0.0f    //�¶ȿ���PID��kd

#define MPU6500_TEMPERATURE_PID_MAX_OUT 4500.0f  //�¶ȿ���PID��max_out
#define MPU6500_TEMPERATURE_PID_MAX_IOUT 4400.0f //�¶ȿ���PID��max_iout

#define IMU_Temperature_PID_DEFAULT \
{\
    0,\
    0,\
    {0,0},\
    MPU6500_TEMPERATURE_PID_KP,\
    MPU6500_TEMPERATURE_PID_KI,\
    MPU6500_TEMPERATURE_PID_KD,\
    0,\
    0,\
    0,\
    0,\
    0,\
    0,\
    0,\
    MPU6500_TEMPERATURE_PID_MAX_OUT,\
    0,\
    0,\
    0,\
    &PID_Calc,\
    &PID_Reset,\
}

typedef struct{
    float ax; //unit: m/s2
    float ay;
    float az;
    
    float gx; //rad/s
    float gy;
    float gz;
    
    float mx; //unit: uT
    float my;
    float mz;

    float temp;
}imu_ripdata_t;  //�����������

typedef struct{
    float rol;
    float pit;
    float yaw;
}attitude_angle_t;  //��̬��

typedef struct{
    int16_t ax;
    int16_t ay;
    int16_t az;

    int16_t gx;
    int16_t gy;
    int16_t gz;
    
    int16_t mx;
    int16_t my;
    int16_t mz;
    
}imu_offset_t;      //ƫ��

typedef struct{
    int16_t ax;
    int16_t ay;
    int16_t az;
 
    int16_t temp;

    int16_t gx;
    int16_t gy;
    int16_t gz;
    
    int16_t mx;     //unit: mG
    int16_t my;
    int16_t mz;
}imu_rawdata_t;     //ԭʼ����

typedef struct{
    attitude_angle_t atti;
    imu_ripdata_t rip;
    imu_rawdata_t raw;
    imu_offset_t  offset;
}imu_t;

extern void imu_init(void);              //imu��ʼ��
extern void imu_main(void);              //imu��ȡ��̬��������

extern float imu_yaw_angle;              //yaw�Ƕ� ��
extern float imu_yaw_angular_speed;      //yaw���ٶ� ��/s
extern float imu_pitch_angular_speed;    //pitch���ٶ� ��/s


#ifdef __cplusplus
		}
#endif
#endif
