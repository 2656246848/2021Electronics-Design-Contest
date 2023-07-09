#include "uart.h"
#include "stdio.h"
#include "string.h"
#include "leds.h"
/*
    * @name   fputc,fputs
    * @brief  ���ڴ�ӡ�ض���
    * @param  None
    * @retval None
    * ʵ����s%,d%�Ĵ�ӡ
*/

__interrupt void seriblRxISR(void);
// ������ջ�����
volatile Uint16 rxBuffer[BUFFER_SIZE];
Uint16 rdataA[8];    // Received data for SCI-A
Uint16 rdataB[8];    // Received data for SCI-A
volatile Uint16 r_value,l_value,r_x,l_x;


int fputc(int ch,FILE *fp)
{
    while(SciaRegs.SCICTL2.bit.TXRDY == 0);
    SciaRegs.SCITXBUF= ch;
   return ch;
}

//�ض����ӡ�ַ���
int fputs(const char *_ptr,FILE *_fp)
{
    unsigned int i,len;
    len = strlen(_ptr);
    for(i=0;i<len;i++)
    {
        while(SciaRegs.SCICTL2.bit.TXRDY == 0);
        SciaRegs.SCITXBUF= (unsigned char)(_ptr[i]);
    }
    return len;
}

void UARTa_Init(Uint32 baud)
{
    unsigned char scihbaud=0;
    unsigned char scilbaud=0;
    Uint16 scibaud=0;

    //���ݺ�������baud���õĲ����ʼ���洢�������������ĸ�8λ�͵�8λ
    scibaud=37500000/(8*baud)-1;//ͨ��ʱ��37.5MHZ
    scihbaud=scibaud>>8;
    scilbaud=scibaud&0xff;


    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;   // SCI-A
    EDIS;

    InitSciaGpio();

    //Initalize the SCI FIFO
    SciaRegs.SCIFFTX.all=0xE040;//��ʹ���ж�(��5λ)������жϱ�־λ(��6λ)�����FIFO����(��13λ)��ʹ��FIFO(��14λ)��SCI FIFO���Իָ����ܻ���(��15λ);
                                //FIFO�����ݸ���Ϊ0(��8~12λ);FIFO�����ݸ���<=0�����ж�(0~4λ)
    SciaRegs.SCIFFRX.all=0x204f; //��ʹ���ж�(��5λ)������жϱ�־λ(��6λ)�����RXFFOVF(�������)��־λ(��14λ)����λFIFOָ��(��13λ)
                                 //FIFO�����ݸ���Ϊ0(��8~12λ);FIFO�����ݸ���>=15�����ж�(0~4λ)
    SciaRegs.SCIFFCT.all=0x0;//��ʹ��FIFO������ʱ

    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    SciaRegs.SCICCR.all =0x0007;   // 1 stop bit(��7λ),  No loopback(��4λ)�� No parity(����żУ��)(��5λ),
                                    //8 char bits(0~2λ),async mode, idle-line protocol(��3λ)(����ģʽѡ�񣺿�����ģʽ)
    SciaRegs.SCICTL1.all =0x0003;  // enable TX(��0λ), RX(��1λ), internal SCICLK,
                                   // Disable RX ERR, SLEEP(��2λ), TXWAKE(��3λ)(��������ѡ��)
    SciaRegs.SCICTL2.all =0x0003;//��Ч��������
    //SciaRegs.SCICTL2.bit.TXINTENA =1;//���ͻ�����(SCITFBUF)�ж�(TXRDY flag)
    //SciaRegs.SCICTL2.bit.RXBKINTENA =1;//���ý��ջ������ж�(RXRDY flag�� RKDT flag)
    SciaRegs.SCIHBAUD    =scihbaud;  // 115200 baud @LSPCLK = 37.5MHz.
    SciaRegs.SCILBAUD    =scilbaud;
//  SciaRegs.SCICCR.bit.LOOPBKENA =1; // Enable loop back,�ػ�����ģʽ
    SciaRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset ��ʼͨ�ţ�������˯��ģʽ�ͽ��մ����жϣ���������SCIģ��ķ����жϺͽ����ж�

}

void UARTb_Init(Uint32 baud)
{
    unsigned char scihbaud=0;
    unsigned char scilbaud=0;
    Uint16 scibaud=0;

    //���ݺ�������baud���õĲ����ʼ���洢�������������ĸ�8λ�͵�8λ
    scibaud=37500000/(8*baud)-1;//ͨ��ʱ��37.5MHZ
    scihbaud=scibaud>>8;
    scilbaud=scibaud&0xff;


    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;   // SCI-B
    EDIS;

    InitScibGpio();

    //Initalize the SCI FIFO
//    ScibRegs.SCIFFTX.all=0xE040;//��ʹ���ж�(��5λ)������жϱ�־λ�����FIFO���ݣ�����SCI�Ľ��պͷ���ͨ��
//    ScibRegs.SCIFFRX.all=0x204f; //
    ScibRegs.SCIFFTX.all=0xC028;//ʹ���жϣ����յ�8λ����(0~4λ)�����ж�
    ScibRegs.SCIFFRX.all=0x0028;//ʹ���жϣ�����8λ�����ж�
    ScibRegs.SCIFFCT.all=0x0;

    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    ScibRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
    ScibRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
    ScibRegs.SCICTL2.all =0x0003;
    ScibRegs.SCICTL2.bit.TXINTENA =1;
    ScibRegs.SCICTL2.bit.RXBKINTENA =1;
    // ���ô��ڽ����ж�
    EALLOW;
    PieVectTable.SCIRXINTB = &seriblRxISR;
//    PieVectTable.SCITXINTB = &scibTxFifoIsr;
    EDIS;
    PieCtrlRegs.PIEIER9.bit.INTx3=1;
    IER |= M_INT9;
    ScibRegs.SCIHBAUD    =scihbaud;  // 9600 baud @LSPCLK = 37.5MHz
    ScibRegs.SCILBAUD    =scilbaud;
//  SciaRegs.SCICCR.bit.LOOPBKENA =1; // Enable loop back
    ScibRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset ��ʼͨ�ţ�������˯��ģʽ�ͽ��մ����жϣ���������SCIģ��ķ����жϺͽ����ж�
    ScibRegs.SCIFFTX.bit.TXFIFOXRESET=1;
    ScibRegs.SCIFFRX.bit.RXFIFORESET=1;
}

void SCIa_SendByte(int dat)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0);
    SciaRegs.SCITXBUF = dat;
}

void SCIb_SendByte(int dat)
{
    while (ScibRegs.SCIFFTX.bit.TXFFST != 0);
    ScibRegs.SCITXBUF = dat;
}

// Transmit a character from the SCI'
void UARTa_SendByte(int a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0);
    SciaRegs.SCITXBUF=a;
}

void UARTa_SendString(char * msg)
{
    int i=0;

    while(msg[i] != '\0')
    {
        UARTa_SendByte(msg[i]);
        i++;
    }
}

void UARTb_SendByte(int a)
{
    while (ScibRegs.SCIFFTX.bit.TXFFST != 0);
    ScibRegs.SCITXBUF=a;
}

void UARTb_SendString(char * msg)
{
    int i=0;

    while(msg[i] != '\0')
    {
        UARTb_SendByte(msg[i]);
        i++;
    }
}

__interrupt void seriblRxISR(void)
{
    static Uint16 bufferIndex = 0;
        static Uint16 Index2,Index3,Index4,Index5;

        // ��ȡ��������
        Uint16 receivedData = ScibRegs.SCIRXBUF.all;
        if((bufferIndex==0&&receivedData==FRAME_HEADER_1)||
                (bufferIndex==1&&receivedData==FRAME_HEADER_2)||
                    (bufferIndex>=2&&bufferIndex<=5)||
                        (bufferIndex==6 && receivedData==Index2*16+Index3+Index4-Index5)||
                             (bufferIndex==7&&receivedData==FRAME_TAIL))
        {
            bufferIndex++;
        }
        else{bufferIndex=0;r_value=0;l_value=0;}
        if(bufferIndex==3)
        {
            Index2=receivedData;
        }
        if(bufferIndex==4)
        {
            Index3=receivedData;
        }
        if(bufferIndex==5)
        {
            Index4=receivedData;
        }
        if(bufferIndex==6)
        {
            Index5=receivedData;
        }
        if(bufferIndex==8&&receivedData==FRAME_TAIL)
        {
            l_value=Index2;
            l_x=Index3;
            r_value=Index4;
            r_x=Index5;
            bufferIndex=0;
        }

        ScibRegs.SCIFFRX.bit.RXFFINTCLR=1;//����жϱ�־
        // ����жϱ�־
        PieCtrlRegs.PIEACK.all |= PIEACK_GROUP9;
    }
