/** 
 * @file Datamanage.c
 * @brief 统一的数据管理层
 * @author Nahida
 * @date 2026.6.5
 */

#include "stm32f4xx.h"                  // Device header
#include "Flash.h"
#include "Password.h"

#define UW_MAX_SIZE 512
#define PW_MAX_SIZE 512
#define TM_MAX_SIZE 128
#define LEN_MAX_SIZE 10
#define PS_MAX_SIZE   1
#define PW_PS_MAX     2
#define PASSWORD_MAX_LENGTH 8
#define FLASH_WR_ADDR 0x004000
#define FLASH_MEM_ADDR 0x010000
#define FLASH_LEN_ADDR FLASH_MEM_ADDR
#define FLASH_PS_ADDR (FLASH_LEN_ADDR + LEN_MAX_SIZE)
#define FLASH_UW_ADDR (FLASH_PS_ADDR + PS_MAX_SIZE)
#define FLASH_TM_ADDR (FLASH_UW_ADDR + UW_MAX_SIZE)
#define FLASH_PW_ADDR (FLASH_TM_ADDR + TM_MAX_SIZE)
#define FLASH_WRC_ADDR (FLASH_PW_ADDR + 1)
#define FLASH_MEM_NUM 10

uint8_t UserwordMemory[UW_MAX_SIZE] = {0};
uint8_t TimeMemory[TM_MAX_SIZE] = {0};
uint8_t PasswordMemory[PW_MAX_SIZE] = {0};
uint8_t Flash_IsWrite[1] = {0};
uint8_t PW_Sel[1] = {0};
uint8_t FLASH_Buf[4096];

volatile uint16_t UWMemory_Index = 0;
volatile uint16_t TMMemory_Index = 0;
volatile uint16_t PWMemory_Index = 0;
uint8_t PW1_Len = 0, PW2_Len = 0, PW3_Len = 0;
//uint8_t WriteReadCount = 0;
//uint32_t FLASH_MEM_ADDR = FLASH_MM_ADDR;
//uint16_t FLASH_MEM_NUM = FLASH_MM_NUM;

extern uint8_t Password_Sel;
extern uint8_t Password1_Length;
extern uint8_t Password2_Length;
extern uint8_t Password3_Length;
extern volatile uint8_t Password1[PASSWORD_MAX_LENGTH];
extern volatile uint8_t Password2[PASSWORD_MAX_LENGTH];
extern volatile uint8_t Password3[PASSWORD_MAX_LENGTH];

/**
  *@brief 擦除数据所在扇区
  *@param NULL
  *@retval NULL
  */
void Data_Erase(void)
{
	Flash_EraseSector(FLASH_MEM_NUM);
	uint8_t Buf[11] = {0};
	Flash_Write(Buf, FLASH_LEN_ADDR, 11);
			UWMemory_Index = 0;
			TMMemory_Index = 0;
			PWMemory_Index = 0;
			Password_Sel = 0;
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
		uint8_t Buf[11] = {0};
		Flash_Read(Buf, FLASH_LEN_ADDR, 11);
		UWMemory_Index = (Buf[0] << 8) | Buf[1];
		TMMemory_Index = (Buf[2] << 8) | Buf[3];
		PW1_Len = (Buf[4] << 8) | Buf[5];
		PW2_Len = (Buf[6] << 8) | Buf[7];
		PW3_Len = (Buf[8] << 8) | Buf[9];
		Password_Sel = Buf[10];
		if(PW1_Len > PASSWORD_MAX_LENGTH || PW2_Len > PASSWORD_MAX_LENGTH || PW3_Len > PASSWORD_MAX_LENGTH)
		{
				Data_Erase();
				return;
		}
		PWMemory_Index = PW1_Len + PW2_Len + PW3_Len;// +1
		if(Password_Sel > PW_PS_MAX)Password_Sel = 0;
		if(UWMemory_Index > UW_MAX_SIZE || TMMemory_Index > TM_MAX_SIZE || PWMemory_Index > PW_MAX_SIZE)
		{
			Data_Erase();
			UWMemory_Index = 0;
			TMMemory_Index = 0;
			PWMemory_Index = 0;
			return;
		}
		Flash_Read(UserwordMemory, FLASH_UW_ADDR, UWMemory_Index);
		Flash_Read(TimeMemory, FLASH_TM_ADDR, TMMemory_Index);	
		Flash_Read(PasswordMemory, FLASH_PW_ADDR, PWMemory_Index);
		
		uint8_t i = 0, j = 0;
		for(i = 0; i < PW1_Len; i++)
		{
			Password1[i] = PasswordMemory[j++];
		}
		for(i = 0; i < PW2_Len; i++)
		{
			Password2[i] = PasswordMemory[j++];
		}
		for(i = 0; i < PW3_Len; i++)
		{
			Password3[i] = PasswordMemory[j++];
		}
		Password1_Length = PW1_Len; 
		Password2_Length = PW2_Len; 
		Password3_Length = PW3_Len; 
//		WriteReadCount = PasswordMemory[j++];
//		if(WriteReadCount > 100)
//		{
//			Flash_Read(FLASH_Buf, FLASH_MEM_ADDR, 4096);
//			FLASH_MEM_ADDR += 4096;
//			FLASH_MEM_NUM ++;
//			WriteReadCount = 0;
//			uint8_t buf[1] = {WriteReadCount};
//			Flash_Write(FLASH_Buf, FLASH_MEM_ADDR, 4096);
//			Flash_Write(buf, FLASH_WRC_ADDR, 1);
//		}
		for(uint8_t x = 0; x < PWMemory_Index; x++)
		{
			PasswordMemory[x] = 0;
		}
		PWMemory_Index = 0;
		PW_Select(Password_Sel);
	}
	else
	{
		uint8_t Flash_WriteYes[1];
		Flash_WriteYes[0] = 0xA5;
		Flash_Write(Flash_WriteYes, FLASH_WR_ADDR, 1);
		uint8_t Buf[11] = {0};
		Flash_Write(Buf, FLASH_LEN_ADDR, 11);
		PW_Select(Password_Sel);
	}
}

/**
  *@brief 保存数据到对应的扇区
  *@param NULL
  *@retval NULL
  */
void Data_Save(void)
{
//	WriteReadCount++;
	uint8_t i = 0, j = 0;
	for(j = 0; j < Password1_Length; j++)
	{
		PasswordMemory[i++] = Password1[j];
	}
	for(j = 0; j < Password2_Length; j++)
	{
		PasswordMemory[i++] = Password2[j];
	}
	for(j = 0; j < Password3_Length; j++)
	{
		PasswordMemory[i++] = Password3[j];
	}
//	PasswordMemory[i++] = WriteReadCount;
	Flash_Write(UserwordMemory, FLASH_UW_ADDR, UWMemory_Index);
	Flash_Write(TimeMemory, FLASH_TM_ADDR, TMMemory_Index);
	Flash_Write(PasswordMemory, FLASH_PW_ADDR, i);
	uint8_t Buf[11];
	Buf[0] = (UWMemory_Index >> 8) & 0xFF; 
	Buf[1] = UWMemory_Index & 0xFF;
	Buf[2] = (TMMemory_Index >> 8) & 0xFF; 
	Buf[3] = TMMemory_Index & 0xFF;
	Buf[4] = (Password1_Length >> 8) & 0xFF;
	Buf[5] = Password1_Length & 0xFF;
	Buf[6] = (Password2_Length >> 8) & 0xFF;
	Buf[7] = Password2_Length & 0xFF;
	Buf[8] = (Password3_Length >> 8) & 0xFF;
	Buf[9] = Password3_Length & 0xFF;
	Buf[10] = Password_Sel;
	Flash_Write(Buf, FLASH_LEN_ADDR, 11);
}



