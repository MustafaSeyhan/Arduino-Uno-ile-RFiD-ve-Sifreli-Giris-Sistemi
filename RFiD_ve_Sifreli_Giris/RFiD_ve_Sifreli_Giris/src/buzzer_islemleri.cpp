#include "buzzer_islemleri.h"

//*****************************************************************************
//*****************************************************************************
void onay_biplemesi(){
    for(int i=0 ; i<3; i++)
    {
    digitalWrite(buzzer,HIGH);
    delay(75);
    digitalWrite(buzzer,LOW);
    delay(40);
    }
}
//*****************************************************************************
//*****************************************************************************
void hata_biplemesi()
{
    uint8_t leddurumukirmizi=0;
    uint8_t leddurmuyesil=0;
    leddurumukirmizi=digitalRead(led_kirmizi);
    leddurmuyesil = digitalRead(led_yesil);
    digitalWrite(led_yesil,0);
    for(uint16_t i=0 ;i<500 ; i++)
    {
    digitalWrite(buzzer,HIGH);
    delay(1);    
    digitalWrite(buzzer,LOW);
    delay(1);
    if(i%20 == 0) 
    {
        digitalWrite(led_kirmizi, !digitalRead(led_kirmizi));
    }
    }
    digitalWrite(led_kirmizi,leddurumukirmizi);
    digitalWrite(led_yesil,leddurmuyesil);
}
//*****************************************************************************
//*****************************************************************************
void acilis_biplemesi()
{
    for(uint16_t i = 75 ; i<=300 ; i=i*2)
    {
    digitalWrite(buzzer,HIGH);
    delay(i);
    digitalWrite(buzzer,LOW);
    delay(50); 
    }
}
//*****************************************************************************
//*****************************************************************************
void hata_biplemesi_eeprom_hafiza_doldu(){
    for(int i = 0 ; i<4 ; i++){
    digitalWrite(buzzer,HIGH);
    delay(100);
    digitalWrite(buzzer,LOW);
    delay(50);
    }
} 