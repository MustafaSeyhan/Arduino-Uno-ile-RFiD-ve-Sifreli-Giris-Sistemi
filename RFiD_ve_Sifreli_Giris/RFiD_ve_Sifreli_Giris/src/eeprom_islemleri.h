#ifndef __EEPROMISLEMLERI
#define __EEPROMISLEMLERI
#include <Arduino.h>
#include <tanimlamalar.h>

void eeprom_verilerini_yukle();
void eeproma_yeni_kullanici_kaydet();
void eepromu_tamamen_sil();
void eepromdan_bir_kullanici_sil();
void eeproma_yeni_sifreyi_kaydet(void);

#endif