/** 
 * @file Display.c
 * @brief 通过调用OLED接口显示状态，方便调试
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
  *@brief 通过OLED实时显示锁的状态
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
  *@brief 通过OLED实时显示开锁成功次数
  *@param NULL
  *@retval NULL
  */
void Display_ShowLockSuc(void)
{
	OLED_ShowNum(2, 5, Succeses, 2);
}

/**
  *@brief 通过OLED实时显示开锁失败次数
  *@param NULL
  *@retval NULL
  */
void Display_ShowLockErr(void)
{
	OLED_ShowNum(2, 11, Warnings, 2);
}

/**
  *@brief 通过OLED显示空闲状态
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
  *@brief 通过OLED显示时间
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
  *@brief 通过OLED显示日期
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
  *@brief 通过OLED显示RTC介绍
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
  *@brief 通过OLED显示OLED引导
  *@param NULL
  *@retval NULL
  */
void Display_ShowRTCSetInc(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "NowChange:");
	OLED_ShowString(2, 1, "20  -  -  ");
	OLED_ShowString(3, 1, "  :  :   Week:");
	Display_ShowTime();
	Display_ShowDate();
}

/**
  *@brief 通过OLED显示RTC设置
  *@param NULL
  *@retval NULL
  */
void Display_ShowRTCSet(void)
{
	switch(RTCSet_Mode)
	{
		case 1:
			OLED_ShowString(1, 11, "Year");
			break;
		case 2:
			OLED_ShowString(1, 11, "Mon ");
			break;
		case 3:
			OLED_ShowString(1, 11, "Date");
			break;
		case 4:
			OLED_ShowString(1, 11, "Hour");
			break;
		case 5:
			OLED_ShowString(1, 11, "Min ");
			break;
		case 6:
			OLED_ShowString(1, 11, "Sec ");
			break;
		case 7:
			OLED_ShowString(1, 11, "Week");
			break;
	}	
}
	
/**
  *@brief 通过OLED显示主页面
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
  *@brief 通过OLED刷新温湿度数据
  *@param NULL
  *@retval NULL
  */
void Display_ShowDHT(void)
{
	OLED_ShowNum(3, 1, Temph, 2);
	OLED_ShowNum(3, 5, Humih, 2);
}

/**
  *@brief 通过OLED帅新烟雾数据
  *@param NULL
  *@retval NULL
  */
void Display_ShowSmog(void)
{
	OLED_ShowNum(4, 1, Smog, 4);
}

/**
  *@brief 通过OLED刷新光照数据
  *@param NULL
  *@retval NULL
  */
void Display_ShowLight(void)
{
	OLED_ShowNum(4, 9, Light, 2);
}

/**
  *@brief 通过OLED显示密码正确
  *@param NULL
  *@retval NULL
  */
void Display_ShowOK(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Vertify OK");
}

/**
  *@brief 通过OLED显示输入密码界面UI
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
  *@brief 通过OLED实时显示输入的密码
  *@param NULL
  *@retval NULL
  */
void Display_ShowInput(void)
{
	OLED_ShowNum(2, Userword_Index, Userword[Userword_Index - 1], 1);
	OLED_ShowString(2, Userword_Index + 1, "  ");
}

/**
  *@brief 通过OLED显示开锁后的UI
  *@param NULL
  *@retval NULL
  */
void Display_ShowOpen(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Already Open");
	HAL_Delay(500);
	OLED_Clear();
	OLED_ShowString(1, 1, "12 To Control");
	OLED_ShowString(2, 1, "13 To Select");
	OLED_ShowString(3, 1, "14 To Change");
	OLED_ShowString(4, 1, "15 To View");
}

/**
  *@brief 通过OLED显示控制UI
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
  *@brief 通过OLED显示温度阈值设置
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
  *@brief 通过OLED显示温度阈值
  *@param NULL
  *@retval NULL
  */
void Display_ShowTem(void)
{
	OLED_ShowNum(2, 1, TempThrehold, 2);
}

/**
  *@brief 通过OLED显示湿度阈值设置
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
  *@brief 通过OLED显示湿度阈值
  *@param NULL
  *@retval NULL
  */
void Display_ShowHum(void)
{
	OLED_ShowNum(2, 1, HumiThrehold, 2);
}

/**
  *@brief 通过OLED显示烟雾阈值设置
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
  *@brief 通过OLED显示烟雾阈值
  *@param NULL
  *@retval NULL
  */
void Display_ShowSmo(void)
{
	OLED_ShowNum(2, 1, SmogThrehold, 4);
}

/**
  *@brief 通过OLED显示LED亮度设置
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
  *@brief 通过OLED显示LED亮度
  *@param NULL
  *@retval NULL
  */
void Display_ShowLED(void)
{
	OLED_ShowNum(2, 1, Brightness, 3);
}

/**
  *@brief 通过OLED显示风扇温控设置
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
  *@brief 通过OLED显示风扇开启温度
  *@param NULL
  *@retval NULL
  */
void Display_ShowMoOn(void)
{
	OLED_ShowNum(2, 1, Motor_ON_Tem, 2);
}

/**
  *@brief 通过OLED显示风扇最大温度设置
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
  *@brief 通过OLED显示风扇最大温度
  *@param NULL
  *@retval NULL
  */
void Display_ShowMoOff(void)
{
	OLED_ShowNum(2, 1, Motor_MAX_Tem, 2);
}

/**
  *@brief 通过OLED显示设置界面
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
  *@brief 通过OLED显示更改密码界面的UI
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
  *@brief 通过OLED实时显示更改的密码
  *@param NULL
  *@retval NULL
  */
void Display_ShowChaPwd(void)
{
	OLED_ShowNum(2, Password_Index, NewPassWord[Password_Index - 1], 1);
	OLED_ShowString(2, Password_Index + 1, "  ");
}

/**
  *@brief 通过OLED显示引导view的界面
  *@param NULL
  *@retval NULL
  */
void Display_ViewInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Password records");
}

/**
  *@brief 读取历史密码
  *@param NULL
  *@retval -1 历史记录为空
  *         1 成功
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
  *@brief 通过OLED显示历史密码记录 可翻页
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
  *@brief 通过OLED显示引导密码选择的函数
  *@param NULL
  *@retval NULL
  */
void Display_SelInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "14 To Selexct");
}

/**
  *@brief 通过OLED显示密码选择UI 
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
  *@brief 通过OLED显示密码选择失败
  *@param NULL
  *@retval NULL
  */
void Display_ShowSelErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Password Sel Err");
}

/**
  *@brief 通过OLED显示长度错误
  *@param NULL
  *@retval NULL
  */
void Display_ShowLenErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Len Err");
}

/**
  *@brief 通过OLED显示密码错误
  *@param NULL
  *@retval NULL
  */
void Display_ShowPwdErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Pwd Err"); 
}

/**
  *@brief 通过OLED 显示存储密码错误
  *@param NULL
  *@retval NULL
  */
void Display_ShowMemErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Mem Err"); 
}

/**
  *@brief 通过OLED显示更改密码成功
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
  *@brief 通过OLED显示长度错误
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
