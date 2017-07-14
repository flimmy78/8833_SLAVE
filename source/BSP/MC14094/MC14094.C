/*
 * Copyright(c) 2013,
 * All rights reserved
 * �ļ����ƣ�MC14094.c
 * ժ  Ҫ  ��������
 * ��ǰ�汾��V1.0�������ɱ�д
 * �޸ļ�¼��2014.8.12 ����������Һ���ԱȶȵĿ���
 *                     ���������˶�·�˿ڵĿ���
 *
 */

#include "MC14094.h"
#include "string.h"

/* ���Կ��� */
#define MC14094_SETCLK()				(GPIO_SetBits(GPIOB,GPIO_Pin_9))
#define MC14094_CLRCLK()				(GPIO_ResetBits(GPIOB,GPIO_Pin_9))
#define MC14094_SETDATA()				(GPIO_SetBits(GPIOB,GPIO_Pin_6))
#define MC14094_CLRDATA()				(GPIO_ResetBits(GPIOB,GPIO_Pin_6))
#define MC14094_SETST()					(GPIO_SetBits(GPIOB,GPIO_Pin_7))
#define MC14094_CLRST()					(GPIO_ResetBits(GPIOB,GPIO_Pin_7))

/* Һ���Աȶȵ��� ��RA��β */
#define MC14094_SETCLK_RA()				//(GPIO_SetBits(GPIOG,GPIO_Pin_9))
#define MC14094_CLRCLK_RA()				//(GPIO_ResetBits(GPIOG,GPIO_Pin_9))
#define MC14094_SETDATA_RA()			//(GPIO_SetBits(GPIOB,GPIO_Pin_11))
#define MC14094_CLRDATA_RA()			//(GPIO_ResetBits(GPIOB,GPIO_Pin_11))
#define MC14094_SETST_RA()				//(GPIO_SetBits(GPIOB,GPIO_Pin_10))
#define MC14094_CLRST_RA()				//(GPIO_ResetBits(GPIOB,GPIO_Pin_10))

/* ���Զ˿ڿ��� */
#define MC14094_SETCLK_PORT()			(GPIO_SetBits(GPIOE,GPIO_Pin_6))
#define MC14094_CLRCLK_PORT()			(GPIO_ResetBits(GPIOE,GPIO_Pin_6))
#define MC14094_SETDATA_PORT()			(GPIO_SetBits(GPIOE,GPIO_Pin_1))
#define MC14094_CLRDATA_PORT()			(GPIO_ResetBits(GPIOE,GPIO_Pin_1))
#define MC14094_SETST_PORT()			(GPIO_SetBits(GPIOE,GPIO_Pin_0))
#define MC14094_CLRST_PORT()			(GPIO_ResetBits(GPIOE,GPIO_Pin_0))

#define MC14094_Dly					 360 // (720 * 1)//(720) // 

uint8_t 	MC14094_BUFFER[MC14094_NUM];
uint8_t		MC14094_BUFFER_RA[MC14094_NUM_RA];/* Һ���Աȶ� */
uint8_t		MC14094_BUFFER_PORT[MC14094_NUM_PORT];/* ���Զ˿� */

/*
 * ��������MC14094_GPIO_Config
 * ����  ����������
 * ����  ����
 * ���  ����
 */
static void MC14094_GPIO_Config(void)
{
	/* �����ṹ����� */
	GPIO_InitTypeDef GPIO_InitStructure;
    
	/* ����ӦIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOG, ENABLE);
	/* ����IO */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����ٶ�
    
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ���˿�B
	
	MC14094_CLRDATA_PORT();
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;/* ����ʱ�� */
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*
 * ��������MC14094_Delay
 * ����  ��������ʱ
 * ����  ����
 * ���  ����
 */
static void MC14094_Delay(u32 t)
{
	while(t--);
}

/*
 * ��������MC14094_WriteByte
 * ����  ����4094дһ���ֽ�����
 * ����  ��d Ҫд������
 * ���  ����
 * ����  : ��
 */
static void MC14094_WriteByte(u8 d)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		MC14094_CLRCLK();
		if(d & 0x80)
		{
			MC14094_SETDATA();
		}
		else
		{
			MC14094_CLRDATA();
		}
		MC14094_Delay(MC14094_Dly);
		MC14094_SETCLK();
		MC14094_Delay(MC14094_Dly);
		d=d<<1;
	}
}

/*
 * ��������MC14094_WriteByte
 * ����  ����4094дn���ֽ�����
 * ����  ��p ָ�����ݻ���
 *         n Ҫд�����ݵĸ���
 * ���  ����
 * ����  : ��
 */
static void MC14094_Write(u8 *p, u8 n)
{
	p += (n - 1);
	MC14094_CLRST();
    
	for(; n > 0; n--)
	{
		MC14094_WriteByte(*p--);
	}
    
	MC14094_SETST();
    MC14094_Delay(MC14094_Dly);
    MC14094_Delay(MC14094_Dly);
    MC14094_Delay(MC14094_Dly);
	MC14094_CLRST();
}

/*
 * ��������MC14094_WriteByte
 * ����  ������4094�����ָ��λ״̬
 * ����  ��ID ����4094��ID 
 *         bit ָ����λ
 *         status Ҫ���õ�״̬
 * ���  ����
 * ����  : ��
 */
void MC14094_CMD(u8 ID,u8 bits,u8 status)
{
	status != 0? (MC14094_BUFFER[ID] |= bits) : (MC14094_BUFFER[ID] &= ~bits);
}

/*
 * ��������MC14094_CMD_BYTE
 * ����  ������4094�����ָ��chip��״̬
 * ����  ��ID ����4094��ID 
 *         status Ҫ���õ�״̬
 * ���  ����
 * ����  : ��
 */
void MC14094_CMD_BYTE(u8 ID, u8 status)
{
	MC14094_BUFFER[ID] = status;
}

/*
 * ��������MC14094_Updata
 * ����  ����4094�����е�����д�뵽Ӳ����
 * ����  ����
 * ���  ����
 * ����  : ��
 */
void MC14094_Updata(void)
{
	MC14094_Write(MC14094_BUFFER,MC14094_NUM);
}

/*
 * ��������MC14094_Init
 * ����  ��MC14094��ʼ��
 * ����  ����
 * ���  ����
 */
void MC14094_Init(void)
{
	u8 i;
	MC14094_GPIO_Config();
	for(i=0;i<MC14094_NUM;i++)MC14094_BUFFER[i]=0;
	MC14094_Write(MC14094_BUFFER,MC14094_NUM);
    
    reset_posrts_mc14094_ctrl();
}

/*********************************************************
*				Һ�����Աȶ�����
*********************************************************/

/*
 * ��������MC14094_WriteByte_RA
 * ����  ����4094дһ���ֽ�����
 * ����  ��d Ҫд������
 * ���  ����
 * ����  : ��
 */
static void MC14094_WriteByte_RA(u8 d)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		MC14094_CLRCLK_RA();
		if(d & 0x80)
		{
			MC14094_SETDATA_RA();
		}
		else
		{
			MC14094_CLRDATA_RA();
		}
		MC14094_Delay(MC14094_Dly);
		MC14094_SETCLK_RA();
		MC14094_Delay(MC14094_Dly);
		d=d<<1;
	}
}

/*
 * ��������MC14094_Write_RA
 * ����  ����4094дn���ֽ�����
 * ����  ��p ָ�����ݻ���
 *         n Ҫд�����ݵĸ���
 * ���  ����
 * ����  : ��
 */
static void MC14094_Write_RA(u8 *p, u8 n)
{
	p += (n-1);
    
	MC14094_CLRST_RA();
	for(;n>0;n--)
	{
		MC14094_WriteByte_RA(*p--);
	}
    
	MC14094_SETST_RA();
    MC14094_Delay(MC14094_Dly);
	MC14094_CLRST_RA();
}
/*
 * ��������MC14094_CMD_RA
 * ����  ������4094�����ָ��chip��״̬
 * ����  ��status Ҫ���õ�״̬
 * ���  ����
 * ����  : ��
 */
void MC14094_CMD_RA(u8 status)
{
	MC14094_BUFFER_RA[0] = status;
}
/*
 * ��������MC14094_Updata_RA
 * ����  ����4094�����е�����д�뵽Ӳ����
 * ����  ����
 * ���  ����
 * ����  : ��
 */
void MC14094_Updata_RA(void)
{
	MC14094_Write_RA(MC14094_BUFFER_RA,MC14094_NUM_RA);
}

/*********************************************************
*				�˿����ü̵�������
*********************************************************/

/*
 * ��������MC14094_WriteByte_Port
 * ����  ����4094дһ���ֽ�����
 * ����  ��d Ҫд������
 * ���  ����
 * ����  : ��
 */
static void MC14094_WriteByte_Port(u8 d)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		MC14094_CLRCLK_PORT();
		if(d & 0x80)
		{
			MC14094_SETDATA_PORT();
		}
		else
		{
			MC14094_CLRDATA_PORT();
		}
		MC14094_Delay(MC14094_Dly);
		MC14094_SETCLK_PORT();
		MC14094_Delay(MC14094_Dly);
		d=d<<1;
	}
}

/*
 * ��������MC14094_Write_Port
 * ����  ����4094дn���ֽ�����
 * ����  ��p ָ�����ݻ���
 *         n Ҫд�����ݵĸ���
 * ���  ����
 * ����  : ��
 */
static void MC14094_Write_Port(u8 *p, u8 n)
{
	p += (n-1);
	MC14094_CLRST_PORT();
	for(;n>0;n--)
	{
		MC14094_WriteByte_Port(*p--);
	}
	MC14094_SETST_PORT();
}

/*
 * ��������MC14094_CMD_Port
 * ����  ������4094�����ָ��λ״̬
 * ����  ��ID ����4094��ID 
 *         bit ָ����λ
 *         status Ҫ���õ�״̬
 * ���  ����
 * ����  : ��
 */
void MC14094_CMD_Port(u8 ID, u8 bits, u8 status)
{
	status != 0? (MC14094_BUFFER_PORT[ID] |= 1<<bits) : (MC14094_BUFFER_PORT[ID] &= ~(1<<bits));
}

/*
 * ��������reset_posrts_mc14094_ctrl
 * ����  �����4094���������� �� ���µ�Ӳ��
 * ����  ����
 * ���  ����
 * ����  : ��
 */
void reset_posrts_mc14094_ctrl(void)
{
    memset(MC14094_BUFFER_PORT, 0, sizeof MC14094_BUFFER_PORT);
    MC14094_Updata_Port();
}
/*
 * ��������MC14094_Updata_Port
 * ����  �����¶˿ڼĴ���״̬
 * ����  ����
 * ���  ����
 * ����	 ����
 */
void MC14094_Updata_Port(void)
{
	MC14094_Write_Port(MC14094_BUFFER_PORT, MC14094_NUM_PORT);
}


