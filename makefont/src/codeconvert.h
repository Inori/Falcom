//
// Created by asuka on 10/29/17.
//

#ifndef CODECONVERT_H
#define CODECONVERT_H

#include <stdint.h>

int cc_init();


int cc_utf16_to_utf8(uint16_t* u16_buff, uint32_t u16_size,
                  uint8_t* u8_buff, uint32_t u8_size);


int cc_utf8_to_utf16(uint8_t* u8_buff, uint32_t u8_size,
                     uint16_t* u16_buff, uint32_t u16_size);


void cc_unit();


#endif //CODECONVERT_H
