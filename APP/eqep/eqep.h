/*
 * eqep.h
 *
 *  Created on: 2023��4��16��
 *      Author: gy
 */

#ifndef APP_EQEP_EQEP_H_
#define APP_EQEP_EQEP_H_

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"
#include "speed.h"
#include "stdio.h"

#define SP        CPU_CLK/(2*PWM_CLK)       //���¼���ģʽ�������ʽ
#define PWM_CLK   5000             // (300rpm) EPWM1 frequency. Freq. can be changed here
#define TBCTLVAL  0x200E           // up-down count, timebase=SYSCLKOUT

void EQEP1_Init(void);
void EPwm1Setup(void);
void EPwm1A_SetCompare(Uint16 val);
void EPwm1B_SetCompare(Uint16 val);

#endif /* APP_EQEP_EQEP_H_ */
