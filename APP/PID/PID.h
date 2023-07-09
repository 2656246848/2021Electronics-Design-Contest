#ifndef APP_PID_PID_H_
#define APP_PID_PID_H_

#include "eqep.h"
#include "speed.h"

/*PID������غ�*/
#define     P               0.06f        //2.5
#define     I               0.0f        //0
#define     D               0.0f        //0

#define     P2               0.06f        //2.5
#define     I2               0.0f        //0
#define     D2               0.0f        //0

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

extern PID speed_pid;
extern PID speed_pid2;

void PID_Init();
void PID2_Init();
float Get_speed(PID *p,POSSPEED *p2);
float Get_speed2(PID *p,POSSPEED *p2);
void PID_Calc(PID *p);

#endif /* APP_PID_PID_H_ */
