/**
  ******************************************************************************
  * @file    i2c.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "common.h"

typedef struct
{
    uint8_t instance;    //!< I2C pin select
    uint32_t baudrate;   //!< baudrate some common baudrate: 48000Hz 76000Hz 96000Hz 376000Hz
}I2C_InitTypeDef;


#define HW_I2C0         (0x00U)
#define HW_I2C1         (0x01U)
#define HW_I2C2         (0x02U)


//!< I2C QuickInit macro
#define I2C1_SCL_PE01_SDA_PE00  (0x81a1U)
#define I2C0_SCL_PE19_SDA_PE18  (0xa520U)
#define I2C0_SCL_PF22_SDA_PF23  (0xaca8U)
#define I2C0_SCL_PB00_SDA_PB01  (0x8088U)
#define I2C0_SCL_PB02_SDA_PB03  (0x8488U)
#define I2C1_SCL_PC10_SDA_PC11  (0x9491U)
#define I2C0_SCL_PD08_SDA_PD09  (0x9098U)


typedef enum
{
    kI2C_Read,                  //!< I2C Master Read Data
    kI2C_Write,                 //!< I2C Master Write Data
    kI2C_DirectionNameCount,
}I2C_Direction_Type; 

typedef enum
{
    kI2C_ITDMA_Disable,     //!< Disable Interrupt and DMA
    kI2C_IT_BTC,            //!< Byte Transfer Complete Interrupt
    kI2C_DMA_BTC,           //!< DMA Trigger On Byte Transfer Complete
}I2C_ITDMAConfig_Type;



//!< I2C CallBack Type
typedef void (*I2C_CallBackType)(void);

//!< API functions
void I2C_Init(I2C_InitTypeDef* I2C_InitStruct);
uint8_t I2C_QuickInit(uint32_t I2CxMAP, uint32_t baudrate);
void I2C_GenerateSTART(uint8_t instance);
void I2C_GenerateRESTART(uint8_t instance);
void I2C_GenerateSTOP(uint8_t instance);
void I2C_SendData(uint8_t instance, uint8_t data);
void I2C_Send7bitAddress(uint8_t instance, uint8_t address, I2C_Direction_Type direction);
uint8_t I2C_WaitAck(uint8_t instance);
uint8_t I2C_IsBusy(uint8_t instance);
void I2C_ITDMAConfig(uint8_t instance, I2C_ITDMAConfig_Type config, FunctionalState newState);
void I2C_CallbackInstall(uint8_t instance, I2C_CallBackType AppCBFun);
uint8_t I2C_ReadSingleRegister(uint8_t instance, uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t* pData);
uint8_t I2C_WriteSingleRegister(uint8_t instance, uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t Data);
int32_t I2C_ReadMutipleRegister(uint8_t instance, uint8_t deviceAddress, uint32_t subAddress, uint32_t subAddressLen, uint8_t * pData, uint32_t dataLen);



//!< param check
#define IS_I2C_ALL_INSTANCE(INSTANCE)  (INSTANCE < ARRAY_SIZE(I2C_InstanceTable))

#ifdef __cplusplus
}
#endif


#endif


