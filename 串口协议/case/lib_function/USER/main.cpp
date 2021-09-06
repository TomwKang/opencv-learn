#include "main.h"
#include "bsp.h"
#include "imu.h"
#include "gun.h"
#include "beep.h" 
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "usmart.h"
#include "remote_task.h"
#include "can_bus_task.h"
#include "control_task.h"
#include "visualscope.h"                              
#include "minipc_task.h"
#include "communication.h"
#include "init_task.h"
#include "usart6.h"
#include <rt_heap.h>	//_init_alloc

int main(void) {
	BSP_Pre_Init();
	usmart_dev.init(SystemCoreClock / 1000000);//使用的是timer4
	if (Check_Sum > 1) {
		printf("Check your Monitors!\r\n");
		Sing_bad_case();
	}
	BSP_Init();

	Task_Init();
	
	delay_xms(10);
	serialAddNewSendData("TNOW", 'P','S', (void *)"0");
			serialAddNewSendData("TNOW", 'P', 0);					//当前位置
		serialAddNewSendData("YNOW", 'P', 0);					//pitch轴角度
	//	serialAddNewSendData("PNOW", 'P', 0);					//yaw轴角度		
		
	vTaskStartScheduler();
		

	while (1) {
		//        printf("yaw_raw_value:%6.6d  pit_raw_value:%6.6d  yaw_ecd:%6.6f  pit_ecd:%6.6f\r\n",
		//        GMYawEncoder.raw_value, GMPitchEncoder.raw_value,GMYawEncoder.ecd_angle, GMPitchEncoder.ecd_angle);
		//        printf("heat:%d, bullet_speed:%f  %d, %d\r\n",Get_Sentry_HeatData(),Get_Sentry_BulletSpeed(),available_bullet_num, launched_bullet_num);
		//        VisualScope(USART3, GMYPositionPID.fdb,GMYPositionPID.ref, GMYSpeedPID.fdb, GMYSpeedPID.ref);
		//        VisualScope(USART3, RAMMERSpeedPID.fdb,RAMMERSpeedPID.ref,GMYPositionPID.fdb,GMYPositionPID.ref);
		//					VisualScope(USART3, GMPPositionPID[0].ref,GMPPositionPID[0].fdb,GMPSpeedPID[0].ref,(GET_PITCH_ANGULAR_SPEED));
		//        VisualScope(USART3, get_imu_wz(),get_imu_wy(), get_yaw_angle(), get_pit_angle());
		//        VisualScope(USART3, GMYPositionPID.fdb,GMYPositionPID.ref, imu.atti.yaw, imu_yaw_angle);
		//    printf("pit:%f\r\n", GET_PITCH_ANGLE);
		imu_main();
		
		

		
		

//		if (Get_Flag(Gimble_ok) == 1) {
//			miniPC_task();
//		}
		delay_xms(2);
	}
}
