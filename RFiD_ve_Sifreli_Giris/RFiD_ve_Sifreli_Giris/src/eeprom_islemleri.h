#ifndef __EEPROMISLEMLERI
#define __EEPROMISLEMLERI
#include "Arduino.h"
#include "tanimlamalar.h"

void eeprom_datalari_yukle(bool);
void eeproma_yeni_kullanici_kaydet();
void eepromu_tamamen_sil();
void eepromdan_bir_kullanici_sil();
void eeproma_yeni_sifreyi_kaydet(void);
void eeprom_ara_islemleri_verileri_temizle();


#endif
