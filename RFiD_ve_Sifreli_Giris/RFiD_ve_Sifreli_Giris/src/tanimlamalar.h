#ifndef __TANIMLAMA
#define __TANIMLAMA

#include <Arduino.h>
#include <EEPROM.h>

#define led_kirmizi         A0
#define led_yesil           A1
#define role                9
#define buzzer              12
#define btn_kapi_ac         11
#define btn_kapi_kilitle    10
#define yildiz              10
#define kare                11
#define x1                  3
#define x2                  8
#define x3                  7
#define x4                  5
#define y1                  4
#define y2                  2
#define y3                  6

const   uint8_t sifre_degistirme_kodu[10] = {yildiz, yildiz, yildiz, 6, 5, 4, 3, 2, 1, kare}; // on haneli olmalı ve son hane #(kare) ile bitmeli.
const   uint8_t kullanici_karti_ekleme_kodu[10] = {yildiz, yildiz, yildiz, 1, 2, 3, 1, 2, 3, kare}; // on haneli olmalı ve son hane #(kare) ile bitmeli.
const   uint8_t tum_rfid_kartlari_silme_kodu[10] = {yildiz, yildiz, yildiz, 4, 5, 6, 4, 5, 6, kare}; // on haneli olmalı ve son hane #(kare) ile bitmeli.
const   uint8_t kullanici_karti_silme_kodu[10] = {yildiz, yildiz, yildiz, 7, 8, 9, 7, 8, 9, kare}; // on haneli olmalı ve son hane #(kare) ile bitmeli.
const   int     TUS_TAKIMI_BOSLUK_VERISI = 55;//tuş takımında 55 sayısını boş değer olarak atandı.ÇÜNKÜ TUŞ TAKIMINDA GELEN VERİLER 0,1....11 E KADAR DEĞER ALABİLİYOR.
const   int     BUFFER_SIZE = 14; // RFID DATA FRAME FORMAT: 1byte head (value: 2), 10byte data (2byte version + 8byte tag), 2byte checksum, 1byte tail (value: 3)
const   int     KART_DATA_BOYUTU = 5;// 8 byte tag + 2 byte versiyon numarası  ile toplam
// 10 bayt olan veri checksum işlemleri sayesinde 5 bayt olarak kodlanabiliyor. Bu sayede checksum işlemlerinden sonra
// 10 bayt olan veri 5 bayt olacak şekilde kodlanmış oluyor. Eğer çeksum işlemi doğru sonuç verirse kart numarası 5 bayt
// olarak hafızada saklanacak.
const   int     MAX_RFID_ALANI = 500;// 500/5 = 100 farklı kimlik bilgisini tutar.
const   int     ZAMAN_ASIMI_SURESI = 5000;

#endif
