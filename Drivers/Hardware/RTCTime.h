#ifndef __RTCTIME_H
#define __RTCTIME_H

void RTC_GetTime(uint8_t *hour, uint8_t *minute, uint8_t *second);
void RTC_GetDate(uint8_t *year, uint8_t *month, uint8_t *weekday, uint8_t *date);
void RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second);
void RTC_SetDate(uint8_t year, uint8_t month, uint8_t weekday, uint8_t date);

#endif
