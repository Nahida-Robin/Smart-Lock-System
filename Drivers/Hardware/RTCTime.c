/**
 * @file RTCTime.c
 * @brief RTC实时时钟
 * @author Nahida
 * @date 2026.6.6
 */

#include "stm32f4xx.h"                  // Device header

extern RTC_HandleTypeDef hrtc;

/* RTC获取时间，必须先获取时间再获取日期，且获取时间后必须获取日期 */

/**
  *@brief RTC获取当前时间
  *@param hour 当前小时
	*@param minute 当前分钟
	*@param second 当前秒钟
  *@retval NULL
  */
void RTC_GetTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	RTC_TimeTypeDef RTC_Time;
	HAL_RTC_GetTime(&hrtc, &RTC_Time, FORMAT_BIN);
	*hour = RTC_Time.Hours;
	*minute = RTC_Time.Minutes;
	*second = RTC_Time.Seconds;
}

/**
  *@brief RTC获取当前日期
  *@param year 当前年份
	*@param month 当前月份
	*@param weekday 当前星期
	*@param date 当前日期
  *@retval
  */
void RTC_GetDate(uint8_t *year, uint8_t *month, uint8_t *weekday, uint8_t *date)
{
	RTC_DateTypeDef RTC_Date;
	HAL_RTC_GetDate(&hrtc, &RTC_Date, FORMAT_BIN);
	*year = RTC_Date.Year;
	*month = RTC_Date.Month;
	*weekday = RTC_Date.WeekDay;
	*date = RTC_Date.Date;
}

/**
  *@brief RTC设置时间
  *@param hour 小时
	*@param minute 分钟
	*@param second 秒
  *@retval NULL
  */
void RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	RTC_TimeTypeDef sTime = {0};
	sTime.Hours = hour;
	sTime.Minutes = minute;
	sTime.Seconds = second;
	HAL_RTC_SetTime(&hrtc, &sTime, FORMAT_BIN);
}

/**
  *@brief RTC设置日期
  *@param year 年
	*@param month 月
	*@param weekday 星期
	*@param date 日
  *@retval
  */
void RTC_SetDate(uint8_t year, uint8_t month, uint8_t weekday, uint8_t date)
{
	RTC_DateTypeDef sDate = {0};
	sDate.Year = year;
	sDate.Month = month;
	sDate.WeekDay = weekday;
	sDate.Date = date;
	HAL_RTC_SetDate(&hrtc, &sDate, FORMAT_BIN);
}
