/** 
 * @file Password.c
 * @brief 密码认证和修改
 * @author Nahida
 * @date 2026.5.22
 */
 
#include "stm32f4xx.h"                  // Device header

#include "Matrix.h"
#include "RTCTime.h"
//#include "TickTime.h"
//#include "StepMotor.h"
//#include "Display.h"

#define ERR         0
#define STEP_OK     1
#define PWD_OK      2
#define ERR_COM     3
#define ERR_SPE     4
#define CHA_OK      5
#define STEP_RETURN 6
#define CONFIRM     7
#define RESELECT    8
#define LEN_ERR    -1
#define PWD_ERR    -2
#define MEM_ERR    -3
#define ARR_ERR    -4

#define PASSWORD_MAX_LENGTH 8
#define PASSWORD_MIN_LENGTH 4
//#define USERWORDMEMORY_SIZE 256
#define USERWORDMEMORY_END 'A'
//#define TIMEMEMORY_SIZE 64
#define TIMEMEMORY_END 'Z'
#define UW_MAX_SIZE 512
#define PW_MAX_SIZE 512
#define TM_MAX_SIZE 128

volatile uint8_t Password[PASSWORD_MAX_LENGTH] = {1,0,2,7,1,0,2,7};
volatile uint8_t Password1[PASSWORD_MAX_LENGTH] = {1,0,2,7,1,0,2,7};
volatile uint8_t Password2[PASSWORD_MAX_LENGTH] = {2,0,2,6,2,0,2,6};
volatile uint8_t Password3[PASSWORD_MAX_LENGTH] = {6,2,9,4,3,2,0,0};
volatile uint8_t Userword[PASSWORD_MAX_LENGTH] = {0};
volatile uint8_t NewPassWord[PASSWORD_MAX_LENGTH] = {0};
//volatile uint8_t UserwordMemory[USERWORDMEMORY_SIZE] = {0};
//volatile uint8_t TimeMemory[TIMEMEMORY_SIZE] = {0};

uint8_t Password_Length = 8;
static uint8_t Password1_Length = 8;
static uint8_t Password2_Length = 8;
static uint8_t Password3_Length = 6;

extern uint8_t PasswordMemory[PW_MAX_SIZE];
extern uint8_t UserwordMemory[UW_MAX_SIZE];
extern uint8_t TimeMemory[TM_MAX_SIZE];

extern volatile uint16_t PWMemory_Index;
extern volatile uint16_t UWMemory_Index;
extern volatile uint16_t TMMemory_Index;

//static uint8_t Key = 0;
uint8_t Userword_Index = 0;
uint8_t Password_Index = 0;
//static uint16_t ReadUserwordMemory_Index = 0;
uint8_t Vertify_State = 0;
uint8_t Warnings = 0;
uint8_t Succeses = 0;
static uint8_t Password_Sel = 0;

/**
  *@brief 用户密码清空
  *@param NULL
  *@retval NULL
  */
static void Userword_Clear()
{
	for(uint8_t i = 0; i < PASSWORD_MAX_LENGTH; i++)
	{
		Userword[i] = 0;
	}
	Userword_Index = 0;
}	

/**
  *@brief 正确密码清空
  *@param NULL
  *@retval NULL
  */
static void Password_Clear()
{
	for(uint8_t i = 0; i <PASSWORD_MAX_LENGTH; i++)
	{
		Password[i] = 0;
	}
}

/**
  *@brief 临时密码清空
  *@param NULL
  *@retval NULL
  */
static void NewPassword_Clear()
{
	for(uint8_t i = 0; i < PASSWORD_MAX_LENGTH; i++)
	{
		NewPassWord[i] = 0;
	}
	Password_Index = 0;
}

/**
  *@brief 临时密码更新到正确密码
  *@param NULL
  *@retval NULL
  */
static void NewPassword_Set()
{
	Password_Clear();
	
	for(uint8_t i = 0; i < Password_Length; i++)
	{
		Password[i] = NewPassWord[i];
	}
	
	NewPassword_Clear();
}

/**
  *@brief 有效密码切换
  *@param	NULL
  *@retval STEP_OK 1 成功
	*        ERR     0 失败
	*		 STEP_RETURN 6 返回上一级
  */
int8_t Password_Select(uint8_t Key)
{
	// Key = Matrix_GetNum();  // 馗用外部传入
	
	switch(Key)
	{
		case 11:
				return CONFIRM;
		case 13:
				return RESELECT;
		case 14:
				Password_Sel++;
				if(Password_Sel>2){Password_Sel = 0;}
				switch(Password_Sel)
				{
					case 0:
						Password_Clear();
						for(uint8_t i = 0; i < Password1_Length; i++)
						{
							Password[i] = Password1[i];
						}	
						Password_Length = Password1_Length;
						return STEP_OK;
					case 1:
						Password_Clear();
						for(uint8_t i = 0; i < Password2_Length; i++)
						{
							Password[i] = Password2[i];
						}
						Password_Length = Password2_Length;
						return STEP_OK;
					case 2:
						Password_Clear();
						for(uint8_t i = 0; i < Password3_Length; i++)
						{
							Password[i] = Password3[i];
						}
						Password_Length = Password3_Length;
						return STEP_OK;
					default:
						return ERR;
				}
			case 16:
				return STEP_RETURN;
			default:
				return ERR;
	}
	
}

/**
  *@brief 更新当前密码到对应密码
  *@param NULL 
  *@retval STEP_OK 1 成功
  *        ERR     0	失败	
  */
static int8_t Password_Refresh()
{
	switch(Password_Sel)
	{
		case 0:
			for(uint8_t i = 0; i < PASSWORD_MAX_LENGTH; i++)
			{
		  	Password1[i] = 0;
			}
			for(uint8_t i = 0; i < Password_Length; i++)
			{
				Password1[i] = Password[i];
			}
			Password1_Length = Password_Length;
			return STEP_OK;
		case 1:
			for(uint8_t i = 0; i < PASSWORD_MAX_LENGTH; i++)
			{
		  	Password2[i] = 0;
			}
			for(uint8_t i = 0; i < Password_Length; i++)
		  {
		  	Password2[i] = Password[i];
		  }
			Password2_Length = Password_Length;
		  return STEP_OK;
		case 2:
			for(uint8_t i = 0; i < PASSWORD_MAX_LENGTH; i++)
			{
		  	Password3[i] = 0;
			}
			for(uint8_t i = 0; i < Password_Length; i++)
		  {
		  	Password3[i] = Password[i];
		  }
			Password3_Length = Password_Length;
		  return STEP_OK;
		default: 
			return ERR;
	}
}

/**
  *@brief 密码历史记录
  *@param NULL
  *@retval STEP_OK  1 成功 
  *        ERR      0 当前密码为空
  *		   LEN_ERR -1 长度不对
  */
static int8_t UserwordToMemory()
{
	if(Userword_Index == 0){return ERR;}
	if(UWMemory_Index + Userword_Index + 1 > UW_MAX_SIZE){return LEN_ERR;}
	for(uint8_t i = 0; i < Userword_Index; i++)
	{
		UserwordMemory[UWMemory_Index++] = Userword[i];
	}
	UserwordMemory[UWMemory_Index++] = USERWORDMEMORY_END;
	return STEP_OK;
}

/**
  *@brief 存储时间
  *@param NULL
  *@retval ERR     0 存储失败
  *        STEP_OK 1 成功
  */
static int8_t TimeToMemory()
{
	uint8_t Hour = 0, Min = 0, Sec = 0;
	uint8_t Year = 0, Mon = 0, Week = 0, Date = 0;
	if(TMMemory_Index + 4 > TM_MAX_SIZE){return ERR;}
	RTC_GetTime(&Hour, &Min, &Sec);
	RTC_GetDate(&Year, &Mon, &Week, &Date);//此处只是清除影子寄存器
	TimeMemory[TMMemory_Index++] = Hour;
	TimeMemory[TMMemory_Index++] = Min;
	TimeMemory[TMMemory_Index++] = Sec;
	TimeMemory[TMMemory_Index++] = TIMEMEMORY_END;
	return STEP_OK;
}

/**
  *@brief 密码认证
  *@param NULL
  *@retval STEP_OK  1 步骤成功
  *        PWD_OK   2 认证成功
  *        LEN_ERR -1 长度错误
  *        PWD_ERR -2 密码错误
  *        MEM_ERR -3 存储失败
	*        STEP_RETURN 6 返回上一级 
  */
int8_t Password_Vertify(uint8_t Key)
{
	// Key = Matrix_GetNum();  // 用外部传入
	
	switch(Key)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			if(Userword_Index < PASSWORD_MAX_LENGTH)//输入八位后禁止输入
			{
				Userword[Userword_Index++] = Key;
				return STEP_OK;
			}
			else
			{
				Userword_Clear();
				return LEN_ERR;
			}
		case 10:
			if(Userword_Index < PASSWORD_MAX_LENGTH)//输入八位后禁止输入
			{
				Userword[Userword_Index++] = 0;
				return STEP_OK;
			}
			else
			{
				Userword_Clear();
				return LEN_ERR;
			}
		case 11://确定
			if(Password_Length < PASSWORD_MIN_LENGTH || Password_Length > PASSWORD_MAX_LENGTH)//验证长度合法
			{
					Userword_Clear();
					Vertify_State = 0;
				  Warnings++;
					return LEN_ERR;
			}
			
			if(UserwordToMemory() != STEP_OK){return MEM_ERR;}
			if(TimeToMemory() != STEP_OK){return ERR;}
			
			
			if(Userword_Index != Password_Length)//长度不对就不对了
			{
				Userword_Clear();
				Warnings++;
				return PWD_ERR;
			}
			
			for(uint8_t i = 0; i < Password_Length; i++)//长度对了再挨个比较
			{
				if(Userword[i] != Password[i])
					{	
						Userword_Clear();
						Warnings++;
						return PWD_ERR;
					}
			}

			Userword_Clear();
			Vertify_State = 1;//认证成功
			Warnings = 0;
			Succeses++;
//			Display_ShowLockFreq();
			return PWD_OK;
		case 12://开锁
			if(Vertify_State != 1){return 0;}
//			Display_ShowLockState(1);
//			StepMotor_Forward_90_Degree(1);
			return STEP_OK;
		case 13://关锁
			if(Vertify_State != 1){return 0;}
//			OLED_ShowLockState(0);
//			StepMotor_Forward_90_Degree(-1);
			Vertify_State = 0;
			return STEP_OK;
		case 14://删除
			if(Userword_Index > 0)//防止数组越界
			{
				Userword_Index--;
				Userword[Userword_Index] = 0;				
			}
			return STEP_OK;
		case 16:
			Userword_Index = 0;
			Userword_Clear();
			return STEP_RETURN;
	}

}

/**
  *@brief 修改密码
  *@param NULL
  *@retval STEP_OK     1 步骤成功
  *        CHA_OK      5 修改成功
  *        LEN_ERR    -1 长度错误	
  *        STEP_RETURN 6 返回上一级
  */
int8_t Password_Change(uint8_t Key)
{
	// Key = Matrix_GetNum();  // 馗用外部传入
//	if(Vertify_State != 1)//认证状态下才可修改
//		{
//			return -1;
//		}
	
	switch(Key)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			if(Password_Index < PASSWORD_MAX_LENGTH)
			{
				NewPassWord[Password_Index++] = Key; 
				return STEP_OK;
			}
			else
			{
				NewPassword_Clear();
				return LEN_ERR;
			}
		case 10:
			if(Password_Index < PASSWORD_MAX_LENGTH)
			{
				NewPassWord[Password_Index++] = 0; 
				return STEP_OK;
			}
			else
			{
				NewPassword_Clear();
				return LEN_ERR;
			}			
		case 11://确定
			if(Password_Index < PASSWORD_MIN_LENGTH)
				{
					NewPassword_Clear();
					return LEN_ERR;
				}
			Password_Length = Password_Index;
			NewPassword_Set();
			Password_Refresh();	
			// Vertify_State = 0;
			return CHA_OK;
		case 14://删除
			if(Password_Index > 0)
			{
				Password_Index--;
				NewPassWord[Password_Index] = 0;				
			}
			return STEP_OK;
		case 16:
			return STEP_RETURN;
	}
}



/**
  *@brief 密码错误次数报警
  *@param NULL
  *@retval ERR_COM 3 错误一次普通报警
  *        ERR_SPE 4 错误三次以上特殊报警
  *        STEP_OK 1 无错误次数
  */
uint8_t Open_Error()
{
	if(Warnings == 1)
	{
		return ERR_COM;
	}
	else if(Warnings >= 3)
	{
		return ERR_SPE;
	}
	else
	{
		return STEP_OK;
	}
}
