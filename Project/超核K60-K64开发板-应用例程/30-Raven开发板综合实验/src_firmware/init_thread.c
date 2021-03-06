#include <rtthread.h>
#include "board.h"
#include "components.h"
#include "chlib_k.h"
#include "IS61WV25616.h"
#include "rtt_drv.h"
#include "pin.h"


void rt_system_comonent_init(void);
void usb_thread_entry(void* parameter);


//static uint8_t INIT_STACK[1024*13];

void rt_heap_init(void)
{
    int ret;
    SRAM_Init();
    ret = SRAM_SelfTest();
    if(ret)
    {
    //    rt_system_heap_init((void*)INIT_STACK, (void*)(sizeof(INIT_STACK) + (uint32_t)INIT_STACK));
    }
    else
    {
        rt_system_heap_init((void*)(SRAM_ADDRESS_BASE), (void*)(SRAM_ADDRESS_BASE + SRAM_SIZE));
    }
}

void init_thread(void* parameter)
{
    rt_thread_t tid;

    rt_system_comonent_init();
    rt_hw_uart_init("uart0", 0);
    rt_console_set_device("uart0");
    rt_hw_sd_init("sd0");
    rt_hw_rtc_init();
    rt_hw_spi_init();
    rt_hw_pin_init("gpio");
    rt_hw_i2c_bit_ops_bus_init("i2c0");
    at24cxx_init("at24c02", "i2c0");
    rt_hw_ads7843_init("ads7843", "spi20");
    w25qxx_init("sf0", "spi21");
    rt_hw_lcd_init("lcd0");
    
    finsh_system_init();
 //   tid = rt_thread_create("usb", usb_thread_entry, RT_NULL, 1024, 9, 20);
   // rt_thread_startup(tid);
    
    if((*(uint32_t*)0x60000) != 0xFFFFFFFF)
    {
        tid = rt_thread_create("init", (void*)(0x60000), RT_NULL, 1024, 8, 20);
        rt_thread_startup(tid);
    }
    else
    {
        printf("addr:0x%X has no application\r\n", 0x60000);
    }
    rt_hw_enet_phy_init();

    tid = rt_thread_self();
    rt_thread_delete(tid); 
}


void rt_application_init(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("init", init_thread, RT_NULL, 1024, 20, 20);
    rt_thread_startup(tid);
}




