#ifndef __SERIAL_H
#define __SERIAL_H

void Serial_Init(void);
void Serial_Rx_Callback(uint16_t Size);
void Serial_ProcessCmd(void);

#endif
