/** 
 * @file Flash.c
 * @brief Flash存储器驱动 W25Q16 大小为2MB，页大小为256B，扇区大小为4KB
 * @author Nahida
 * @date 2026.6.4
 */

#include "stm32f4xx.h"                  // Device header
#include "Delay.h"
#include "cmsis_os.h"

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
  *@brief 向Flash写入一个字节
  *@param byte 要写入的字节
  *@retval NULL
  */
void Flash_WriteByte(uint8_t byte)
{
	HAL_SPI_Transmit(&hspi1, &byte, 1, 100); 
}

/**
  *@brief 向Flash读写一个字节
  *@param byte 要写入的字节
  *@retval RxByte 从Flash读取的字节
  */
uint8_t Flash_WriteRead(uint8_t byte)
{
	uint8_t RxByte;
	HAL_SPI_TransmitReceive(&hspi1, &byte, &RxByte, 1, 100);
	return RxByte;
}

/**
  *@brief 读取Flash ID
  *@param NULL
  *@retval Flash_ID Flash ID值
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
  *@brief 读取Flash状态寄存器
  *@param NULL
  *@retval SRStatus 状态寄存器值
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
  *@brief 写入Flash状态寄存器
  *@param sr 要写入的状态寄存器值
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
  *@brief 等待Flash空闲
  *@param NULL
  *@retval NULL
  */
void Flash_WaitBusy(void)
{
	while((Flash_ReadSR() & 0x01) == 0x01);
//	do
//	{
//		osDelay(1);
//	}while((Flash_ReadSR() & 0x01) == 0x01);
}

/**
  *@brief 使Flash进入掉电模式
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
  *@brief 使Flash退出掉电模式
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
  *@brief 使Flash进入写使能状态
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
  *@brief 使Flash退出写使能状态
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
  *@brief 从Flash读取数据
  *@param buf 存储读取数据的缓冲区
  *@param addr 读取的起始地址
  *@param size 读取的数据大小
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
  *@brief 向Flash写入一页数据
  *@param buf 存储要写入数据的缓冲区
  *@param addr 写入的起始地址
  *@param size 写入的数据大小
  *@retval NULL
  */
void Flash_WritePage(uint8_t *buf, uint32_t addr, uint16_t size)
{
	Flash_EnableWrite();//写使能
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
  *@brief 向Flash写入数据（不检查）
  *@param buf 存储要写入数据的缓冲区
  *@param addr 写入的起始地址
  *@param size 写入的数据大小
  *@retval NULL
  */
void Flash_Write_NoCheck(uint8_t *buf, uint32_t addr, uint16_t size)
{
//	uint16_t PageRemain = PAGEBYTE - size % PAGEBYTE;//当前页剩余空间
	uint16_t PageRemain = 0;
	if(size < PAGEBYTE)PageRemain = size;
	else PageRemain = PAGEBYTE;
	
	while(1)
	{
		Flash_WritePage(buf, addr, PageRemain);//写满当前页
		if(size == PageRemain)break;//写完了 
		else
		{
			buf += PageRemain;//数组位置偏移
			addr += PageRemain;//数据地址偏移
			
			size -= PageRemain;//剩余数据数量
			if(size > PAGEBYTE)PageRemain = PAGEBYTE;//当前页还是不够写
			else PageRemain = size;
		}
	}
}

/**
  *@brief 擦除整个Flash芯片
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
  *@brief 擦除Flash扇区
  *@param sec_addr 扇区编号
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
  *@brief 向Flash写入数据
  *@param buf 存储要写入数据的缓冲区
  *@param addr 写入的起始地址
  *@param size 写入的数据大小
  *@retval NULL
  */
uint8_t BUF[4096];
void Flash_Write(uint8_t *buf, uint32_t addr, uint16_t size)
{
	uint8_t *Flash_Buf = BUF;
	uint16_t i = 0;
	uint16_t SecPos = addr / SECTORBYTE;//扇区序号 0，1，2...
	uint16_t SecOff = addr % SECTORBYTE;//扇区下的地址偏移
	uint16_t SecRemain = SECTORBYTE - SecOff;//扇区剩余空间
	if(size < SecRemain)SecRemain = size;//这个扇区能写满了
	while(1)
	{
		Flash_Read(Flash_Buf, SecPos * SECTORBYTE, SECTORBYTE);//读出整个扇区
		for(i = 0; i < SecRemain; i++)
		{
			if(Flash_Buf[SecOff + i] != 0xFF)break;//如果扇区不全是0xFF可写，就break擦除
		}
		if(i < SecRemain)//如果上一个for读完全是0xFF就不会进这个if去擦除整个扇区
		{
			Flash_EraseSector(SecPos);
			for(i = 0; i < SecRemain; i++)
			{
				Flash_Buf[SecOff + i] = buf[i];//复制要写的内容
			}
			Flash_Write_NoCheck(Flash_Buf, SecPos * SECTORBYTE, SECTORBYTE);//写入整个扇区
		}
		else
		{
			Flash_Write_NoCheck(buf, addr, SecRemain);//不需要擦除的话直接写满这个扇区
		}
		if(SecRemain == size)break;//写完了
		else
		{
			SecPos++;
			SecOff = 0;
			buf += SecRemain;//数组位置偏移
			addr += SecRemain;//数据地址偏移
			size -= SecRemain;//剩余数据数量
			if(size > SECTORBYTE)SecRemain = SECTORBYTE;//下一个扇区还是不够写
			else SecRemain = size;
		}
	}
}

