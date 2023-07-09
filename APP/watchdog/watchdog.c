#include "watchdog.h"
__interrupt void wakeint_isr(void);
extern volatile Uint16 va;
extern volatile Uint16 va2;

/*���Ź���������Timeout period = 1 / (OSCCLK / (512 * prescaler * 256) ).
 * OSCCLK ������Ƶ�ʣ�30MHZ
 * prescaler��Ԥ��Ƶϵ����WDCR��0~2λ����
 * ʱ��������:Timeout period/150M
 * ����������������ɲ������512��OSCCLKʱ�ӵ������źš����ź���Ϊ��λ�ź�WDRST�����ж��ź�WDINT��SCSR��WDENNIT(SCSR�ĵ�1λ)������
*/
void WatchDog_init()
{
       GpioDataRegs.GPASET.bit.GPIO0=1;
       ServiceDog();
       EALLOW;
       PieVectTable.WAKEINT = &wakeint_isr; //ָ���ж���ں���
       EDIS;

       PieCtrlRegs.PIECTRL.bit.ENPIE = 1;      // Enable the PIE block
       PieCtrlRegs.PIEIER1.bit.INTx8 = 1;      // Enable PIE Group 1 INT8
       IER |= M_INT1;                          // Enable CPU int1
       EINT;                                   // Enable Global Interrupts

       // Reset the watchdog counter
       ServiceDog();

       // Enable the watchdog
       EALLOW;
       //����Ƶʱ���Ź��ж�(��ϵͳ��λ)��Ӧʱ����̣�Ϊ1/(30M/512)*2^8S=1/(30/512)*0.256mS=4.37ms
       //��WDPSΪ111ʱ��64��Ƶ���Ź��ж���Ӧʱ�����1/(30M/512/64)*2^8S=1/(30/512/64)*0.256mS=279.62ms
       SysCtrlRegs.WDCR = 0x002F;//���Ź����ƼĴ�����16λ����64��Ƶ(0-2λ��111);WDCHKΪ101(3-5λ��101);ʹ�ܿ��Ź�(��6λ��0);���Ź���λ��־λ����(����λ��0)
       //SysCtrlRegs.WDCR = 0x0028;//0��Ƶ
       SysCtrlRegs.SCSR = 0x0002;//ʹ�ܿ��Ź��ж�(��2λΪ0);����һλд1��ʾ����������������ж��źŶ��Ǹ�λ�ź�,��0λ(WDOVERRIDE)д��1�󽫲������ÿ��ƼĴ���WDCR��WDDI(���Ź�ʧ��λ)
       EDIS;
       GpioDataRegs.GPACLEAR.bit.GPIO0=1;
       //DisableDog();���ÿ��Ź�
}

// wakeint_isr -
//
__interrupt void wakeint_isr(void)
{
    va = 0;
    va2 = 0;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}
