/*************************************************************************
	> File Name: bsp_ov2640.c
	> Author: lvsenlv - translated
	> Mail: lvsen46000@163.com
	> Created Time: 2017��04��05�� ������ 19ʱ07��06��
 ************************************************************************/
 
#include "bsp_Systick.h"
#include "bsp_ov2640.h"

uint16_t *g_dma_target_ptr = NULL;
const uint16_t g_img_width = 480;
const uint16_t g_img_height = 360;
const uint32_t g_img_size = (g_img_height*g_img_width*2);

//dma could only transfer 0xFFFF bytes data
//DMA_BufferSize is based on uint32_t
const uint16_t g_dma_buf_size = (g_img_width*2/sizeof(uint32_t));                             

const unsigned char OV2640_UXGA[][2] =
{
  0xff, 0x00,
  0x2c, 0xff,
  0x2e, 0xdf,
  0xff, 0x01,
  0x3c, 0x32,
  0x11, 0x00,
  0x09, 0x02,
  0x04, 0xA0, //0x20|0x80,	//ˮƽ��ת
  0x13, 0xe5,
  0x14, 0x48,
  0x2c, 0x0c,
  0x33, 0x78,
  0x3a, 0x33,
  0x3b, 0xfB,
  0x3e, 0x00,
  0x43, 0x11,
  0x16, 0x10,
  0x4a, 0x81,
  0x21, 0x99,
  0x24, 0x40,
  0x25, 0x38,
  0x26, 0x82,
  0x5c, 0x00,
  0x63, 0x00,
  0x46, 0x3f,
  0x0c, 0x3c,
  0x61, 0x70,
  0x62, 0x80,
  0x7c, 0x05,
  0x20, 0x80,
  0x28, 0x30,
  0x6c, 0x00,
  0x6d, 0x80,
  0x6e, 0x00,
  0x70, 0x02,
  0x71, 0x94,
  0x73, 0xc1,
  0x3d, 0x34,
  0x5a, 0x57,
  0x12, 0x00,
  0x11, 0x00,
  0x17, 0x11,
  0x18, 0x75,
  0x19, 0x01,
  0x1a, 0x97,
  0x32, 0x36,
  0x03, 0x0f,
  0x37, 0x40,
  0x4f, 0xbb,
  0x50, 0x9c,
  0x5a, 0x57,
  0x6d, 0x80,
  0x6d, 0x38,
  0x39, 0x02,
  0x35, 0x88,
  0x22, 0x0a,
  0x37, 0x40,
  0x23, 0x00,
  0x34, 0xa0,
  0x36, 0x1a,
  0x06, 0x02,
  0x07, 0xc0,
  0x0d, 0xb7,
  0x0e, 0x01,
  0x4c, 0x00,
  0xff, 0x00,
  0xe5, 0x7f,
  0xf9, 0xc0,
  0x41, 0x24,
  0xe0, 0x14,
  0x76, 0xff,
  0x33, 0xa0,
  0x42, 0x20,
  0x43, 0x18,
  0x4c, 0x00,
  0x87, 0xd0,
  0x88, 0x3f,
  0xd7, 0x03,
  0xd9, 0x10,
  0xd3, 0x82,
  0xc8, 0x08,
  0xc9, 0x80,
  0x7d, 0x00,
  0x7c, 0x03,
  0x7d, 0x48,
  0x7c, 0x08,
  0x7d, 0x20,
  0x7d, 0x10,
  0x7d, 0x0e,
  0x90, 0x00,
  0x91, 0x0e,
  0x91, 0x1a,
  0x91, 0x31,
  0x91, 0x5a,
  0x91, 0x69,
  0x91, 0x75,
  0x91, 0x7e,
  0x91, 0x88,
  0x91, 0x8f,
  0x91, 0x96,
  0x91, 0xa3,
  0x91, 0xaf,
  0x91, 0xc4,
  0x91, 0xd7,
  0x91, 0xe8,
  0x91, 0x20,
  0x92, 0x00,
  0x93, 0x06,
  0x93, 0xe3,
  0x93, 0x02,
  0x93, 0x02,
  0x93, 0x00,
  0x93, 0x04,
  0x93, 0x00,
  0x93, 0x03,
  0x93, 0x00,
  0x93, 0x00,
  0x93, 0x00,
  0x93, 0x00,
  0x93, 0x00,
  0x93, 0x00,
  0x93, 0x00,
  0x96, 0x00,
  0x97, 0x08,
  0x97, 0x19,
  0x97, 0x02,
  0x97, 0x0c,
  0x97, 0x24,
  0x97, 0x30,
  0x97, 0x28,
  0x97, 0x26,
  0x97, 0x02,
  0x97, 0x98,
  0x97, 0x80,
  0x97, 0x00,
  0x97, 0x00,
  0xc3, 0xef,
  0xff, 0x00,
  0xba, 0xdc,
  0xbb, 0x08,
  0xb6, 0x24,
  0xb8, 0x33,
  0xb7, 0x20,
  0xb9, 0x30,
  0xb3, 0xb4,
  0xb4, 0xca,
  0xb5, 0x43,
  0xb0, 0x5c,
  0xb1, 0x4f,
  0xb2, 0x06,
  0xc7, 0x00,
  0xc6, 0x51,
  0xc5, 0x11,
  0xc4, 0x9c,
  0xbf, 0x00,
  0xbc, 0x64,
  0xa6, 0x00,
  0xa7, 0x1e,
  0xa7, 0x6b,
  0xa7, 0x47,
  0xa7, 0x33,
  0xa7, 0x00,
  0xa7, 0x23,
  0xa7, 0x2e,
  0xa7, 0x85,
  0xa7, 0x42,
  0xa7, 0x33,
  0xa7, 0x00,
  0xa7, 0x23,
  0xa7, 0x1b,
  0xa7, 0x74,
  0xa7, 0x42,
  0xa7, 0x33,
  0xa7, 0x00,
  0xa7, 0x23,
  0xc0, 0xc8,
  0xc1, 0x96,
  0x8c, 0x00,
  0x86, 0x3d,
  0x50, 0x92,
  0x51, 0x90,
  0x52, 0x2c,
  0x53, 0x00,
  0x54, 0x00,
  0x55, 0x88,
  0x5a, 0x50,
  0x5b, 0x3c,
  0x5c, 0x00,
  0xd3, 0x04,
  0x7f, 0x00,
  0xda, 0x00,
  0xe5, 0x1f,
  0xe1, 0x67,
  0xe0, 0x00,
  0xdd, 0x7f,
  0x05, 0x00,
  0xff, 0x00,
  0xe0, 0x04,
  0xc0, 0xc8,
  0xc1, 0x96,
  0x86, 0x3d,
  0x50, 0x92,
  0x51, 0x90,
  0x52, 0x2c,
  0x53, 0x00,
  0x54, 0x00,
  0x55, 0x88,
  0x57, 0x00,
  0x5a, 0x50,
  0x5b, 0x3c,
  0x5c, 0x00,
  0xd3, 0x04,
  0xe0, 0x00,
  0xFF, 0x00,
  0x05, 0x00,
  0xDA, 0x08,
  0xda, 0x09,
  0x98, 0x00,
  0x99, 0x00,
  0x00, 0x00,
};

/**
  * @brief  ��ʼ����������ͷʹ�õ�GPIO(I2C/DCMI)
  * @param  None
  * @retval None
  */
void OV2640_HW_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  I2C_InitTypeDef  I2C_InitStruct;

	  RCC_AHB1PeriphClockCmd(DCMI_PWDN_GPIO_CLK|DCMI_VSYNC_GPIO_CLK | DCMI_HSYNC_GPIO_CLK | DCMI_PIXCLK_GPIO_CLK|
			  DCMI_D0_GPIO_CLK| DCMI_D1_GPIO_CLK| DCMI_D2_GPIO_CLK| DCMI_D3_GPIO_CLK|
			  DCMI_D4_GPIO_CLK| DCMI_D5_GPIO_CLK| DCMI_D6_GPIO_CLK| DCMI_D7_GPIO_CLK, ENABLE);

	  /*����/ͬ���ź���*/
	  GPIO_InitStructure.GPIO_Pin = DCMI_VSYNC_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	  GPIO_Init(DCMI_VSYNC_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_VSYNC_GPIO_PORT, DCMI_VSYNC_PINSOURCE, DCMI_VSYNC_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_HSYNC_GPIO_PIN ;
	  GPIO_Init(DCMI_HSYNC_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_HSYNC_GPIO_PORT, DCMI_HSYNC_PINSOURCE, DCMI_HSYNC_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_PIXCLK_GPIO_PIN ;
	  GPIO_Init(DCMI_PIXCLK_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_PIXCLK_GPIO_PORT, DCMI_PIXCLK_PINSOURCE, DCMI_PIXCLK_AF);
    
    GPIO_InitStructure.GPIO_Pin = DCMI_PWDN_GPIO_PIN ;
	  GPIO_Init(DCMI_PWDN_GPIO_PORT, &GPIO_InitStructure);
    /*PWDN���ţ��ߵ�ƽ�رյ�Դ���͵�ƽ����*/
    GPIO_ResetBits(DCMI_PWDN_GPIO_PORT,DCMI_PWDN_GPIO_PIN);
		
		/*�����ź�*/
	  GPIO_InitStructure.GPIO_Pin = DCMI_D0_GPIO_PIN ;
	  GPIO_Init(DCMI_D0_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D0_GPIO_PORT, DCMI_D0_PINSOURCE, DCMI_D0_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D1_GPIO_PIN ;
	  GPIO_Init(DCMI_D1_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D1_GPIO_PORT, DCMI_D1_PINSOURCE, DCMI_D1_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D2_GPIO_PIN ;
	  GPIO_Init(DCMI_D2_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D2_GPIO_PORT, DCMI_D2_PINSOURCE, DCMI_D2_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D3_GPIO_PIN ;
	  GPIO_Init(DCMI_D3_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D3_GPIO_PORT, DCMI_D3_PINSOURCE, DCMI_D3_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D4_GPIO_PIN ;
	  GPIO_Init(DCMI_D4_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D4_GPIO_PORT, DCMI_D4_PINSOURCE, DCMI_D4_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D5_GPIO_PIN ;
	  GPIO_Init(DCMI_D5_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D5_GPIO_PORT, DCMI_D5_PINSOURCE, DCMI_D5_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D6_GPIO_PIN ;
	  GPIO_Init(DCMI_D6_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D6_GPIO_PORT, DCMI_D6_PINSOURCE, DCMI_D6_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D7_GPIO_PIN ;
	  GPIO_Init(DCMI_D7_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D7_GPIO_PORT, DCMI_D7_PINSOURCE, DCMI_D7_AF);

	  /****** ����I2C��ʹ��I2C������ͷ��SCCB�ӿ�ͨѶ*****/
	 /* ʹ��I2Cʱ�� */
	  RCC_APB1PeriphClockCmd(CAMERA_I2C_CLK, ENABLE);
	  /* ʹ��I2Cʹ�õ�GPIOʱ�� */
	  RCC_AHB1PeriphClockCmd(CAMERA_I2C_SCL_GPIO_CLK|CAMERA_I2C_SDA_GPIO_CLK, ENABLE);
	  /* ��������Դ */
	  GPIO_PinAFConfig(CAMERA_I2C_SCL_GPIO_PORT, CAMERA_I2C_SCL_SOURCE, CAMERA_I2C_SCL_AF);
	  GPIO_PinAFConfig(CAMERA_I2C_SDA_GPIO_PORT, CAMERA_I2C_SDA_SOURCE, CAMERA_I2C_SDA_AF);

	  /* ��ʼ��GPIO */
	  GPIO_InitStructure.GPIO_Pin = CAMERA_I2C_SCL_PIN ;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(CAMERA_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(CAMERA_I2C_SCL_GPIO_PORT, CAMERA_I2C_SCL_SOURCE, CAMERA_I2C_SCL_AF);

	  GPIO_InitStructure.GPIO_Pin = CAMERA_I2C_SDA_PIN ;
	  GPIO_Init(CAMERA_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

	  /*��ʼ��I2Cģʽ */
	  I2C_DeInit(CAMERA_I2C); 

	  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	  I2C_InitStruct.I2C_OwnAddress1 = 0xFE;
	  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	  I2C_InitStruct.I2C_ClockSpeed = 40000;

	  /* д������ */
	  I2C_Init(CAMERA_I2C, &I2C_InitStruct);
		
		 /* ʹ��I2C */
	  I2C_Cmd(CAMERA_I2C, ENABLE);
}

/**
  * @brief  Resets the OV2640 camera.
  * @param  None
  * @retval None
  */
void OV2640_Reset(void)
{
	/*OV2640������Ĵ���������0xFF�Ĵ�����ֵΪ0��Ϊ1ʱ��ѡ��ʹ�ò�ͬ��ļĴ���*/
  OV2640_WriteReg(OV2640_DSP_RA_DLMT, 0x01);
  OV2640_WriteReg(OV2640_SENSOR_COM7, 0x80);
}

/**
  * @brief  ��ȡ����ͷ��ID.
  * @param  OV2640ID: �洢ID�Ľṹ��
  * @retval None
  */
void OV2640_ReadID(OV2640_IDTypeDef *OV2640ID)
{
	/*OV2640������Ĵ���������0xFF�Ĵ�����ֵΪ0��Ϊ1ʱ��ѡ��ʹ�ò�ͬ��ļĴ���*/
  OV2640_WriteReg(OV2640_DSP_RA_DLMT, 0x01);
	
	/*��ȡ�Ĵ�оƬID*/
  OV2640ID->Manufacturer_ID1 = OV2640_ReadReg(OV2640_SENSOR_MIDH);
  OV2640ID->Manufacturer_ID2 = OV2640_ReadReg(OV2640_SENSOR_MIDL);
  OV2640ID->PIDH = OV2640_ReadReg(OV2640_SENSOR_PIDH);
  OV2640ID->PIDL = OV2640_ReadReg(OV2640_SENSOR_PIDL);
}

/**
  * @brief  ���� DCMI/DMA �Բ�������ͷ����
  * @param  None
  * @retval None
  */
void OV2640_Init(void)
{
  DCMI_InitTypeDef DCMI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /*** ����DCMI�ӿ� ***/
  /* ʹ��DCMIʱ�� */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);

  /* DCMI ����*/ 
  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
  DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_Low;
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
  DCMI_Init(&DCMI_InitStructure); 	
	
	//��ʼ���䣬�Ӻ��濪ʼһ����ɨ��������ʵ�����ݷ�ת
	//dma_memory ��16λ����Ϊ��λ�� dma_bufsize��32λ����Ϊ��λ(�����ظ���/2)
  OV2640_DMA_Config((uint32_t)g_dma_target_ptr, g_dma_buf_size);

	/* �����ж� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* �����ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn ;//DMA�������ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);
	
 	/* ����֡�жϣ����յ�֡ͬ���źžͽ����ж� */
	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn ;	//֡�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DCMI_ITConfig (DCMI_IT_FRAME,ENABLE);	 	
}


/**
  * @brief  ���� DCMI/DMA �Բ�������ͷ����
	* @param  DMA_Memory0BaseAddr:���δ����Ŀ���׵�ַ
  * @param DMA_BufferSize�����δ����������(��λΪ��,��4�ֽ�)
  */
void OV2640_DMA_Config(uint32_t DMA_Memory0BaseAddr,uint16_t DMA_BufferSize)
{

  DMA_InitTypeDef  DMA_InitStructure;
  
  /* ����DMA��DCMI�л�ȡ����*/
  /* ʹ��DMA*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
  DMA_Cmd(DMA2_Stream1,DISABLE);
  while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);

  DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;	//DCMI���ݼĴ�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = DMA_Memory0BaseAddr; //DMA�����Ŀ�ĵ�ַ(����Ĳ���)	
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = DMA_BufferSize; 						//��������ݴ�С(����Ĳ���)
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//�Ĵ�����ַ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC8;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  /*DMA�ж����� */
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
  
  DMA_Cmd(DMA2_Stream1,ENABLE);
	while(DMA_GetCmdStatus(DMA2_Stream1) != ENABLE);
}

/**
  * @brief  ����ͼ�������С��OV2640���ͼ��Ĵ�С(�ֱ���),��ȫ�ɸú���ȷ��
  * @param  width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical),width��height������4�ı���
  * @retval 0,���óɹ�������,����ʧ��
  */
u8 OV2640_OutSize_Set(u16 width,u16 height)
{
	u16 outh;
	u16 outw;
	u8 temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	OV2640_WriteReg(0XFF,0X00);	
	OV2640_WriteReg(0XE0,0X04);	
    OV2640_WriteReg(0X50,outw&0X00);		//����v_divider hdivider
	OV2640_WriteReg(0X5A,outw&0XFF);		//����OUTW�ĵͰ�λ
	OV2640_WriteReg(0X5B,outh&0XFF);		//����OUTH�ĵͰ�λ
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	OV2640_WriteReg(0X5C,temp);				//����OUTH/OUTW�ĸ�λ 
	OV2640_WriteReg(0XE0,0X00);	
	return 0;
}

/**δ����*/
/**
  * @brief  ����ͼ��ߴ��С,Ҳ������ѡ��ʽ������ֱ���
  * @param  width,height:ͼ���Ⱥ�ͼ��߶�
  * @retval 0,���óɹ�������,����ʧ��
  */
u8 OV2640_ImageSize_Set(u16 width,u16 height)
{
	u8 temp;
	OV2640_WriteReg(0XFF,0X00);
	OV2640_WriteReg(0XE0,0X04);
	OV2640_WriteReg(0XC0,(width)>>3&0XFF);		//����HSIZE��10:3λ
	OV2640_WriteReg(0XC1,(height)>>3&0XFF);		//����VSIZE��10:3λ
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80;
	OV2640_WriteReg(0X8C,temp);
	OV2640_WriteReg(0XE0,0X00);
	return 0;
}

/**
  * @brief  ����OV2640ΪUXGAģʽ�����������ͼ���С
  * @param  None
  * @retval None
  */
void OV2640_UXGAConfig(void)
{
  uint32_t i;
	
	/*����ͷ��λ*/
  OV2640_Reset();

	/*�������μĴ���д�룬ȷ������д������
	(��ʹ������ͷ������ʱ��IIC�����߸��Žϴ󣬱�����д������֤����)*/
  /* д��Ĵ������� */
  for(i=0; i<(sizeof(OV2640_UXGA)/2); i++)
  {
    OV2640_WriteReg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);

  }
	  /* Initialize OV2640 */
  for(i=0; i<(sizeof(OV2640_UXGA)/2); i++)
  {
    OV2640_WriteReg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);

  }
	  /* Initialize OV2640 */
  for(i=0; i<(sizeof(OV2640_UXGA)/2); i++)
  {
    OV2640_WriteReg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);

  }

  OV2640_OutSize_Set(g_img_width, g_img_height);   
}

/**
  * @brief  дһ�ֽ����ݵ�OV2640�Ĵ���
  * @param  Addr: OV2640 �ļĴ�����ַ
  * @param  Data: Ҫд�������
  * @retval ����0��ʾд��������0xFF��ʾ����
  */
uint8_t OV2640_WriteReg(uint16_t Addr, uint8_t Data)
{
  uint32_t timeout = DCMI_TIMEOUT_MAX;
  
  /* Generate the Start Condition */
  I2C_GenerateSTART(CAMERA_I2C, ENABLE);

  /* Test on CAMERA_I2C EV5 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
   
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(CAMERA_I2C, OV2640_DEVICE_WRITE_ADDRESS, I2C_Direction_Transmitter);
 
  /* Test on CAMERA_I2C EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
 
  /* Send CAMERA_I2C location address LSB */
  I2C_SendData(CAMERA_I2C, (uint8_t)(Addr));

  /* Test on CAMERA_I2C EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
  
  /* Send Data */
  I2C_SendData(CAMERA_I2C, Data);

  /* Test on CAMERA_I2C EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }  
 
  /* Send CAMERA_I2C STOP Condition */
  I2C_GenerateSTOP(CAMERA_I2C, ENABLE);
  
  /* If operation is OK, return 0 */
  return 0;
}

/**
  * @brief  ��OV2640�Ĵ����ж�ȡһ���ֽڵ�����
  * @param  Addr: �Ĵ�����ַ
  * @retval ���ض�ȡ�õ�����
  */
uint8_t OV2640_ReadReg(uint16_t Addr)
{
  uint32_t timeout = DCMI_TIMEOUT_MAX;
  uint8_t Data = 0;

  /* Generate the Start Condition */
  I2C_GenerateSTART(CAMERA_I2C, ENABLE);

  /* Test on CAMERA_I2C EV5 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(CAMERA_I2C, OV2640_DEVICE_READ_ADDRESS, I2C_Direction_Transmitter);
 
  /* Test on CAMERA_I2C EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 

  /* Send CAMERA_I2C location address LSB */
  I2C_SendData(CAMERA_I2C, (uint8_t)(Addr));

  /* Test on CAMERA_I2C EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Clear AF flag if arised */
  CAMERA_I2C->SR1 |= (uint16_t)0x0400;

  /* Generate the Start Condition */
  I2C_GenerateSTART(CAMERA_I2C, ENABLE);
  
  /* Test on CAMERA_I2C EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(CAMERA_I2C, OV2640_DEVICE_READ_ADDRESS, I2C_Direction_Receiver);
   
  /* Test on CAMERA_I2C EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }  
 
  /* Prepare an NACK for the next data received */
  I2C_AcknowledgeConfig(CAMERA_I2C, DISABLE);

  /* Test on CAMERA_I2C EV7 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }   
    
  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(CAMERA_I2C, ENABLE);

  /* Receive the Data */
  Data = I2C_ReceiveData(CAMERA_I2C);

  /* return the read data */
  return Data;
}

