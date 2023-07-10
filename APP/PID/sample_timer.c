/*
 * sample_timer.c
 *
 *  Created on: 2023��5��29��
 *      Author: gy
 */
#include "sample_timer.h"
#include "PID.h"
#include "leds.h"


volatile Uint16 va = 0;//(���15000)
volatile Uint16 va2 = 0;//(���15000)
//int interrupt_count;
//��ʱ��2��ʼ������
//Freq��CPUʱ��Ƶ�ʣ�150MHz��
//Period����ʱ����ֵ����λus

void TIM2_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1; // CPU Timer 2
    EDIS;

    //���ö�ʱ��2���ж���ڵ�ַΪ�ж��������TINT2
    EALLOW;
    PieVectTable.TINT2 = &TIM2_IRQn;
    EDIS;

    //ָ��ʱ��2�ļĴ�����ַ
    CpuTimer2.RegsAddr = &CpuTimer2Regs;
    //���ö�ʱ��2�����ڼĴ���ֵ
    CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;
    //���ö�ʱ��Ԥ���������ֵΪ0
    CpuTimer2Regs.TPR.all  = 0;
    CpuTimer2Regs.TPRH.all = 0;
    //ȷ����ʱ��2Ϊֹͣ״̬
    CpuTimer2Regs.TCR.bit.TSS = 1;
    //����ʹ��
    CpuTimer2Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer2.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer2, Freq, Period);

    //��ʼ��ʱ������
    CpuTimer2Regs.TCR.bit.TSS=0;
    //��ʱ��2ֱ�������ں��жϣ���������PIE
    IER |= M_INT14;

    //ʹ�����ж�
    EINT;
    ERTM;

}

interrupt void TIM2_IRQn(void)
{
    EALLOW;
    Get_speed(&speed_pid1, &qep_posspeed);
    Get_speed(&speed_pid2, &qep_posspeed2);
    PID_Calc(&speed_pid1);
    PID_Calc(&speed_pid2);

    if (speed_pid1.OUT<SP*2/3)//�������ռ�ձ�
    {
        va += speed_pid1.OUT;
    }
    if (speed_pid2.OUT<SP*2/3)//�������ռ�ձ�
   {
      va2 += speed_pid2.OUT;
  }
    EPwm1B_SetCompare(va2);//IO1
    EPwm1A_SetCompare(va);//IO0
    CpuTimer2Regs.TCR.bit.TSS=0;//������ʱ��
    EDIS;
}
