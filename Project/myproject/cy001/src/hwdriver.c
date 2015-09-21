#include "hwdriver.h"

static void hwGprsPowerGpioInit(void);
static void hwGprsPowerEn(bool en);
static void hwGprsPowerKeyGpioInit(void);
static void hwGprsPowerKey(bool active);
static void hwCmosPwdnGpioInit(void);
static void hwCmosResetGpioInit(void);

void hwDriverInit(void)
{
    hwGprsPowerGpioInit();
    hwGprsPowerKeyGpioInit();
    hwCmosPwdnGpioInit();
    hwCmosResetGpioInit();
    DelayMs(100);
    hwGprsPowerOn();
    hsCmosPowerOn();
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   初始化GPRS模块电源控制脚，输出低关闭电源
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwGprsPowerGpioInit(void)
{
    GPIO_QuickInit(GPRSPOWER_GPIO_PORT, GPRSPOWER_GPIO_PIN, kGPIO_Mode_OPP);
    hwGprsPowerEn(GPRSPOWER_OFF);
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
static void hwGprsPowerEn(bool en)
{
    GPIO_WriteBit(GPRSPOWER_GPIO_PORT, GPRSPOWER_GPIO_PIN, en);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   初始化GPRS模块开关控制脚，输出高电平
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwGprsPowerKeyGpioInit(void)
{
    GPIO_QuickInit(GPRSPOWERKEY_GPIO_PORT, GPRSPOWERKEY_GPIO_PIN, kGPIO_Mode_OPP);//后续尝试使用kGPIO_Mode_OOD降低功耗
    hwGprsPowerKey(GPRSPOWERKEY_OFF);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   输出下降沿使GPRS模块开机或者关机，开机时至少持续输出低电平1s
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwGprsPowerKey(bool active)
{
    GPIO_WriteBit(GPRSPOWERKEY_GPIO_PORT, GPRSPOWERKEY_GPIO_PIN, active);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   GPRS开机时序
 *   GPRS供电100ms后，再将PowerKey输出低
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

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   初始化CMOS PWDN控制脚，输出高不工作
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwCmosPwdnGpioInit(void)
{
    GPIO_QuickInit(CMOSPWDN_GPIO_PORT, CMOSPWDN_GPIO_PIN, kGPIO_Mode_OPP);
    hwGprsPowerEn(CMOSPWDN_ACTIVE);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   输出低电平使CMOS工作
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwCmosPwdn(bool active)
{
    GPIO_WriteBit(CMOSPWDN_GPIO_PORT, CMOSPWDN_GPIO_PIN, active);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   初始化CMOS Reset控制脚，输出低不工作
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwCmosResetGpioInit(void)
{
    GPIO_QuickInit(CMOSRESET_GPIO_PORT, CMOSRESET_GPIO_PIN, kGPIO_Mode_OPP);
    hwGprsPowerEn(CMOSRESET_ACTIVE);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   输出低电平复位CMOS
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwCmosReset(bool active)
{
    GPIO_WriteBit(CMOSRESET_GPIO_PORT, CMOSRESET_GPIO_PIN, active);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   CMOS开机时序
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
void hsCmosPowerOn(void)
{
    hwCmosPwdn(CMOSPWDN_NEGATIVE);
    hwCmosReset(CMOSRESET_NEGATIVE);
}