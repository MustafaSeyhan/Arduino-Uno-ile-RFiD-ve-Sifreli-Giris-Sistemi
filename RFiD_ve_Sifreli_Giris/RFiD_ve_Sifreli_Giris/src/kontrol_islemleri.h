#ifndef __KONTROLISLEMLERI
#define __KONTROLISLEMLERI
#include "Arduino.h"
#include "tanimlamalar.h"
#include "tustakimi.h"
#include "buzzer_islemleri.h"
#include "eeprom_islemleri.h"
#include "uart_islemleri.h"
#include "tustakimi.h"


bool    sifre_kontrol(void);
bool    sifre_degistirme_kodu_kontrol(void);
bool    kullanici_karti_silme_kodu_kontrol(void);
bool    kullanici_karti_ekleme_kodu_kontrol(void);
bool    tum_rfid_kayitlarini_silme_kodu_kontrol();
void    kontrol_verilerini_temizle();
void    rfid_veri_kontrolu_yap();
void    zaman_asimi_kontrolu_yap();
void    kapi_kapatma_buton_kontrolu_yap();
void    kapi_acma_buton_kontrolu_yap();
void    sifre_degistirme_islemleri_kontrolu_yap();
void    kapi_ac();
void    sifreli_giris_istegi_kontrolu_yap();
void    kart_ekleme_istegi_kontrolu_yap();
bool    rfid_kart_kontrolu_yap();
void    kart_silme_istegi_kontrolu_yap();
void    tum_rfid_kartlarin_silinmesi_istegi_kontrolu_yap();

#endif
