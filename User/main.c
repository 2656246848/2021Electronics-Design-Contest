#include "main.h"
#include "INIT.h"

volatile Uint16 va = 0;//(���15000)
volatile Uint16 va2 = 0;//(���15000)

void main()
 {
    #if RUN_TYPE==FLASH_RUN
        MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
        InitFlash();
    #endif
    SYSTERM_INIT();

    if(1)
    {
        OLED_DisShowCHinese(56,5,2);
        OLED_DisShowCHinese(74,5,3);
        OLED_DisShowCHinese(92,5,4);
        OLED_ShowString(0,0,"freq:150kHz",16);
        OLED_ShowString(90,0,"IO0",16);
        OLED_ShowString(0,2,"duty:",16);
        OLED_ShowNum(70,2,va/150,2,2,16);
        OLED_ShowString(0,4,"K1/K2",16);
        OLED_ShowString(0,6,"10/1",16);
    }

    #if RUN_TYPE==SRAM_RUN
        // ����
        printf("Hello World\n");

    #endif

    while (1)
    {
        if(Timer_10ms_Task==1) //10msɨ��һ��
        {
            Timer_10ms_Task=0;
            //printf("%d,%d,%d,%d\n",(int)(speed_pid.pv),(int)speed_pid.sv,(int)speed_pid.OUT,va);
            printf("%d,%d,%d,%d\n",(int)(speed_pid2.pv),(int)speed_pid2.sv,(int)speed_pid2.OUT,va2);
        }
        if(Timer_50ms_Task==1) //50msɨ��һ��
        {
            Timer_50ms_Task=0;
            ServiceDog();//ι����ÿ279.62ms/4.37ms��Ҫιһ�Σ���������Ź��ж�
        }
        if(Timer_1s_Task==1)   //1sɨ��һ��
        {
            Timer_1s_Task=0;
            LED2_TOGGLE;
        }
    }

}
