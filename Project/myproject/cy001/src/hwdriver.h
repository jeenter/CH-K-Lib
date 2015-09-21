#ifndef _HWDRIVER_H_
#define _HWDRIVER_H_

#include "main.h"

#define OUTPUT_HIGH     (true)
#define OUTPUT_LOW      (false)

//GPRS电源控制引脚
#define GPRSPOWER_GPIO_PORT             HW_GPIOB
#define GPRSPOWER_GPIO_PIN              (2)
#define GPRSPOWER_ON                    (OUTPUT_HIGH)
#define GPRSPOWER_OFF                   (OUTPUT_LOW)

//GPRS开机键控制引脚
#define GPRSPOWERKEY_GPIO_PORT          HW_GPIOB
#define GPRSPOWERKEY_GPIO_PIN           (18)
#define GPRSPOWERKEY_ON                 (OUTPUT_LOW)
#define GPRSPOWERKEY_OFF                (OUTPUT_HIGH)

//CMOS PWDN控制引脚
#define CMOSPWDN_GPIO_PORT              HW_GPIOC
#define CMOSPWDN_GPIO_PIN               (11)
#define CMOSPWDN_ACTIVE                 (OUTPUT_HIGH)
#define CMOSPWDN_NEGATIVE               (OUTPUT_LOW)

//CMOS RESET控制引脚
#define CMOSRESET_GPIO_PORT             HW_GPIOC
#define CMOSRESET_GPIO_PIN              (3)
#define CMOSRESET_ACTIVE                (OUTPUT_LOW)
#define CMOSRESET_NEGATIVE              (OUTPUT_HIGH)

extern void hwDriverInit(void);
extern void hwGprsPowerOn(void);
extern void hsCmosPowerOn(void);

#endif //_HWDRIVER_H_
