#ifndef __FLASH_H
#define __FLASH_H

uint16_t Flash_ReadID(void);
uint8_t Flash_ReadSR(void);
void Flash_WriteSR(uint8_t sr);
void Flash_WaitBusy(void);
void Flash_PowerDown(void);
void Flash_PowerOn(void);
void Flash_EnableWrite(void);
void Flash_DisableWrite(void);
void Flash_Read(uint8_t *buf, uint32_t addr, uint16_t size);
void Flash_WritePage(uint8_t *buf, uint32_t addr, uint16_t size);
void Flash_Write_NoCheck(uint8_t *buf, uint32_t addr, uint16_t size);
void Flash_EraseChip(void);
void Flash_EraseSector(uint16_t sec_addr);
void Flash_Write(uint8_t *buf, uint32_t addr, uint16_t size);

#endif
