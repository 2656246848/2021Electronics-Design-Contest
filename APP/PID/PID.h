#ifndef APP_PID_PID_H_
#define APP_PID_PID_H_

#include "eqep.h"
#include "speed.h"

/*����*/
typedef struct
{
    float P;
    float I;
    float D;
} PIDConstants;

/*PID�ṹ��*/
typedef struct
{
    float sv;//�趨ֵ
    float pv;//ʵ��ֵ
    float Kp;
    float Kd;
    float Ki;
    float EK;//����ƫ��
    float Last_EK;//�ϴ�ƫ��
    long SEK;//��ʷƫ��֮��
    float OUT;
    float system_scale;

}PID;

extern PID speed_pid1;
extern PID speed_pid2;
extern PIDConstants pidParams1;
extern PIDConstants pidParams2;

void PID_Init(PID *p,PIDConstants *p2,float Set_speed);
float Get_speed(PID *p,POSSPEED *p2);
float Get_speed2(PID *p,POSSPEED *p2);
void PID_Calc(PID *p);

#endif /* APP_PID_PID_H_ */
