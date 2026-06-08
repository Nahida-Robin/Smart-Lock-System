/** 
 * @file Datamanage.c
 * @brief 统一的数据管理层
 * @author Nahida
 * @date 2026.6.5
 */

#include "stm32f4xx.h"                  // Device header
#include "Flash.h"

#define FLASH_WR_ADDR 0x004000
#define FLASH_LEN_ADDR 0x006000 //~0x006002
#define FLASH_UW_ADDR 0x008000 //~0x0081FF
#define FLASH_PW_ADDR 0x012000 //~0x0121FF
#define FLASH_TM_ADDR 0x010000 //~0x01007F
#define FLASH_UW_NUM 8
#define FLASH_PW_NUM 12
#define FLASH_TM_NUM 10
#define UW_MAX_SIZE 512
#define PW_MAX_SIZE 512
#define TM_MAX_SIZE 128

uint8_t UserwordMemory[UW_MAX_SIZE] = {0};
uint8_t PasswordMemory[PW_MAX_SIZE] = {0};
uint8_t TimeMemory[TM_MAX_SIZE] = {0};
uint8_t Flash_IsWrite[1] = {0};

volatile uint16_t UWMemory_Index = 0;
volatile uint16_t PWMemory_Index = 0;
volatile uint16_t TMMemory_Index = 0;

/**
  *@brief 擦除数据所在扇区
  *@param NULL
  *@retval NULL
  */
void Data_Erase(void)
{
	Flash_EraseSector(FLASH_UW_NUM);
	Flash_EraseSector(FLASH_PW_NUM);
	Flash_EraseSector(FLASH_TM_NUM);
	uint8_t Len[6] = {0};
	Flash_Write(Len, FLASH_LEN_ADDR, 6);
}

/**
  *@brief 读取数据所在扇区的数据
  *@param NULL
  *@retval NULL
  */
void Data_Read(void)
{
	Flash_Read(Flash_IsWrite, FLASH_WR_ADDR, 1);
	if(Flash_IsWrite[0] == 0xA5)
	{
		uint8_t Len[6] = {0};
		Flash_Read(Len, FLASH_LEN_ADDR, 6);
		UWMemory_Index = (Len[0] << 8) | Len[1];
		PWMemory_Index = (Len[2] << 8) | Len[3];
		TMMemory_Index = (Len[4] << 8) | Len[5];
		if(UWMemory_Index > UW_MAX_SIZE || PWMemory_Index > PW_MAX_SIZE || TMMemory_Index > TM_MAX_SIZE)
		{
			Data_Erase();
			UWMemory_Index = 0;
			PWMemory_Index = 0;
			TMMemory_Index = 0;
		}
//		if(UWMemory_Index > UW_MAX_SIZE) UWMemory_Index = 0;
//		if(PWMemory_Index > PW_MAX_SIZE) PWMemory_Index = 0;
//		if(TMMemory_Index > TM_MAX_SIZE) TMMemory_Index = 0;
		Flash_Read(UserwordMemory, FLASH_UW_ADDR, UWMemory_Index);
		Flash_Read(PasswordMemory, FLASH_PW_ADDR, PWMemory_Index);
		Flash_Read(TimeMemory, FLASH_TM_ADDR, TMMemory_Index);	
	}
	else
	{
		uint8_t Flash_WriteYes[1];
		Flash_WriteYes[0] = 0xA5;
		Flash_Write(Flash_WriteYes, FLASH_WR_ADDR, 1);
		uint8_t Len[6] = {0};
		Flash_Write(Len, FLASH_LEN_ADDR, 6);
	}
}

/**
  *@brief 保存数据到对应的扇区
  *@param NULL
  *@retval NULL
  */
void Data_Save(void)
{
	Flash_Write(UserwordMemory, FLASH_UW_ADDR, UWMemory_Index);
	Flash_Write(PasswordMemory, FLASH_PW_ADDR, PWMemory_Index);
	Flash_Write(TimeMemory, FLASH_TM_ADDR, TMMemory_Index);
	uint8_t UWLen[2], PWLen[2], TMLen[2];
	UWLen[0] = (UWMemory_Index >> 8) & 0xFF;
	UWLen[1] = UWMemory_Index & 0xFF;
	PWLen[0] = (PWMemory_Index >> 8) & 0xFF;
	PWLen[1] = PWMemory_Index & 0xFF;
	TMLen[0] = (TMMemory_Index >> 8) & 0xFF;
	TMLen[1] = TMMemory_Index & 0xFF;
	Flash_Write(UWLen, FLASH_LEN_ADDR, 2);
	Flash_Write(PWLen, FLASH_LEN_ADDR + 2, 2);
	Flash_Write(TMLen, FLASH_LEN_ADDR + 4, 2);
}

