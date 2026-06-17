#ifndef __SENSORREAD_H
#define __SENSORREAD_H

/**
  *@brief 传感器数据结构体
  *@param NULL
  *@retval NULL
  */
typedef struct {
	uint8_t Temph; 
	uint8_t Templ; 
	uint8_t Humih; 
	uint8_t Humil; 
	uint8_t Light; 
	float   Smog;  
} SensorData_t;

void SensorRead_Init(void);
void SensorRead_Run(SensorData_t *pData);

#endif
