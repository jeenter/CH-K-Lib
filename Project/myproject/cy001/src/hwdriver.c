#include "hwdriver.h"

static void hwGprsPowerGpioInit(void);
static void hwGprsPowerKeyGpioInit(void);
static void hwGprsPowerEn(bool en);
static void hwGprsPowerKey(bool active);

void hwDriverInit(void)
{
    hwGprsPowerGpioInit();
    hwGprsPowerKeyGpioInit();
    DelayMs(100);
    hwGprsPowerOn();
}

void hwGprsPowerGpioInit(void)
{
    GPIO_QuickInit(GPRSPOWER_GPIO_PORT, GPRSPOWER_GPIO_PIN, kGPIO_Mode_OPP);
    hwGprsPowerEn(GPRSPOWER_OFF);
}

void hwGprsPowerKeyGpioInit(void)
{
    GPIO_QuickInit(GPRSPOWERKEY_GPIO_PORT, GPRSPOWERKEY_GPIO_PIN, kGPIO_Mode_OPP);//后续尝试使用kGPIO_Mode_OOD降低功耗
    hwGprsPowerKey(GPRSPOWERKEY_OFF);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   输出高电平开启GPRS模块电源
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
void hwGprsPowerEn(bool en)
{
    GPIO_WriteBit(GPRSPOWER_GPIO_PORT, GPRSPOWER_GPIO_PIN, en);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   输出低电平使GPRS模块开机
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
void hwGprsPowerKey(bool active)
{
    GPIO_WriteBit(GPRSPOWERKEY_GPIO_PORT, GPRSPOWERKEY_GPIO_PIN, active);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   GPRS开机时序
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
void hwGprsPowerOn(void)
{
//预留调试用
//    hwGprsPowerEn(GPRSPOWER_OFF);
//    hwGprsPowerKey(GPRSPOWERKEY_OFF);
//    DelayMs(100);
    hwGprsPowerEn(GPRSPOWER_ON);
    DelayMs(100);
    hwGprsPowerKey(GPRSPOWERKEY_ON);
}

