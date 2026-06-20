/** 
 * @file Flash.c
 * @brief Flash驱动，W25Q16 
 * @author Nahida
 * @date 2026.6.4
 */

#include "stm32f4xx.h"                  // Device header
#include "Delay.h"

#define FLASH_ID 0xEF14
#define FLASH_PIN GPIOB
#define FLASH_PORT GPIO_PIN_0
#define FLASH_CS(x) HAL_GPIO_WritePin(FLASH_PIN, FLASH_PORT, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define PAGEBYTE 256
#define SECTORBYTE 4096

#define FLASH_WriteEnable		0x06 
#define FLASH_WriteDisable		0x04 
#define FLASH_ReadStatusReg		0x05 
#define FLASH_WriteStatusReg		0x01 
#define FLASH_ReadData			0x03 
#define FLASH_FastReadData		0x0B 
#define FLASH_FastReadDual		0x3B 
#define FLASH_PageProgram		0x02 
#define FLASH_BlockErase			0xD8 
#define FLASH_SectorErase		0x20 
#define FLASH_ChipErase			0xC7 
#define FLASH_PowerDown			0xB9 
#define FLASH_ReleasePowerDown	0xAB 
#define FLASH_DeviceID			0xAB 
#define FLASH_ManufactDeviceID	0x90 
#define FLASH_JedecDeviceID		0x9F 
#define FLASH_WriteNULL     0x00
#define FLASH_WriteFULL     0xFF

extern SPI_HandleTypeDef hspi1;

/**
  *@brief Flash写字节
  *@param byte 要写入的字节
  *@retval NULL
  */
void Flash_WriteByte(uint8_t byte)
{
	HAL_SPI_Transmit(&hspi1, &byte, 1, 100); 
}

/**
  *@brief  Flash写并读
  *@param byte 写入的字节
  *@retval RxByte 读到的字节
  */
uint8_t Flash_WriteRead(uint8_t byte)
{
	uint8_t RxByte;
	HAL_SPI_TransmitReceive(&hspi1, &byte, &RxByte, 1, 100);
	return RxByte;
}

/**
  *@brief 获取Flash的ID
  *@param NULL
  *@retval Flash_ID Flash IDֵ
  */
uint16_t Flash_ReadID(void)
{
	uint16_t Flash_ID = 0;
	FLASH_CS(0);
	Flash_WriteByte(FLASH_ManufactDeviceID); 
	Flash_WriteByte(FLASH_WriteNULL); 
	Flash_WriteByte(FLASH_WriteNULL); 
	Flash_WriteByte(FLASH_WriteNULL); 
	Flash_ID |= Flash_WriteRead(FLASH_WriteFULL)<<8;
	Flash_ID |= Flash_WriteRead(FLASH_WriteFULL);
	FLASH_CS(1);
	return Flash_ID;
}

/**
  *@brief Flash读状态寄存器
  *@param NULL
  *@retval SRStatus Flash状态
  */
uint8_t Flash_ReadSR(void)
{
	uint8_t SRStatus = 0;
	FLASH_CS(0);
	Flash_WriteByte(FLASH_ReadStatusReg);
	SRStatus = Flash_WriteRead(FLASH_WriteFULL);
	FLASH_CS(1);
	return SRStatus;
}

/**
  *@brief Flash写状态寄存器
  *@param sr Flash状态
  *@retval NULL
  */
void Flash_WriteSR(uint8_t sr)
{
	FLASH_CS(0);
	Flash_WriteByte(FLASH_WriteStatusReg);
	Flash_WriteByte(sr);
	FLASH_CS(1);
}

/**
  *@brief Flash等待忙
  *@param NULL
  *@retval NULL
  */
void Flash_WaitBusy(void)
{
	while((Flash_ReadSR() & 0x01) == 0x01);
}

/**
  *@brief Flash掉电
  *@param NULL
  *@retval NULL
  */
void Flash_PowerDown(void)
{
	FLASH_CS(0);
	Flash_WriteByte(FLASH_PowerDown);
	FLASH_CS(1);
	Delay_us(3);
}

/**
  *@brief Flash上电
  *@param NULL
  *@retval NULL
  */
void Flash_PowerOn(void)
{
	FLASH_CS(0);
	Flash_WriteByte(FLASH_ReleasePowerDown);
	FLASH_CS(1);
	Delay_us(3);
}

/**
  *@brief Flash写使能
  *@param NULL
  *@retval NULL
  */
void Flash_EnableWrite(void)
{
	FLASH_CS(0);
	Flash_WriteByte(FLASH_WriteEnable);
	FLASH_CS(1);
}

/**
  *@brief Flash写失能
  *@param NULL
  *@retval NULL
  */
void Flash_DisableWrite(void)
{
	FLASH_CS(0);
	Flash_WriteByte(FLASH_WriteDisable);
	FLASH_CS(1);
}

/**
  *@brief Flash读数据
  *@param buf 读取缓冲数组
  *@param addr 地址
  *@param size 大小
  *@retval NULL
  */
void Flash_Read(uint8_t *buf, uint32_t addr, uint16_t size)
{
	uint16_t i;
	FLASH_CS(0);
	Flash_WriteByte(FLASH_ReadData);
	Flash_WriteByte((uint8_t)((addr) >> 16));
	Flash_WriteByte((uint8_t)((addr) >> 8));
	Flash_WriteByte((uint8_t)(addr));
	for(i = 0; i < size; i++)
	{
		buf[i] = Flash_WriteRead(FLASH_WriteFULL);
	}
	FLASH_CS(1);
}

/**
  *@brief Flash写一页
  *@param buf 写入缓冲数组
  *@param addr 地址
  *@param size 大小
  *@retval NULL
  */
void Flash_WritePage(uint8_t *buf, uint32_t addr, uint16_t size)
{
	Flash_EnableWrite();
	FLASH_CS(0);
	Flash_WriteByte(FLASH_PageProgram);
	Flash_WriteByte((uint8_t)((addr) >> 16));
	Flash_WriteByte((uint8_t)((addr) >> 8));
	Flash_WriteByte((uint8_t)(addr));
	for(uint16_t i = 0; i < size; i++)
	{
		Flash_WriteByte(buf[i]);
	}
	FLASH_CS(1);
	Flash_WaitBusy();
}

/**
  *@brief Flash不检查写
  *@param buf 写入缓冲数组
  *@param addr 地址
  *@param size 大小
  *@retval NULL
  */
void Flash_Write_NoCheck(uint8_t *buf, uint32_t addr, uint16_t size)
{
//	uint16_t PageRemain = PAGEBYTE - size % PAGEBYTE;//当前页剩余大小
	// uint16_t PageRemain = 0;
	// if(size < PAGEBYTE)PageRemain = size;
	// else PageRemain = PAGEBYTE;
	uint16_t PageRemain = PAGEBYTE - (addr % PAGEBYTE);
	if(PageRemain > size) PageRemain = size;		
	while(1)
	{
		Flash_WritePage(buf, addr, PageRemain);//写满当前页
		if(size == PageRemain)break;//写完了
		else
		{
			buf += PageRemain;
			addr += PageRemain;
			
			size -= PageRemain;
			if(size > PAGEBYTE)PageRemain = PAGEBYTE;
			else PageRemain = size;
		}
	}
}

/**
  *@brief 擦除整个芯片
  *@param NULL
  *@retval NULL
  */
void Flash_EraseChip(void)
{
	Flash_EnableWrite();
	Flash_WaitBusy();
	FLASH_CS(0);
	Flash_WriteByte(FLASH_ChipErase);
	FLASH_CS(1);
	Flash_WaitBusy();
}

/**
  *@brief Flash擦除扇区
  *@param sec_addr 扇区序号
  *@retval NULL
  */
void Flash_EraseSector(uint32_t sec_addr)
{
	sec_addr *= SECTORBYTE;
	Flash_EnableWrite();
	Flash_WaitBusy();
	FLASH_CS(0);
	Flash_WriteByte(FLASH_SectorErase);
	Flash_WriteByte((uint8_t)((sec_addr) >> 16));
	Flash_WriteByte((uint8_t)((sec_addr) >> 8));
	Flash_WriteByte((uint8_t)sec_addr);
	FLASH_CS(1);
	Flash_WaitBusy();
}

/**
  *@brief Flash写数据
  *@param buf 数组
  *@param addr 地址
  *@param size 大小
  *@retval NULL
  */
uint8_t BUF[4096];
void Flash_Write(uint8_t *buf, uint32_t addr, uint16_t size)
{
	uint8_t *Flash_Buf = BUF;
	uint16_t i = 0;
	uint16_t SecPos = addr / SECTORBYTE;//扇区序号
	uint16_t SecOff = addr % SECTORBYTE;//扇区地址偏移
	uint16_t SecRemain = SECTORBYTE - SecOff;
	if(size < SecRemain)SecRemain = size;
	while(1)
	{
		Flash_Read(Flash_Buf, SecPos * SECTORBYTE, SECTORBYTE);
		for(i = 0; i < SecRemain; i++)
		{
			if(Flash_Buf[SecOff + i] != 0xFF)break;//检查扇区是否是新的
		}
		if(i < SecRemain)
		{
			Flash_EraseSector(SecPos);
			for(i = 0; i < SecRemain; i++)
			{
				Flash_Buf[SecOff + i] = buf[i];
			}
			Flash_Write_NoCheck(Flash_Buf, SecPos * SECTORBYTE, SECTORBYTE);
		}
		else
		{
			Flash_Write_NoCheck(buf, addr, SecRemain);
		}
		if(SecRemain == size)break;
		else
		{
			SecPos++;
			SecOff = 0;
			buf += SecRemain;
			addr += SecRemain;
			size -= SecRemain;
			if(size > SECTORBYTE)SecRemain = SECTORBYTE;
			else SecRemain = size;
		}
	}
}

