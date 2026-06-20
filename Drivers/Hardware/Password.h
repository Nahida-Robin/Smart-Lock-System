#ifndef __PASSWORD_H
#define __PASSWORD_H

int8_t Password_Vertify(void);
int8_t Password_Change(void);
uint8_t Open_Error(void); 
int8_t Password_Select(void);
void PW_Select(uint8_t sel);
//int8_t Read_Memory(uint8_t *userwordmemory, uint16_t userwordmemory_size, uint16_t *userwordmemory_len,
//									 uint8_t *timememory, uint16_t timememory_size, uint16_t *timememory_len);

#endif
