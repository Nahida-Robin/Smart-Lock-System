/** 
 * @file dht11.h
 * @brief dht11温湿度模块驱动
 * @author Nahida
 * @date 2026-5-16
 */
#ifndef __DHT11_H
#define __DHT11_H


void DHT_Read(uint8_t *temph, uint8_t *templ, uint8_t *humih, uint8_t *humil);
    
#endif
