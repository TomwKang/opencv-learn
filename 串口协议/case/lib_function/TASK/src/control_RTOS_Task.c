#include "control_RTOS_Task.h"
#include "control_task.h"
#include "can_bus_task.h"
#include "remote_task.h"
uint16_t timeForDelay = 7000;
static uint16_t clockTick = 0;

void control_RTOS_Task(void*p_arg) {
	portTickType currentTime;
	while (1)
	{
		currentTime = xTaskGetTickCount();	//��ȡ��ǰϵͳʱ��
		Control_Task();
		sendToClassis_test();
		timeForDelay++;
		clockTick++;
		vTaskDelayUntil(&currentTime, CONTROL_RTOS_TASK_PERIOD/portTICK_RATE_MS);			//��ʱ
	}
}

void resetTimeForDelay() {
	timeForDelay = 0;
}
uint16_t getTimeForDelay() {
	return timeForDelay;
}
uint16_t getClockTick() {
	return clockTick;
}
