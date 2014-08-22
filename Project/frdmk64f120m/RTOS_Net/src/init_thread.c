#include <rtthread.h>
#include <rthw.h>
#include "components.h"
#include <drivers/spi.h>

extern void gui_thread_entry(void* parameter);
extern void led_thread_entry(void* parameter);
extern void sd_thread_entry(void* parameter);

void init_thread_entry(void* parameter)
{
    struct rt_spi_device *spi_device;
    rt_thread_t thread;
    rt_err_t result;
    
#ifdef RT_USING_FINSH
	finsh_system_init(); /* init finsh */
#endif
    
	/* initialize lwip stack */
    eth_system_device_init();
	/* register ethernetif device */
    rt_hw_ksz8041_init();
	/* initialize lwip system */
	lwip_system_init();


//	dfs_init();
//	elm_init();

    /* led thread */
    thread = rt_thread_create("led", led_thread_entry, RT_NULL, 1024*1, 0x21, 20);                                                      
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    
    /* sd_thread */
//    thread = rt_thread_create("sd", sd_thread_entry, RT_NULL, 1024, 0x23, 20); 
//    if (thread != RT_NULL)
//    {
//        rt_thread_startup(thread);		
//    }
    
    /* supend me */
    thread = rt_thread_self();
    rt_thread_suspend(thread); 
}

