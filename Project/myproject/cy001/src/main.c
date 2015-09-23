#include "main.h"
#include "hwdriver.h"

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
 
#define CAMERA_SCCB_ADDR    (0x21)//(0x42)
#define CAMERA_GC0308_ID    (0x9B)

// 改变图像大小
//0: 80x60
//1: 160x120
//2: 240x180
#define IMAGE_SIZE  3

#if (IMAGE_SIZE  ==  0)
#define OV7620_W    (80)
#define OV7620_H    (60)

#elif (IMAGE_SIZE == 1)
#define OV7620_W    (160)
#define OV7620_H    (120)

#elif (IMAGE_SIZE == 2)
#define OV7620_W    (240)
#define OV7620_H    (180)

#elif (IMAGE_SIZE == 3)
#define OV7620_W    (640)
#define OV7620_H    (100)

#else
#error "Image Size Not Support!"
#endif

#define CMOS_RET_OK     (0)
#define CMOS_RET_ERR    (-1)

#define CMOS_REG_ID     (0x00)

// 图像内存池
uint8_t gCCD_RAM[(OV7620_H)*((OV7620_W/8)+1)];   //使用内部RAM

/* 行指针 */
uint8_t * gpHREF[OV7620_H+1];

/* 引脚定义 PCLK VSYNC HREF 接到同一个PORT上 */
#define CMOS_PCLK_PORT      HW_GPIOC
#define CMOS_PCLK_PIN       (9)
#define CMOS_VSYNC_PORT     HW_GPIOC
#define CMOS_VSYNC_PIN      (10)
#define CMOS_HREF_PORT      HW_GPIOA
#define CMOS_HREF_PIN       (13)

#define CMOS_DATA_PORT      HW_GPIOD
#define CMOS_DATA0_PIN      (0)
#define CMOS_DATA1_PIN      (1)
#define CMOS_DATA2_PIN      (2)
#define CMOS_DATA3_PIN      (3)
#define CMOS_DATA4_PIN      (4)
#define CMOS_DATA5_PIN      (5)
#define CMOS_DATA6_PIN      (6)
#define CMOS_DATA7_PIN      (7)

/* 状态机定义 */
typedef enum
{
    TRANSFER_IN_PROCESS, //数据在处理
    NEXT_FRAME,          //下一帧数据
}OV7620_Status;

struct {
    uint32_t i2c_instance;
    uint8_t  addr;
    uint32_t h_size;
    uint32_t v_size;
}m_gc0308;
uint32_t h_counter;
static void UserApp(uint32_t vcount);


/* 接收完成一场后 用户处理函数 */
static void UserApp(uint32_t vcount)
{
    //GUI_printf(100,0, "frame:%d", vcount);
    //GUI_DispCCDImage(0, 15, OV7620_W, OV7620_H, gpHREF);
    //SerialDispCCDImage(OV7620_W, OV7620_H, CCDBuffer);
    LOG("frame:%d\r\n", vcount);
}

void GC0308_write_cmos_sensor(uint8_t addr, uint8_t para)
{
    if(SCCB_WriteSingleRegister(m_gc0308.i2c_instance, m_gc0308.addr, addr, para))
    {
        LOG("i2c_write addr=0x%02X reg=0x%02X para=0x%02X failed!!!\r\n", m_gc0308.addr, addr, para);
    }
}

/*************************************************************************
* FUNCTION
*GC0308_Sensor_Init
*
* DESCRIPTION
*This function apply all of the initial setting to sensor.
*
* PARAMETERS
*None
*
* RETURNS
*None
*
*************************************************************************/
void GC0308_Sensor_Init(void)
{
    GC0308_write_cmos_sensor(0xfe , 0x80);   

    GC0308_write_cmos_sensor(0xfe, 0x00);//GC0308_SET_PAGE0;       // set page0


    GC0308_write_cmos_sensor(0xd2 , 0x10);   // close AEC
    GC0308_write_cmos_sensor(0x22 , 0x55);   // close AWB

    GC0308_write_cmos_sensor(0x5a , 0x56); 
    GC0308_write_cmos_sensor(0x5b , 0x40);
    GC0308_write_cmos_sensor(0x5c , 0x4a);

    GC0308_write_cmos_sensor(0x22 , 0x57);  // Open AWB

    GC0308_write_cmos_sensor(0x01 , 0xfa); 
    GC0308_write_cmos_sensor(0x02 , 0x70); 
    GC0308_write_cmos_sensor(0x0f , 0x01); 

    GC0308_write_cmos_sensor(0x03 , 0x01); 
    GC0308_write_cmos_sensor(0x04 , 0x2c); 

    GC0308_write_cmos_sensor(0xe2 , 0x00); //anti-flicker step [11:8]
    GC0308_write_cmos_sensor(0xe3 , 0x64);   //anti-flicker step [7:0]

    GC0308_write_cmos_sensor(0xe4 , 0x02);   //exp level 0  16.67fps
    GC0308_write_cmos_sensor(0xe5 , 0x58); 
    GC0308_write_cmos_sensor(0xe6 , 0x03);   //exp level 1  12.5fps
    GC0308_write_cmos_sensor(0xe7 , 0x20); 
    GC0308_write_cmos_sensor(0xe8 , 0x04);   //exp level 2  8.33fps
    GC0308_write_cmos_sensor(0xe9 , 0xb0); 
    GC0308_write_cmos_sensor(0xea , 0x09);   //exp level 3  4.00fps
    GC0308_write_cmos_sensor(0xeb , 0xc4); 

    GC0308_write_cmos_sensor(0x05 , 0x00);
    GC0308_write_cmos_sensor(0x06 , 0x00);
    GC0308_write_cmos_sensor(0x07 , 0x00);
    GC0308_write_cmos_sensor(0x08 , 0x00);
    GC0308_write_cmos_sensor(0x09 , 0x01);
    GC0308_write_cmos_sensor(0x0a , 0xe8);
    GC0308_write_cmos_sensor(0x0b , 0x02);
    GC0308_write_cmos_sensor(0x0c , 0x88);
    GC0308_write_cmos_sensor(0x0d , 0x02);
    GC0308_write_cmos_sensor(0x0e , 0x02);
    GC0308_write_cmos_sensor(0x10 , 0x22);
    GC0308_write_cmos_sensor(0x11 , 0xfd);
    GC0308_write_cmos_sensor(0x12 , 0x2a);
    GC0308_write_cmos_sensor(0x13 , 0x00);
    GC0308_write_cmos_sensor(0x14 , 0x10);
    GC0308_write_cmos_sensor(0x15 , 0x0a);
    GC0308_write_cmos_sensor(0x16 , 0x05);
    GC0308_write_cmos_sensor(0x17 , 0x01);
    GC0308_write_cmos_sensor(0x18 , 0x44);
    GC0308_write_cmos_sensor(0x19 , 0x44);
    GC0308_write_cmos_sensor(0x1a , 0x1e);
    GC0308_write_cmos_sensor(0x1b , 0x00);
    GC0308_write_cmos_sensor(0x1c , 0xc1);
    GC0308_write_cmos_sensor(0x1d , 0x08);
    GC0308_write_cmos_sensor(0x1e , 0x60);
    GC0308_write_cmos_sensor(0x1f , 0x16);


    GC0308_write_cmos_sensor(0x20 , 0xff);
    GC0308_write_cmos_sensor(0x21 , 0xf8);
    GC0308_write_cmos_sensor(0x22 , 0x57);
    GC0308_write_cmos_sensor(0x24 , 0xa0);
    GC0308_write_cmos_sensor(0x25 , 0x0f);
                             
    //output sync_mode       
    GC0308_write_cmos_sensor(0x26 , 0x03);
    GC0308_write_cmos_sensor(0x2f , 0x01);
    GC0308_write_cmos_sensor(0x30 , 0xf7);
    GC0308_write_cmos_sensor(0x31 , 0x50);
    GC0308_write_cmos_sensor(0x32 , 0x00);
    GC0308_write_cmos_sensor(0x39 , 0x04);
    GC0308_write_cmos_sensor(0x3a , 0x18);
    GC0308_write_cmos_sensor(0x3b , 0x20);
    GC0308_write_cmos_sensor(0x3c , 0x00);
    GC0308_write_cmos_sensor(0x3d , 0x00);
    GC0308_write_cmos_sensor(0x3e , 0x00);
    GC0308_write_cmos_sensor(0x3f , 0x00);
    GC0308_write_cmos_sensor(0x50 , 0x10);
    GC0308_write_cmos_sensor(0x53 , 0x82);
    GC0308_write_cmos_sensor(0x54 , 0x80);
    GC0308_write_cmos_sensor(0x55 , 0x80);
    GC0308_write_cmos_sensor(0x56 , 0x82);
    GC0308_write_cmos_sensor(0x8b , 0x40);
    GC0308_write_cmos_sensor(0x8c , 0x40);
    GC0308_write_cmos_sensor(0x8d , 0x40);
    GC0308_write_cmos_sensor(0x8e , 0x2e);
    GC0308_write_cmos_sensor(0x8f , 0x2e);
    GC0308_write_cmos_sensor(0x90 , 0x2e);
    GC0308_write_cmos_sensor(0x91 , 0x3c);
    GC0308_write_cmos_sensor(0x92 , 0x50);
    GC0308_write_cmos_sensor(0x5d , 0x12);
    GC0308_write_cmos_sensor(0x5e , 0x1a);
    GC0308_write_cmos_sensor(0x5f , 0x24);
    GC0308_write_cmos_sensor(0x60 , 0x07);
    GC0308_write_cmos_sensor(0x61 , 0x15);
    GC0308_write_cmos_sensor(0x62 , 0x08);
    GC0308_write_cmos_sensor(0x64 , 0x03);
    GC0308_write_cmos_sensor(0x66 , 0xe8);
    GC0308_write_cmos_sensor(0x67 , 0x86);
    GC0308_write_cmos_sensor(0x68 , 0xa2);
    GC0308_write_cmos_sensor(0x69 , 0x18);
    GC0308_write_cmos_sensor(0x6a , 0x0f);
    GC0308_write_cmos_sensor(0x6b , 0x00);
    GC0308_write_cmos_sensor(0x6c , 0x5f);
    GC0308_write_cmos_sensor(0x6d , 0x8f);
    GC0308_write_cmos_sensor(0x6e , 0x55);
    GC0308_write_cmos_sensor(0x6f , 0x38);
    GC0308_write_cmos_sensor(0x70 , 0x15);
    GC0308_write_cmos_sensor(0x71 , 0x33);
    GC0308_write_cmos_sensor(0x72 , 0xdc);
    GC0308_write_cmos_sensor(0x73 , 0x80);
    GC0308_write_cmos_sensor(0x74 , 0x02);
    GC0308_write_cmos_sensor(0x75 , 0x3f);
    GC0308_write_cmos_sensor(0x76 , 0x02);
    GC0308_write_cmos_sensor(0x77 , 0x36);
    GC0308_write_cmos_sensor(0x78 , 0x88);
    GC0308_write_cmos_sensor(0x79 , 0x81);
    GC0308_write_cmos_sensor(0x7a , 0x81);
    GC0308_write_cmos_sensor(0x7b , 0x22);
    GC0308_write_cmos_sensor(0x7c , 0xff);
    GC0308_write_cmos_sensor(0x93 , 0x48);
    GC0308_write_cmos_sensor(0x94 , 0x00);
    GC0308_write_cmos_sensor(0x95 , 0x05);
    GC0308_write_cmos_sensor(0x96 , 0xe8);
    GC0308_write_cmos_sensor(0x97 , 0x40);
    GC0308_write_cmos_sensor(0x98 , 0xf0);
    GC0308_write_cmos_sensor(0xb1 , 0x38);
    GC0308_write_cmos_sensor(0xb2 , 0x38);
    GC0308_write_cmos_sensor(0xbd , 0x38);
    GC0308_write_cmos_sensor(0xbe , 0x36);
    GC0308_write_cmos_sensor(0xd0 , 0xc9);
    GC0308_write_cmos_sensor(0xd1 , 0x10);
    //GC0308_write_cmos_sensor(0xd2 , 0x90);
    GC0308_write_cmos_sensor(0xd3 , 0x80);
    GC0308_write_cmos_sensor(0xd5 , 0xf2);
    GC0308_write_cmos_sensor(0xd6 , 0x16);
    GC0308_write_cmos_sensor(0xdb , 0x92);
    GC0308_write_cmos_sensor(0xdc , 0xa5);
    GC0308_write_cmos_sensor(0xdf , 0x23);
    GC0308_write_cmos_sensor(0xd9 , 0x00);
    GC0308_write_cmos_sensor(0xda , 0x00);
    GC0308_write_cmos_sensor(0xe0 , 0x09);
    GC0308_write_cmos_sensor(0xec , 0x20);
    GC0308_write_cmos_sensor(0xed , 0x04);
    GC0308_write_cmos_sensor(0xee , 0xa0);
    GC0308_write_cmos_sensor(0xef , 0x40);
    GC0308_write_cmos_sensor(0x80 , 0x03);
    GC0308_write_cmos_sensor(0x80 , 0x03);
    GC0308_write_cmos_sensor(0x9F , 0x10);
    GC0308_write_cmos_sensor(0xA0 , 0x20);
    GC0308_write_cmos_sensor(0xA1 , 0x38);
    GC0308_write_cmos_sensor(0xA2 , 0x4E);
    GC0308_write_cmos_sensor(0xA3 , 0x63);
    GC0308_write_cmos_sensor(0xA4 , 0x76);
    GC0308_write_cmos_sensor(0xA5 , 0x87);
    GC0308_write_cmos_sensor(0xA6 , 0xA2);
    GC0308_write_cmos_sensor(0xA7 , 0xB8);
    GC0308_write_cmos_sensor(0xA8 , 0xCA);
    GC0308_write_cmos_sensor(0xA9 , 0xD8);
    GC0308_write_cmos_sensor(0xAA , 0xE3);
    GC0308_write_cmos_sensor(0xAB , 0xEB);
    GC0308_write_cmos_sensor(0xAC , 0xF0);
    GC0308_write_cmos_sensor(0xAD , 0xF8);
    GC0308_write_cmos_sensor(0xAE , 0xFD);
    GC0308_write_cmos_sensor(0xAF , 0xFF);
    GC0308_write_cmos_sensor(0xc0 , 0x00);
    GC0308_write_cmos_sensor(0xc1 , 0x10);
    GC0308_write_cmos_sensor(0xc2 , 0x1C);
    GC0308_write_cmos_sensor(0xc3 , 0x30);
    GC0308_write_cmos_sensor(0xc4 , 0x43);
    GC0308_write_cmos_sensor(0xc5 , 0x54);
    GC0308_write_cmos_sensor(0xc6 , 0x65);
    GC0308_write_cmos_sensor(0xc7 , 0x75);
    GC0308_write_cmos_sensor(0xc8 , 0x93);
    GC0308_write_cmos_sensor(0xc9 , 0xB0);
    GC0308_write_cmos_sensor(0xca , 0xCB);
    GC0308_write_cmos_sensor(0xcb , 0xE6);
    GC0308_write_cmos_sensor(0xcc , 0xFF);
    GC0308_write_cmos_sensor(0xf0 , 0x02);
    GC0308_write_cmos_sensor(0xf1 , 0x01);
    GC0308_write_cmos_sensor(0xf2 , 0x01);
    GC0308_write_cmos_sensor(0xf3 , 0x30);
    GC0308_write_cmos_sensor(0xf9 , 0x9f);
    GC0308_write_cmos_sensor(0xfa , 0x78);

    //---------------------------------------------------------------
    GC0308_write_cmos_sensor(0xfe, 0x01);//GC0308_SET_PAGE1;

    GC0308_write_cmos_sensor(0x00 , 0xf5);
    GC0308_write_cmos_sensor(0x02 , 0x1a);
    GC0308_write_cmos_sensor(0x0a , 0xa0);
    GC0308_write_cmos_sensor(0x0b , 0x60);
    GC0308_write_cmos_sensor(0x0c , 0x08);
    GC0308_write_cmos_sensor(0x0e , 0x4c);
    GC0308_write_cmos_sensor(0x0f , 0x39);
    GC0308_write_cmos_sensor(0x11 , 0x3f);
    GC0308_write_cmos_sensor(0x12 , 0x72);
    GC0308_write_cmos_sensor(0x13 , 0x13);
    GC0308_write_cmos_sensor(0x14 , 0x42);
    GC0308_write_cmos_sensor(0x15 , 0x43);
    GC0308_write_cmos_sensor(0x16 , 0xc2);
    GC0308_write_cmos_sensor(0x17 , 0xa8);
    GC0308_write_cmos_sensor(0x18 , 0x18);
    GC0308_write_cmos_sensor(0x19 , 0x40);
    GC0308_write_cmos_sensor(0x1a , 0xd0);
    GC0308_write_cmos_sensor(0x1b , 0xf5);
    GC0308_write_cmos_sensor(0x70 , 0x40);
    GC0308_write_cmos_sensor(0x71 , 0x58);
    GC0308_write_cmos_sensor(0x72 , 0x30);
    GC0308_write_cmos_sensor(0x73 , 0x48);
    GC0308_write_cmos_sensor(0x74 , 0x20);
    GC0308_write_cmos_sensor(0x75 , 0x60);
    GC0308_write_cmos_sensor(0x77 , 0x20);
    GC0308_write_cmos_sensor(0x78 , 0x32);
    GC0308_write_cmos_sensor(0x30 , 0x03);
    GC0308_write_cmos_sensor(0x31 , 0x40);
    GC0308_write_cmos_sensor(0x32 , 0xe0);
    GC0308_write_cmos_sensor(0x33 , 0xe0);
    GC0308_write_cmos_sensor(0x34 , 0xe0);
    GC0308_write_cmos_sensor(0x35 , 0xb0);
    GC0308_write_cmos_sensor(0x36 , 0xc0);
    GC0308_write_cmos_sensor(0x37 , 0xc0);
    GC0308_write_cmos_sensor(0x38 , 0x04);
    GC0308_write_cmos_sensor(0x39 , 0x09);
    GC0308_write_cmos_sensor(0x3a , 0x12);
    GC0308_write_cmos_sensor(0x3b , 0x1C);
    GC0308_write_cmos_sensor(0x3c , 0x28);
    GC0308_write_cmos_sensor(0x3d , 0x31);
    GC0308_write_cmos_sensor(0x3e , 0x44);
    GC0308_write_cmos_sensor(0x3f , 0x57);
    GC0308_write_cmos_sensor(0x40 , 0x6C);
    GC0308_write_cmos_sensor(0x41 , 0x81);
    GC0308_write_cmos_sensor(0x42 , 0x94);
    GC0308_write_cmos_sensor(0x43 , 0xA7);
    GC0308_write_cmos_sensor(0x44 , 0xB8);
    GC0308_write_cmos_sensor(0x45 , 0xD6);
    GC0308_write_cmos_sensor(0x46 , 0xEE);
    GC0308_write_cmos_sensor(0x47 , 0x0d); 

    GC0308_write_cmos_sensor(0xfe, 0x00);//GC0308_SET_PAGE0;

    GC0308_write_cmos_sensor(0xd2 , 0x90);  // Open AEC at last.  

    //GC0308_sensor.preview_pclk = 240;
}

int8_t Cmos_Probe(uint8_t i2c_instance)
{
    int8_t r;
    uint8_t dummy;
    
    if(!SCCB_ReadSingleRegister(i2c_instance, CAMERA_SCCB_ADDR, CMOS_REG_ID, &dummy))
    {
        LOG("Cmos 7bits-addr=0x%02X found.", CAMERA_SCCB_ADDR);
        if(dummy == CAMERA_GC0308_ID)
        {
            LOG("id=0x%02X.\r\n", dummy);
            m_gc0308.addr = CAMERA_SCCB_ADDR;
            m_gc0308.i2c_instance = i2c_instance;
            //success
            GC0308_Sensor_Init();
            return CMOS_RET_OK;
        }
        else
        {
            LOG("id=0x%02X.Not support!!!\r\n", dummy);
        }
    }
    else
    {
        LOG("Device 7bits-addr=0x%02X not found!!!\r\n", CAMERA_SCCB_ADDR);
    }
    
    return CMOS_RET_ERR;
}


int8_t Cmos_SCCB_Init(uint32_t I2C_MAP)
{
    int8_t r;
    uint32_t instance;
    instance = I2C_QuickInit(I2C_MAP, 100*1000);
    r = Cmos_Probe(instance);
    
    return 0;
    if(r)
    {
        return 1;
    }
    r = ov7725_set_image_size(IMAGE_SIZE);
    if(r)
    {
        printf("OV7725 set image error\r\n");
        return 1;
    }
    return 0;
}
//行中断和场中断都使用PTC中断
void CMOS_ISR1(uint32_t index)
{
    static uint8_t status = TRANSFER_IN_PROCESS;
   // uint32_t i;
    
    /* 行中断 */
    if(index & (1 << CMOS_HREF_PIN))
    {
        DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)gpHREF[h_counter++]);
        //i = DMA_GetMajorLoopCount(HW_DMA_CH2);
        DMA_SetMajorLoopCounter(HW_DMA_CH2, (OV7620_W/8)+1);
        DMA_EnableRequest(HW_DMA_CH2);
        
        return;
    }
}
//行中断和场中断都使用PTC中断
void CMOS_ISR(uint32_t index)
{
    static uint8_t status = TRANSFER_IN_PROCESS;
    static uint32_t v_counter;
   // uint32_t i;
    
#if 0//行中断
    /* 行中断 */
    if(index & (1 << CMOS_HREF_PIN))
    {
        DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)gpHREF[h_counter++]);
        //i = DMA_GetMajorLoopCount(HW_DMA_CH2);
        DMA_SetMajorLoopCounter(HW_DMA_CH2, (OV7620_W/8)+1);
        DMA_EnableRequest(HW_DMA_CH2);
        
        return;
    }
#endif//行中断
    /* 场中断 */
    if(index & (1 << CMOS_VSYNC_PIN))
    {
        GPIO_ITDMAConfig(CMOS_VSYNC_PORT, CMOS_VSYNC_PIN, kGPIO_IT_RisingEdge, false);
        GPIO_ITDMAConfig(CMOS_HREF_PORT, CMOS_HREF_PIN, kGPIO_IT_RisingEdge, false);
        switch(status)
        {
            case TRANSFER_IN_PROCESS: //接受到一帧数据调用用户处理
                    UserApp(v_counter++);
                    //printf("i:%d %d\r\n", h_counter, i);
                    status = NEXT_FRAME;
                    h_counter = 0;

                break;
            case NEXT_FRAME: //等待下次传输
                status =  TRANSFER_IN_PROCESS;
                break;
            default:
                break;
        }
        GPIO_ITDMAConfig(CMOS_VSYNC_PORT, CMOS_VSYNC_PIN, kGPIO_IT_RisingEdge, true);
        GPIO_ITDMAConfig(CMOS_HREF_PORT, CMOS_HREF_PIN, kGPIO_IT_RisingEdge, true);
        PORTC->ISFR = 0xFFFFFFFF;
        h_counter = 0;
        return;
    }
}

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

    hwDriverInit();

    /* 使用快速初始化 */
    FTM_PWM_QuickInit(FTM3_CH4_PC08, kPWM_EdgeAligned, 12000000);

    /* 设置FTM0模块3通道的占空比 */
    //FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH4, 5000); /* 0-10000 对应 0-100% */

    DMA_InitTypeDef DMA_InitStruct1 = {0};

    Cmos_SCCB_Init(I2C0_SCL_PB00_SDA_PB01);

    /* 场中断  行中断 像素中断 */
    GPIO_QuickInit(CMOS_PCLK_PORT, CMOS_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(CMOS_VSYNC_PORT, CMOS_VSYNC_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(CMOS_HREF_PORT, CMOS_HREF_PIN, kGPIO_Mode_IPD);
    
    /* install callback */
    GPIO_CallbackInstall(CMOS_VSYNC_PORT, CMOS_ISR);
    GPIO_CallbackInstall(CMOS_HREF_PORT, CMOS_ISR1);
    GPIO_ITDMAConfig(CMOS_HREF_PORT, CMOS_HREF_PIN, kGPIO_IT_RisingEdge, true);
    GPIO_ITDMAConfig(CMOS_VSYNC_PORT, CMOS_VSYNC_PIN, kGPIO_IT_RisingEdge, true);
    GPIO_ITDMAConfig(CMOS_PCLK_PORT, CMOS_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
    /* 初始化数据端口 */
    for(i=0;i<8;i++)
    {
        GPIO_QuickInit(HW_GPIOD, CMOS_DATA0_PIN+i, kGPIO_Mode_IFT);
    }

    //每行数据指针
    for(i=0; i<OV7620_H+1; i++)
    {
        gpHREF[i] = (uint8_t*)&gCCD_RAM[i*OV7620_W/8];
    }

    //DMA配置
    DMA_InitStruct1.chl = HW_DMA_CH2;
    DMA_InitStruct1.chlTriggerSource = PORTC_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 1;
    DMA_InitStruct1.majorLoopCnt = ((OV7620_W/8) +1);
    
    DMA_InitStruct1.sAddr = (uint32_t)&PTD->PDIR;
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)gpHREF[0];
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;

    /* initialize DMA moudle */
    DMA_Init(&DMA_InitStruct1);
    while(1)
    {
        /* 闪烁小灯 */
        GPIO_ToggleBit(HW_GPIOC, 1);
        DelayMs(500);
    }
}


