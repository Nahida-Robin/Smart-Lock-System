#include "stm32f4xx.h"                  // Device header

#define IR_TIM_CHANNEL TIM_CHANNEL_1
#define IR_ACT_CHANNEL HAL_TIM_ACTIVE_CHANNEL_1
#define IR_IC_PORT GPIOC
#define IR_IC_PIN GPIO_PIN_6
#define IR_IC_THR 300

extern TIM_HandleTypeDef htim8;

volatile uint16_t Width = 0;
volatile uint16_t LastValue = 0;
volatile uint8_t IR_Ready = 0;
volatile uint8_t IR_Process_Ready = 0;
uint8_t IR_Addr[8] = {0};
uint8_t IR_Addr_Index = 0;
uint8_t IR_ReAddr[8] = {0};
uint8_t IR_ReAddr_Index = 0;
uint8_t IR_Data[8] = {0};
uint8_t IR_Data_Index = 0;
uint8_t IR_ReData[8] = {0};
uint8_t IR_ReData_Index = 0;
uint8_t IDLE_Step = 0;
uint8_t LEADER_Step = 0;
uint8_t ADDR_Step = 0;
uint8_t DATA_Step = 0;
uint8_t TAIL_Step = 0;
uint32_t Now = 0;

typedef enum{
	IR_IDLE = 0,
	IR_LEADER,
	IR_ADDR,
	IR_DATA,
	IR_TAIL,
	IR_VERT
}IR_State_t;

volatile IR_State_t IR_State = IR_IDLE;

void IR_Init(void)
{
	HAL_TIM_IC_Start_IT(&htim8, IR_TIM_CHANNEL);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM8)
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			uint32_t now = HAL_TIM_ReadCapturedValue(&htim8, IR_TIM_CHANNEL);
			if(now > LastValue){Width = now - LastValue;}
			else {Width = 0xFFFF - LastValue + now;}
			LastValue = now;
			IR_Ready = 1;
//			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN))
//			{
//				IR_Process_High(Width);
//			}
//			else
//			{
//				IR_Process_Low(Width);
//			}
		}
}
	
static uint8_t Width_Thre(uint16_t width, uint16_t hope)
{
	if(width > hope - IR_IC_THR && width < hope + IR_IC_THR)return 1;
	else return 0;
}

static void IR_Addr_Clear(void)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		IR_Addr[i] = 0;
	}
	IR_Addr_Index = 0;
}

static void IR_ReAddr_Clear(void)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		IR_ReAddr[i] = 0;
	}
	IR_ReAddr_Index = 0;
}

static void IR_Data_Clear(void)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		IR_Data[i] = 0;
	}
	IR_Data_Index = 0;
}

static void IR_ReData_Clear(void)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		IR_ReData[i] = 0;
	}
	IR_ReData_Index = 0;
}

static void IR_Reset(void)
{
    IR_Addr_Clear();
    IR_ReAddr_Clear();
    IR_Data_Clear();
    IR_ReData_Clear();

    IDLE_Step = 0;
    LEADER_Step = 0;
    ADDR_Step = 0;
    DATA_Step = 0;
    TAIL_Step = 0;

    IR_State = IR_IDLE;
}


static void IDLE_Task(void)
{
	if(IR_Ready == 0)return;
	IR_Ready = 0;
	switch(IDLE_Step)
	{
		case 0:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 0 && Width_Thre(Width, 9000))
			{
				IDLE_Step++;
				break;
			}
			else break;
		case 1:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 1 && Width_Thre(Width, 4500))
			{
				IR_State = IR_LEADER;
				IDLE_Step = 0;
				Now = HAL_GetTick();
				break;
			}
			else
			{
				IDLE_Step = 0;
				break;
			}
	}
}

static void LEADER_Task(void)
{
	if(HAL_GetTick() - Now > 100){IR_State = IR_IDLE;return;}
	if(IR_Ready == 0)return;
	IR_Ready = 0;	
	switch(LEADER_Step)
	{
		case 0:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 0 && Width_Thre(Width, 560))
			{
				LEADER_Step++;
				break;
			}	
			else break;
		case 1:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 1 && Width_Thre(Width, 1690))
			{
				IR_Addr[IR_Addr_Index++] = 1;
				LEADER_Step = 0;
				if(IR_Addr_Index == 8){IR_State = IR_ADDR;LEADER_Step = 0;}
				break;
			}
			else if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 1 && Width_Thre(Width, 565))
			{
				IR_Addr[IR_Addr_Index++] = 0;
				LEADER_Step = 0;
				if(IR_Addr_Index == 8){IR_State = IR_ADDR;LEADER_Step = 0;}
				break;
			}
			else
			{
				IR_Addr_Clear();
				IR_State = IR_IDLE;
				break;
			}
	}
}

static void ADDR_Task(void)
{
	if(HAL_GetTick() - Now > 100){IR_State = IR_IDLE;return;}
	if(IR_Ready == 0)return;
	IR_Ready = 0;	
	switch(ADDR_Step)
	{
		case 0:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 0 && Width_Thre(Width, 560))
			{
				ADDR_Step++;
				break;
			}	
			else break;
		case 1:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 1 && Width_Thre(Width, 1690))
			{
				IR_ReAddr[IR_ReAddr_Index++] = 1;
				ADDR_Step = 0;
				if(IR_ReAddr_Index == 8){IR_State = IR_DATA;ADDR_Step = 0;}
				break;
			}
			else if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 1 && Width_Thre(Width, 565))
			{
				IR_ReAddr[IR_ReAddr_Index++] = 0;
				ADDR_Step = 0;
				if(IR_ReAddr_Index == 8){IR_State = IR_DATA;ADDR_Step = 0;}
				break;
			}
			else
			{
				IR_ReAddr_Clear();
				IR_State = IR_IDLE;
				break;
			}
	}
}

static void DATA_Task(void)
{
	if(HAL_GetTick() - Now > 100){IR_State = IR_IDLE;return;}
	if(IR_Ready == 0)return;
	IR_Ready = 0;	
	switch(DATA_Step)
	{
		case 0:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 0 && Width_Thre(Width, 560))
			{
				DATA_Step++;
				break;
			}	
			else break;
		case 1:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 1 && Width_Thre(Width, 1690))
			{
				IR_Data[IR_Data_Index++] = 1;
				DATA_Step = 0;
				if(IR_Data_Index == 8){IR_State = IR_TAIL;DATA_Step = 0;}
				break;
			}
			else if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 1 && Width_Thre(Width, 565))
			{
				IR_Data[IR_Data_Index++] = 0;
				DATA_Step = 0;
				if(IR_Data_Index == 8){IR_State = IR_TAIL;DATA_Step = 0;}
				break;
			}
			else
			{
				IR_Data_Clear();
				IR_State = IR_IDLE;
				break;
			}
	}
}

static void TAIL_Task(void)
{
	if(HAL_GetTick() - Now > 100){IR_State = IR_IDLE;return;}
	if(IR_Ready == 0)return;
	IR_Ready = 0;	
	switch(TAIL_Step)
	{
		case 0:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 0 && Width_Thre(Width, 560))
			{
				TAIL_Step++;
				break;
			}	
			else break;
		case 1:
			if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 1 && Width_Thre(Width, 1690))
			{
				IR_ReData[IR_ReData_Index++] = 1;
				TAIL_Step = 0;
//				if(IR_ReData_Index == 8 && HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 0 && Width_Thre(Width, 560)){IR_State = IR_VERT;TAIL_Step = 0;}
				if(IR_ReData_Index == 8){IR_State = IR_VERT;TAIL_Step = 0;}
				break;
			}
			else if(HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 1 && Width_Thre(Width, 565))
			{
				IR_ReData[IR_ReData_Index++] = 0;
				TAIL_Step = 0;
//				if(IR_ReData_Index == 8 && HAL_GPIO_ReadPin(IR_IC_PORT, IR_IC_PIN) == 0 && Width_Thre(Width, 560)){IR_State = IR_VERT;TAIL_Step = 0;}
				if(IR_ReData_Index == 8){IR_State = IR_VERT;TAIL_Step = 0;}
				break;
			}
			else
			{
				IR_ReData_Clear();
				IR_State = IR_IDLE;
				break;
			}
	}
}

static void VERT_Task(void)
{
	if(HAL_GetTick() - Now > 100){IR_State = IR_IDLE;return;}
	for(uint8_t i = 0; i < 8; i++)
	{
		if(IR_Addr[i] == IR_ReAddr[i] || IR_Data[i] == IR_ReData[i])
		{
			IR_Process_Ready = 0;
		  IR_Addr_Clear();
			IR_Data_Clear();
			IR_ReAddr_Clear();
			IR_ReData_Clear();
			IR_State = IR_IDLE;
			return;
		}
	}
	IR_Process_Ready = 1;
	IR_Addr_Clear();
	IR_Data_Clear();
	IR_ReAddr_Clear();
	IR_ReData_Clear();
	IR_State = IR_IDLE;
	return;
}

void IR_Process(void)
{
	switch(IR_State)
	{
		case IR_IDLE: IDLE_Task();break;
		case IR_LEADER: LEADER_Task();break;
		case IR_ADDR: ADDR_Task();break;
		case IR_DATA: DATA_Task();break;
		case IR_TAIL: TAIL_Task();break;
		case IR_VERT: VERT_Task();break;
	}
}
	