#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"
#include "main.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 startup_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/* UART 快速初始化结构所支持的引脚* 使用时还是推荐标准初始化 */
/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/
 
 /*
     实验名称：UART打印信息
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：使用串口UART将芯片的出厂信息在芯片上电后发送出去
        发送完毕后，进入while中，执行小灯闪烁效果
*/

/* 串口接收中断回调函数
   在函数中写中断想要做的事情
*/
static void UART_RX_ISR(uint16_t byteReceived)
{
    /* 将接收到的数据发送回去 */
    UART_WriteByte(HW_UART1, byteReceived);
}

int main(void)
{
    uint32_t UID_buf[4];
    uint8_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 1, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);//打印信息口，printf会自动选择第一个初始化的串口

    UART_QuickInit(UART1_RX_PE01_TX_PE00, 115200);
    /*  配置UART 中断配置 打开接收中断 安装中断回调函数 */
    UART_CallbackRxInstall(HW_UART1, UART_RX_ISR);
    /* 打开串口接收中断功能 IT 就是中断的意思*/
    UART_ITDMAConfig(HW_UART1, kUART_IT_Rx, true);

    DelayMs(10);
    /* 打印芯片信息 */
    LOG("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());
    /* 打印时钟频率 */
    LOG("core clock:%dHz\r\n", GetClock(kCoreClock));
    LOG("bus clock:%dHz\r\n", GetClock(kBusClock));
    CPUIDY_GetUID(UID_buf);
    LOG("UID:0x");
    for(i=0;i<4;i++)
    {
        LOG("%04x", UID_buf[3-i]);
    }
    LOG("\r\n");
    
    while(1)
    {
        /* 闪烁小灯 */
        GPIO_ToggleBit(HW_GPIOC, 1);
        DelayMs(500);
    }
}


