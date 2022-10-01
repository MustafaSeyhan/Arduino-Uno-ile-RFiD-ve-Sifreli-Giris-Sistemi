#ifndef __UARTISLEMLERI
#define __UARTISLEMLERI
#include "Arduino.h"
#include "tanimlamalar.h"

void    verileri_ekrana_yazdir();
bool    checksum_kontrol();
uint8_t ascii_convert( uint8_t );
void    serialEvent();
void    uart_kesmesi_pasif();
void    uart_kesmesi_aktif();

#endif
