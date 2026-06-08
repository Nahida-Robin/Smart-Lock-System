#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

void BlueTooth_Init(void);
void BlueTooth_Rx_Callback(uint16_t Size);
void BlueTooth_ProcessCmd(void);

#endif
