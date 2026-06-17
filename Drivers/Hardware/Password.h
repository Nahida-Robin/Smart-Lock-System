#ifndef __PASSWORD_H
#define __PASSWORD_H

int8_t Password_Vertify(uint8_t Key);
int8_t Password_Change(uint8_t Key);
uint8_t Open_Error(void);
int8_t Password_Select(uint8_t Key);
//int8_t Read_Memory(uint8_t *userwordmemory, uint16_t userwordmemory_size, uint16_t *userwordmemory_len,
//									 uint8_t *timememory, uint16_t timememory_size, uint16_t *timememory_len);

#endif
