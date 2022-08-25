#ifndef __TANIMLAMA
#define __TANIMLAMA

#include <EEPROM.h>

#define role 9
#define buzzer 12
#define btn_kapi_ac 11
#define btn_kapi_kilitle 10
#define led_kirmizi A0
#define led_yesil A1
#define yildiz 10
#define kare 11

#define x1 3
#define x2 8
#define x3 7
#define x4 5
#define y1 4
#define y2 2
#define y3 6

const uint8_t sifre_degistirme_kodu[10]={yildiz,yildiz,yildiz,6,5,4,3,2,1,kare};// on haneli olmalı ve son hane #(kare) ile bitmeli.
const int TUS_TAKIMI_BOSLUK_VERISI = 55;//tuş takımında 55 sayısını boş değer olarak atandı.ÇÜNKÜ TUŞ TAKIMINDA GELEN VERİLER 0,1....11 E KADAR DEĞER ALABİLİYOR.
const int BUFFER_SIZE = 14; // RFID DATA FRAME FORMAT: 1byte head (value: 2), 10byte data (2byte version + 8byte tag), 2byte checksum, 1byte tail (value: 3)
const int DATA_SIZE = 10; // 10byte data (2byte version + 8byte tag)
const int DATA_VERSION_SIZE = 2; // 2byte version (actual meaning of these two bytes may vary)
const int DATA_TAG_SIZE = 8; // 8byte tag
const int CHECKSUM_SIZE = 2; // 2byte checksum
const int KART_DATA_BOYUTU = 5;// 8 byte tag + 2 byte versiyon numarası  ile toplam 
// 10 bayt olan veri checksum işlemleri sayesinde 5 bayt olarak kodlanabiliyor. Bu sayede checksum işlemlerinden sonra 
// 10 bayt olan veri 5 bayt olacak şekilde kodlanmış oluyor. Eğer çeksum işlemi doğru sonuç verirse kart numarası 5 bayt 
// olarak hafızada saklanacak.
const int MAX_RFID_ALANI = 1000;
const int INDEX_ADRESS = 1022;// RFID kullanıcı sayısının değeri bu eeprom adresinde saklanacak.



#endif