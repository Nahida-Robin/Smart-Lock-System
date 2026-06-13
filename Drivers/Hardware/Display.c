/** 
 * @file Display.c
 * @brief  OLED显示封装，隔离OLED底层驱动
 * @author Nahida
 * @date 2026.5.26
 */

#include "stm32f4xx.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include "stm32f4xx_hal.h"
#include "Password.h"
#include "RTCTime.h"

#define PASSWORD_MAX_LENGTH 8
#define UW_MAX_SIZE 512
#define PW_MAX_SIZE 512
#define TM_MAX_SIZE 128

extern uint8_t Temph;
extern uint8_t Templ;
extern uint8_t Humih;
extern uint8_t Humil;
extern uint8_t Light;
extern float Smog;
extern uint8_t Succeses;
extern uint8_t Warnings;
extern uint8_t Vertify_State;
extern volatile uint8_t Userword[PASSWORD_MAX_LENGTH];
extern volatile uint8_t NewPassWord[PASSWORD_MAX_LENGTH];
extern volatile uint8_t Password[PASSWORD_MAX_LENGTH];
extern uint8_t Userword_Index;
extern uint8_t Password_Index;
extern uint8_t Password_Length;
extern uint8_t Read_Flag;
extern uint8_t View_Flag;
extern uint8_t TempThrehold;
extern uint8_t HumiThrehold;
extern uint16_t SmogThrehold;
extern uint8_t Brightness;
extern uint8_t Motor_ON_Tem;
extern uint8_t Motor_MAX_Tem;
extern uint8_t Control_Mode;
extern uint8_t RTCSet_Mode;
extern uint8_t View_Pages;
extern uint8_t View_Index;

extern uint8_t PasswordMemory[PW_MAX_SIZE];
extern uint8_t UserwordMemory[UW_MAX_SIZE];
extern uint8_t TimeMemory[TM_MAX_SIZE];

extern volatile uint16_t PWMemory_Index;
extern volatile uint16_t UWMemory_Index;
extern volatile uint16_t TMMemory_Index;

volatile uint8_t UW_Memory[16][9] = {0};
volatile uint8_t TM_Memory[16][9] = {0};
volatile uint8_t Rec_Count = 0;
uint8_t Hour = 0, Min = 0, Sec = 0;
uint8_t Year = 0, Mon = 0, Week = 0, Date = 0;

/**
  *@brief  显示锁状态
  *@param NULL
  *@retval NULL
  */
void Display_ShowLockState(void)
{
	if(Vertify_State)
	{
		OLED_ShowString(2, 1, "Open ");
	}
	else
	{
		OLED_ShowString(2, 1, "Close");
	}
}

/**
  *@brief  显示开锁次数
  *@param NULL
  *@retval NULL
  */
void Display_ShowLockSuc(void)
{
	OLED_ShowNum(2, 5, Succeses, 2);
}

/**
  *@brief 显示错误次数
  *@param NULL
  *@retval NULL
  */
void Display_ShowLockErr(void)
{
	OLED_ShowNum(2, 11, Warnings, 2);
}

/**
  *@brief 空闲显示日期时间占位符
  *@param NULL
  *@retval NULL
  */
void Display_ShowIDLE(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "20  -  -  ");
	OLED_ShowString(2, 1, "  :  :   Week:");
	OLED_ShowString(3, 1, "11 To Vert");
	OLED_ShowString(4, 1, "12 To Menu");
}

/**
  *@brief 显示时间
  *@param NULL
  *@retval NULL
  */
void Display_ShowTime(void)
{
	RTC_GetTime(&Hour, &Min, &Sec);
	OLED_ShowNum(2, 1, Hour, 2);
	OLED_ShowNum(2, 4, Min, 2);
	OLED_ShowNum(2, 7, Sec, 2);
}

/**
  *@brief 显示日期
  *@param NULL
  *@retval NULL
  */
void Display_ShowDate(void)
{
	RTC_GetDate(&Year, &Mon, &Week, &Date);
	OLED_ShowNum(1, 3, Year, 2);
	OLED_ShowNum(1, 6, Mon, 2);
	OLED_ShowNum(1, 9, Date, 2); 
	OLED_ShowNum(1, 15, Week, 2);
}

/**
  *@brief 显示设置时间
  *@param NULL
  *@retval NULL
  */
void Display_ShowSetTime(void)
{
	OLED_ShowNum(2, 1, Hour, 2);
	OLED_ShowNum(2, 4, Min, 2);
	OLED_ShowNum(2, 7, Sec, 2);
}

/**
  *@brief 显示设置日期
  *@param NULL
  *@retval NULL
  */
void Display_ShowSetDate(void)
{
	OLED_ShowNum(1, 3, Year, 2);
	OLED_ShowNum(1, 6, Mon, 2);
	OLED_ShowNum(1, 9, Date, 2); 
	OLED_ShowNum(1, 15, Week, 2);
}

/**
  *@brief 显示RTC界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowRTC(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "11 To Conf");
	OLED_ShowString(2, 1, "12 To Sele");
	OLED_ShowString(3, 1, "13 To Incr");
	OLED_ShowString(4, 1, "14 To Decr");
}

/**
  *@brief 显示RTC设置占位符
  *@param NULL
  *@retval NULL
  */
void Display_ShowRTCSetInc(void)
{
	OLED_Clear();
	OLED_ShowString(4, 1, "NowChange:");
	OLED_ShowString(1, 1, "20  -  -   Wk:");
	OLED_ShowString(2, 1, "  :  :   ");
	Display_ShowTime();
	Display_ShowDate();
}

/**
  *@brief 显示RTC当前设置对象
  *@param NULL
  *@retval NULL
  */
void Display_ShowRTCSet(void)
{
	switch(RTCSet_Mode)
	{
		case 1:
			OLED_ShowString(4, 11, "Year");
			break;
		case 2:
			OLED_ShowString(4, 11, "Mon ");
			break;
		case 3:
			OLED_ShowString(4, 11, "Date");
			break;
		case 4:
			OLED_ShowString(4, 11, "Hour");
			break;
		case 5:
			OLED_ShowString(4, 11, "Min ");
			break;
		case 6:
			OLED_ShowString(4, 11, "Sec ");
			break;
		case 7:
			OLED_ShowString(4, 11, "Week");
			break;
	}	
}
	
/**
  *@brief 显示主界面占位符
  *@param NULL
  *@retval NULL
  */
void Display_ShowMainMenu(void)
{
	OLED_Clear();
//	OLED_ShowString(1, 1, "11 To Ver");
	OLED_ShowString(1, 1, "15 To Set RTC");
	OLED_ShowString(2, 1, "Suc:  Err:  ");
	OLED_ShowString(3, 3, "C");
	OLED_ShowString(3, 7, "%");
	OLED_ShowString(4, 5, "ppm");
	OLED_ShowString(4, 11, "%");
}

/**
  *@brief 显示温湿度
  *@param NULL
  *@retval NULL
  */
void Display_ShowDHT(void)
{
	OLED_ShowNum(3, 1, Temph, 2);
	OLED_ShowNum(3, 5, Humih, 2);
}

/**
  *@brief 显示烟雾数据
  *@param NULL
  *@retval NULL
  */
void Display_ShowSmog(void)
{
	OLED_ShowNum(4, 1, Smog, 4);
}

/**
  *@brief 显示光照数据
  *@param NULL
  *@retval NULL
  */
void Display_ShowLight(void)
{
	OLED_ShowNum(4, 9, Light, 2);
}

/**
  *@brief 显示认证成功
  *@param NULL
  *@retval NULL
  */
void Display_ShowOK(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Vertify OK");
}

/**
  *@brief 显示输入引导
  *@param NULL
  *@retval NULL
  */
void Display_InputInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Please Press");
	OLED_ShowString(3, 1, "11 To Confirm");
	OLED_ShowString(4, 1, "14 To Delete");
	
}

/**
  *@brief 显示输入的字符
  *@param NULL
  *@retval NULL
  */
void Display_ShowInput(void)
{
	OLED_ShowNum(2, Userword_Index, Userword[Userword_Index - 1], 1);
	OLED_ShowString(2, Userword_Index + 1, "  ");
}

/**
  *@brief 显示开锁状态
  *@param NULL
  *@retval NULL
  */
void Display_ShowOpen(void)
{
	OLED_Clear();
	// OLED_ShowString(1, 1, "Already Open");
	// HAL_Delay(500);
	// OLED_Clear();
	OLED_ShowString(1, 1, "12 To Control");
	OLED_ShowString(2, 1, "13 To Select");
	OLED_ShowString(3, 1, "14 To Change");
	OLED_ShowString(4, 1, "15 To View");
}

/**
  *@brief 显示控制界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowControl(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "12 To SelMode");
	OLED_ShowString(2, 1, "13 To Increase");
	OLED_ShowString(3, 1, "14 To Decrease");
	OLED_ShowString(4, 1, "16 To Return");
}

/**
  *@brief 显示温度报警阈值
  *@param NULL
  *@retval NULL
  */
void Display_ShowTemAla(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Set Tem Alarm");
	OLED_ShowNum(2, 1, TempThrehold, 2);
}

/**
  *@brief 显示温度阈值
  *@param NULL
  *@retval NULL
  */
void Display_ShowTem(void)
{
	OLED_ShowNum(2, 1, TempThrehold, 2);
}

/**
  *@brief 显示湿度报警阈值
  *@param NULL
  *@retval NULL
  */
void Display_ShowHumAla(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Set Hum Alarm");
	OLED_ShowNum(2, 1, HumiThrehold, 2);
}

/**
  *@brief 显示湿度阈值
  *@param NULL
  *@retval NULL
  */
void Display_ShowHum(void)
{
	OLED_ShowNum(2, 1, HumiThrehold, 2);
}

/**
  *@brief 显示烟雾报警阈值
  *@param NULL
  *@retval NULL
  */
void Display_ShowSmogAla(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Set Smog Alarm");
	OLED_ShowNum(2, 1, SmogThrehold, 4);
}

/**
  *@brief 显示烟雾阈值
  *@param NULL
  *@retval NULL
  */
void Display_ShowSmo(void)
{
	OLED_ShowNum(2, 1, SmogThrehold, 4);
}

/**
  *@brief 显示LED亮度设置界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowLEDSet(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Set LED Bright");
	OLED_ShowNum(2, 1, Brightness, 2);
}

/**
  *@brief 显示LED亮度
  *@param NULL
  *@retval NULL
  */
void Display_ShowLED(void)
{
	OLED_ShowNum(2, 1, Brightness, 3);
}

/**
  *@brief 显示风扇开启温度设置界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowOnTem(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Set On_Tem");
	OLED_ShowNum(2, 1, Motor_ON_Tem, 2);
}

/**
  *@brief 显示风扇开启温度
  *@param NULL
  *@retval NULL
  */
void Display_ShowMoOn(void)
{
	OLED_ShowNum(2, 1, Motor_ON_Tem, 2);
}

/**
  *@brief 显示风扇全速温度设置界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowMaxTem(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Set Max_Tem");
	OLED_ShowNum(2, 1, Motor_MAX_Tem, 2);
}

/**
  *@brief 显示风扇全速温度
  *@param NULL
  *@retval NULL
  */
void Display_ShowMoOff(void)
{
	OLED_ShowNum(2, 1, Motor_MAX_Tem, 2);
}

/**
  *@brief 显示设置界面，根据Control_Mode显示不同的设置界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowSet(void)
{
	switch(Control_Mode)
	{
			case 1: 
				Display_ShowTemAla();
				break;
			case 2:
				Display_ShowHumAla();
				break;
			case 3:
				Display_ShowSmogAla();
				break;
			case 4:
				Display_ShowLEDSet();
				break;
			case 5:
				Display_ShowOnTem();
				break;
			case 6:
				Display_ShowMaxTem();
				break;	
	}
}


/**
  *@brief 显示密码修改引导
  *@param NULL
  *@retval NULL
  */
void Display_ChaInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Press New PW");
	OLED_ShowString(3, 1, "11 To Confirm");
	OLED_ShowString(4, 1, "14 To Delete");
}

/**
  *@brief 显示密码修改界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowChaPwd(void)
{
	OLED_ShowNum(2, Password_Index, NewPassWord[Password_Index - 1], 1);
	OLED_ShowString(2, Password_Index + 1, "  ");
}

/**
  *@brief 显示密码记录引导
  *@param NULL
  *@retval NULL
  */
void Display_ViewInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Password records");
}

/**
  *@brief 读取历史记录到缓存数组
  *@param NULL
  *@retval -1 已经读取过了
  *         1  成功读取记录
  */
int8_t Display_ReadView(void)
{
	if(Read_Flag){return -1;}
	Read_Flag = 1;
	View_Flag = 0;
	View_Index = 1;
	Rec_Count = 0;
	View_Pages = 1;
	for(uint8_t i = 0; i < 16; i++)
	{
		for(uint8_t j = 0; j < 9; j++)
		{
			UW_Memory[i][j] = 0;
			TM_Memory[i][j] = 0;
		}
	}
	uint8_t Uw_Line = 0;
	uint8_t Tm_Line = 0;
		uint16_t Uw_Index = 0, Tm_Index = 0;
		uint8_t Uw_Pos = 0, Tm_Pos = 0;
		if(UWMemory_Index == 0 || TMMemory_Index == 0)
		{
			OLED_Clear();
			OLED_ShowString(1, 1, "No Record");
			HAL_Delay(300);
			return -1;
		}
		while(Uw_Index < UWMemory_Index && Tm_Index < TMMemory_Index && Uw_Line < 16 && Tm_Line < 16)
		{
			while(Uw_Index < UWMemory_Index && UserwordMemory[Uw_Index] != 'A' && Uw_Pos < 8)
			{
				UW_Memory[Uw_Line][Uw_Pos++] = UserwordMemory[Uw_Index];
				Uw_Index++;
			}
			if(Uw_Index < UWMemory_Index && UserwordMemory[Uw_Index] == 'A')
			{
				UW_Memory[Uw_Line++][Uw_Pos++] = 'A';
				Uw_Index++;
				Uw_Pos = 0;
			}
			else
			{
				break;
			}
			while(Tm_Index < TMMemory_Index && TimeMemory[Tm_Index] != 'Z' && Tm_Pos < 8)
			{
				TM_Memory[Tm_Line][Tm_Pos++] = TimeMemory[Tm_Index];
				Tm_Index++;
			}
			if(Tm_Index < TMMemory_Index && TimeMemory[Tm_Index] == 'Z')
			{
				TM_Memory[Tm_Line++][Tm_Pos++] = 'Z';
				Tm_Index++;
				Tm_Pos = 0;
			}
			else
			{
				break;
			}
		}
		Rec_Count = Uw_Line < Tm_Line ? Uw_Line : Tm_Line;
		View_Pages = (Rec_Count + 2) / 3;
		if(View_Pages == 0){View_Pages = 1;}
	return 1;
}

/**
  *@brief 显示密码记录界面，最多显示3条记录，超过3条记录时翻页显示
  *@param NULL
  *@retval NULL
  */
void Display_ShowHistory(void)
{
	if(View_Flag)return;
	View_Flag = 1;
	OLED_Clear();
	if(Rec_Count == 0)
	{
		OLED_ShowString(1, 1, "No Records");
		return;
	}
	if(View_Index < 1){View_Index = 1;}
	if(View_Index > View_Pages){View_Index = View_Pages;}
	OLED_ShowString(1, 1, "Record: Page:");
	OLED_ShowNum(1, 14, View_Index, 1);
	uint8_t Rec_Index = 0;
	uint8_t End_Index = 0;
	uint8_t Line = 2;
	Rec_Index = (View_Index - 1) * 3;
	End_Index = View_Index * 3;
	if(End_Index > Rec_Count){End_Index = Rec_Count;}
	while(Rec_Index < End_Index)
	{
		uint8_t U_Col = 1, T_Col = 10;
		uint8_t UShow_Index = 0, TShow_Index = 0;
		while(UShow_Index < 8 && UW_Memory[Rec_Index][UShow_Index] != 'A')
		{
			OLED_ShowNum(Line, U_Col++, UW_Memory[Rec_Index][UShow_Index], 1);
			UShow_Index++;
		}
		while(TShow_Index < 8 && TM_Memory[Rec_Index][TShow_Index] != 'Z')
		{
			OLED_ShowNum(Line, T_Col, TM_Memory[Rec_Index][TShow_Index], 2);
			T_Col += 2;
			TShow_Index++;
		}
		Line++;
		Rec_Index++;
	}
}

/**
  *@brief 显示选择界面引导
  *@param NULL
  *@retval NULL
  */
void Display_SelInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "14 To Selexct");
}

/**
  *@brief 显示选择界面
  *@param NULL
  *@retval NULL
  */
void Display_Showsel(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Now Password");
	OLED_ShowString(3, 1, "11 To Confirm");
	OLED_ShowString(4, 1, "14 To Resel");
	uint8_t PW_Index = 0;
	while(PW_Index < Password_Length)
	{
		PW_Index++;
		OLED_ShowNum(2, PW_Index, Password[PW_Index - 1], 1);
	}
}

/**
  *@brief 显示选择错误界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowSelErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Password Sel Err");
}

/**
  *@brief 显示长度错误界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowLenErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Len Err");
}

/**
  *@brief 显示密码错误界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowPwdErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Pwd Err"); 
}

/**
  *@brief 显示内存错误界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowMemErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Mem Err"); 
}

/**
  *@brief 显示密码修改成功界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowChaOK(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Change OK");
	HAL_Delay(300);
}

/**
  *@brief 显示密码修改错误界面
  *@param NULL
  *@retval NULL
  */
void Display_ShowChaErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Change Err");
	OLED_ShowString(2, 1, "Len Err");
	HAL_Delay(300);
}
