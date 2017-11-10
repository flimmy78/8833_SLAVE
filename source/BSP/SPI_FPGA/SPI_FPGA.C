#include "stm32f10x.h"
#include "SPI_FPGA.H"

#define SPI_CS_EN()         GPIO_ResetBits(GPIOD,GPIO_Pin_0)
#define SPI_CS_DIS()        GPIO_SetBits(GPIOD,GPIO_Pin_0)
#define SPI_MOSI_HIGH()     GPIO_SetBits(GPIOD,GPIO_Pin_4)
#define SPI_MOSI_LOW()      GPIO_ResetBits(GPIOD,GPIO_Pin_4)
#define SPI_CLK_HIGH()      GPIO_SetBits(GPIOD,GPIO_Pin_1)
#define SPI_CLK_LOW()       GPIO_ResetBits(GPIOD,GPIO_Pin_1)
#define SPI_MISO_DI()       GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_5)


void init_spi_fpga_gpio(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure; 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);  //����GPIODʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_4; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 			 //�������
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 				//��������	 
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
    
    SPI_CS_DIS();
}


static void fpga_delay_ms(u32 time)
{
//    u32 i = 0;
//    
//    while(time--)
//    {
//        for(i = 0; i < 8 * 1000; i++)
//        {
//        }
//    }
}

void fpga_write_byte(u8 SendData)
{
	u8  ShiftCounter;
	u8 	DataBuffer;
    
    for(ShiftCounter = 0; ShiftCounter < 8; ShiftCounter++)
    {
        DataBuffer = SendData;
        DataBuffer = DataBuffer & 0x00000001;//ȡ�������λ
        SendData = SendData >>1;//��������һλ
        SPI_CLK_HIGH();//ʱ���ø�
        fpga_delay_ms(1);
        
        if(DataBuffer == 0x00000000)
        {
            SPI_MOSI_LOW();
            fpga_delay_ms(1);
        }
        else
        {
            SPI_MOSI_HIGH();
            fpga_delay_ms(1);
        }
        
        SPI_CLK_LOW();//����д��CPLD
        fpga_delay_ms(1);
    }
}
//*************************************************************

/* ��������cpld_write
 * ����  ����CPLDд���������һ�ֽ�
 * ����  ������
 * ���  ������ֵ
 */
//**************************************************************
void fpga_write(u8 *buf,u8 bytes)
{
	u8  ShiftCounter;
	u8 	DataBuffer;
	int i;
	u8 SendData;
	
	SPI_CS_DIS();
	
	fpga_delay_ms(1);
	
	SPI_CS_EN();//CS=0;ƬѡCPLD
	fpga_delay_ms(1);
	
    for(i = bytes - 1; i >= 0; i--)
    {
        fpga_write_byte(buf[i]);
    }
    
    fpga_write_byte(0);
    fpga_write_byte(1);
    
    SPI_CS_DIS();//CS=1;ȡ��Ƭѡ
    fpga_delay_ms(1);
}








/******************* (C) COPYRIGHT 2017 ��ʢ���� *****END OF FILE****/
