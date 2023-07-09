/*
 * speed.h
 *
 *  Created on: 2023��4��16��
 *      Author: gy
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"     // Device Headerfile and Examples Include File
#include "speed.h"   // Example specific Include file

POSSPEED qep_posspeed=POSSPEED_DEFAULTS;
POSSPEED qep_posspeed2=POSSPEED_DEFAULTS;

void  POSSPEED_Init(void)
{

    EQep1Regs.QUPRD=CPU_CLK*sampling_T/1000000;
    EQep2Regs.QUPRD=CPU_CLK*sampling_T/1000000;

    EQep1Regs.QDECCTL.bit.QSRC=00;      // QEP quadrature count mode ��������ģʽ ��AB����4��Ƶ
    EQep1Regs.QEPCTL.bit.FREE_SOFT=2;  //�����ϵ�ʱλ�ü���������Ӱ��
    EQep1Regs.QEPCTL.bit.PCRM=10;       // PCRM=10 mode - QPOSCNT reset on first index event      QPOSCNT(λ�ü�����)��ʱ����Դ��QCLK
    EQep1Regs.QEPCTL.bit.UTE=1;         // Unit Timeout Enable
    EQep1Regs.QEPCTL.bit.QCLM=1;        // Latch on unit time out   ��λʱ���¼�����ʱ�������ݡ�λ�ü�����ֵ�������ʱ���Ͳ�������ֵ������QPOSLAT��QCTMRLAT��QCPRDLAT�Ĵ����У���֤���񵽵�ֵ����ͬһ��ʱ���׼�£�
    EQep1Regs.QPOSMAX=pulse_count*10;       //λ�ü����������ֵ�����Ը��ݵ��һȦ��������ȷ��.����10Ȧ����һ��
    EQep1Regs.QEPCTL.bit.QPEN=1;        // QEP ������enable

    EQep1Regs.QCAPCTL.bit.UPPS=1;       // 1/2 for unit position  2��QCLK����һ��UPEVENT����
    EQep1Regs.QCAPCTL.bit.CCPS=7;       // 1/128 for CAP clock
    EQep1Regs.QCAPCTL.bit.CEN=1;        // QEP Capture Enable
    //EQep1Regs.QEPCTL.bit.IEI=2;         //�����ź�������ʱ��ʼ������λ�ü�����
    //EQep1Regs.QEPCTL.bit.SWI=1;//�����λ eQEP ��ϵͳ������,����һ�����������������תλ�û����У׼

    EQep2Regs.QDECCTL.bit.QSRC=00;      // QEP quadrature count mode ��������ģʽ

    EQep2Regs.QEPCTL.bit.FREE_SOFT=2;
    EQep2Regs.QEPCTL.bit.PCRM=10;       // PCRM=10 mode - QPOSCNT reset on first index event      QPOSCNT(λ�ü�����)��ʱ����Դ��QCLK
    EQep2Regs.QEPCTL.bit.UTE=1;         // Unit Timeout Enable
    EQep2Regs.QEPCTL.bit.QCLM=1;        // Latch on unit time out   ��λʱ���¼�����ʱ�������ݡ�λ�ü�����ֵ�������ʱ���Ͳ�������ֵ������QPOSLAT��QCTMRLAT��QCPRDLAT�Ĵ����У���֤���񵽵�ֵ����ͬһ��ʱ���׼�£�
    EQep2Regs.QPOSMAX=pulse_count*10;       //λ�ü����������ֵ�����Ը��ݵ��һȦ��������ȷ��.����10Ȧ����һ��
    EQep2Regs.QEPCTL.bit.QPEN=1;        // QEP ������enable

    EQep2Regs.QCAPCTL.bit.UPPS=1;       // 1/2 for unit position  2��QCLK����һ��UPEVENT����
    EQep2Regs.QCAPCTL.bit.CCPS=7;       // 1/128 for CAP clock
    EQep2Regs.QCAPCTL.bit.CEN=1;        // QEP Capture Enable

}

void POSSPEED_Calc(POSSPEED *p, int eqepNum)
{

     unsigned int newp,oldp;//����M�������ٶȵ��м���

     p->DirectionQep = EQep1Regs.QEPSTS.bit.QDF;    //����

     //��ʱ���                                                   //λ�ü�������ֵ���浽QPOSLAT
     if ((eqepNum == 1 && EQep1Regs.QFLG.bit.UTO == 1) ||
             (eqepNum == 2 && EQep2Regs.QFLG.bit.UTO == 1))    // If unit timeout (one 100Hz period) T��֪��0.03s������X = QPOSLAT��k��-QPOSLAT��k-1��
    {

        if(eqepNum == 1)
        {
            newp=(unsigned int)EQep1Regs.QPOSLAT;
        }
        else if(eqepNum == 2)
        {
            newp=(unsigned int)EQep2Regs.QPOSLAT;
        }
        oldp=p->oldpos;

        if (p->DirectionQep==0)                     // POSCNT is counting down��TmpΪ����
        {
            if (newp>oldp)
                p->Pos_increment = -EQep1Regs.QPOSMAX + newp - oldp;
            else
                p->Pos_increment = newp -oldp;
        }
        else if (p->DirectionQep==1)                // POSCNT is counting up
        {
            if (newp>=oldp)
                p->Pos_increment = newp - oldp;
            else
                p->Pos_increment = EQep1Regs.QPOSMAX - oldp + newp;                     // x2-x1 should be positive
        }
        p->Speed_fr = p->Pos_increment*p->BaseRpm;              //      r/min
        //_IQ15mpy(10000,p->BaseRpm_Q)���Ƚ�10000��p->BaseRpm_Qת��Ϊlong�ͣ�Ȼ������������ٳ���2^15 (long���ֵ 4294967295)
        p->SpeedQ_fr= _IQ15mpy(p->Pos_increment*100,p->BaseRpm_Q); // ��������,Q15*Q0������õ���ʵֵ��100������ֹС���㱻ʡ��,�����ԣ���p->Pos_increment*100��100��Ϊ10ʱ��p->SpeedQ_fr��ֵʱ���������䣬��������pid����
        // Update the electrical angle
        p->oldpos = newp;
        if(eqepNum == 1)
        {
            EQep1Regs.QCLR.bit.UTO=1;                   // Clear interrupt flag
        }
        else if(eqepNum == 2)
        {
            EQep2Regs.QCLR.bit.UTO=1;                   // Clear interrupt flag
        }
    }
}
