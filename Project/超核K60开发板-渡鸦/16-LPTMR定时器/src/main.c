#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "lptmr.h"

/* LPTMR 中断回调函数 */
static void LPTMR_ISR(void)
{
    static uint32_t i;
    printf("lptmr interrupt:%d\r\n", i++);
    GPIO_ToggleBit(HW_GPIOE, 6);
}

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); /* 为了代码简单 这里直接调用快速初始化了 */
    
    printf("lptrm timer counter test\r\n");
    
    /* 快速初始化 LPTMR 用作时间计数 */
    LPTMR_TC_InitTypeDef LPTMR_TC_InitStruct1;
    LPTMR_TC_InitStruct1.timeInMs = 1000; /* 定时时间 1S */
    LPTMR_TC_Init(&LPTMR_TC_InitStruct1);
    
    LPTMR_CallbackInstall(LPTMR_ISR); /* 注册回调函数 */
    LPTMR_ITDMAConfig(kLPTMR_IT_TOF); /* 开启LPTMR中断 */
    
    while(1)
    {
        /* 等待中断 */
    }
}


