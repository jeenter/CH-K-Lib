#ifndef _MAIN_H_
#define _MAIN_H_

#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

/*-*-* 所有宏开关 *-*-*/

#define DEBUG_LOG//定义此宏开启串口打印，所有打印必须使用LOG()

#ifdef DEBUG_LOG
#define LOG(format,args...)  printf(format,##args)
#else //DEBUG_LOG
#define LOG(format,args...)
#endif //DEBUG_LOG

#define OUTPUT_HIGH     (true)
#define OUTPUT_LOW      (false)

//GPRS电源控制引脚
#define GPRSPOWER_GPIO_PORT               HW_GPIOB
#define GPRSPOWER_GPIO_PIN                (2)
#define GPRSPOWER_ON      (OUTPUT_HIGH)
#define GPRSPOWER_OFF     (OUTPUT_LOW)

//GPRS开机键控制引脚
#define GPRSPOWERKEY_GPIO_PORT        HW_GPIOB
#define GPRSPOWERKEY_GPIO_PIN         (18)
#define GPRSPOWERKEY_ON      (OUTPUT_LOW)
#define GPRSPOWERKEY_OFF     (OUTPUT_HIGH)

#endif //_MAIN_H_
