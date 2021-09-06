#ifndef _PID_REGULATOR_H_
#define _PID_REGULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif
#define Abs(x) if(x<0) return -x else return x; 

typedef struct PID_Regulator_t{
	float ref;
	float fdb;
	float err[2];
	float kp;
	float ki;
	float kd;
	float componentKp;
	float componentKi;
	float componentKd;
	float componentKpMax;
	float componentKiMax;
	float componentKdMax;
	float output;
	float outputMax;
	float kp_offset;
	float ki_offset;
	float kd_offset;
	void  (*Calc)(struct PID_Regulator_t *pid);
	void  (*Reset)(struct PID_Regulator_t *pid);
}PID_Regulator_t;


typedef struct PID_Regulator_IncomD_t{
	float ref;
	float fdb;
	float err[2];
	float kp;
	float ki;
	float kd;
	float componentKp;
	float componentKi;
	float componentKd;
	float componentKpMax;
	float componentKiMax;
	float componentKdMax;
	float output;
	float outputMax;
	float kp_offset;
	float ki_offset;
	float kd_offset;
	float pid_differential;
	float alpha;
	void  (*Calc)(struct PID_Regulator_IncomD_t *pid);
	void  (*Reset)(struct PID_Regulator_IncomD_t *pid);
}PID_Regulator_IncomD_t;

void PID_Reset(PID_Regulator_t *pid);
void PID_Calc(PID_Regulator_t *pid);
void PID_Calc_IncomD(PID_Regulator_IncomD_t *pid);   //����ȫ΢��pid
void PID_Reset_IncomD(PID_Regulator_IncomD_t *pid);
#ifdef __cplusplus
}
#endif
#endif
