#include "tustakimi.h"
#include "buzzer_islemleri.h"

extern const    uint8_t         sifre_degistirme_kodu[10];
extern volatile uint8_t         basilan_tus;
extern volatile uint8_t         tustakimi_verisi[10];
extern volatile bool            zaman_asimi_sayaci_kullanilsinmi;
extern volatile unsigned long   eskizaman;

//************************************************************************
//************************************************************************
void tustakimi_ara_islemleri(void)
{
    eskizaman = millis();
    zaman_asimi_sayaci_kullanilsinmi=true;
    digitalWrite(buzzer,1);
} 
//************************************************************************
//************************************************************************
void tus_takimi_verilerini_bosalt(void)
{
    for (int i = 0; i < 10; i++)
    {
    tustakimi_verisi[i] = TUS_TAKIMI_BOSLUK_VERISI ; 
    basilan_tus = TUS_TAKIMI_BOSLUK_VERISI;
    }           
}
//************************************************************************
//************************************************************************
bool tustakimi_kontrol(){
/*

* Bu fonksiyon her çağrıldığında tüm butonları bir defa test eder. eğer bir butona basılırsa
basılan butonun sayı değeri "basilan_tus" kayıtçısınıa atanır. Eğer bir tuşa basılmışsa fonksiyon
sonunda basılan son 10 tuşun verisini hafızasında tutan tustakimi_verisi[] dizisinin elemanları 
önce 8. değişkenin değeri 9. 'ya kopyolanır,
     7. değişkenin değeri 8. 'ye koypalanır
     .
     .
     .
     0. değişkenin değeri 1. 'ye kopyalanır 
en sonunda ise basilan_tus kayıtçısındaki en son basılan tuşun değeri dizinin 0. elemanına yazılır.
Bu sayedek basılan her tuşta en yeni basılan 0. değişkene yazılır ve kaydırma yapılır.
Tuş değerleri
0 -> 0          1 -> 1          2 -> 2          3 -> 3          4 -> 4
5 -> 5          6 -> 6          7 -> 7          8 -> 8          9 -> 9
* -> 10         # -> 11

* fonsiyon dönüşünde ise eğer bir tuşa basılmışsa true döndürülür.

*/    
    basilan_tus =55;
    digitalWrite(x1,0);
    digitalWrite(x2,1);
    digitalWrite(x3,1);
    digitalWrite(x4,1); 
    if(digitalRead(y1)==0)
    {
    tustakimi_ara_islemleri();    
    while (digitalRead(y1)==0)
    {
       delay(1);
    }
    delay(5);
    basilan_tus=1;
    digitalWrite(buzzer,0);
    }
                    else if(digitalRead(y2)==0)
                    {
                    tustakimi_ara_islemleri();
                    while (digitalRead(y2)==0)
                    {
                        delay(1);
                    }
                    delay(5);
                    basilan_tus=2;
                    digitalWrite(buzzer,0);
                    }
                                else if(digitalRead(y3)==0)
                                    {
                                    tustakimi_ara_islemleri();
                                    while (digitalRead(y3)==0)
                                    {
                                        delay(1);
                                    }
                                    delay(5);
                                    basilan_tus=3;
                                    digitalWrite(buzzer,0);
                                    }
//---------------------------------------------------------------
    digitalWrite(x1,1);
    digitalWrite(x2,0);
    if(digitalRead(y1)==0)
    {
    tustakimi_ara_islemleri();
    while (digitalRead(y1)==0)
    {
        delay(1);
    }
    delay(5);
    basilan_tus=4;
    digitalWrite(buzzer,0);
      }
                    else if(digitalRead(y2)==0)
                    {
                    tustakimi_ara_islemleri();
                    while (digitalRead(y2)==0)
                    {
                        delay(1);
                    }
                    delay(5);
                    basilan_tus=5;
                    digitalWrite(buzzer,0);
                    }
                                else if(digitalRead(y3)==0)
                                    {
                                    tustakimi_ara_islemleri();
                                    while (digitalRead(y3)==0)
                                    {
                                        delay(1);
                                    }
                                    delay(5);
                                    basilan_tus=6;
                                    digitalWrite(buzzer,0);                      
                                    }   
//---------------------------------------------------------------
    digitalWrite(x2,1);
    digitalWrite(x3,0);
    if(digitalRead(y1)==0)
    {
    tustakimi_ara_islemleri();
    while (digitalRead(y1)==0)
    {
        delay(1);
    }
    delay(5);
    basilan_tus=7;
    digitalWrite(buzzer,0);
      }
                    else if(digitalRead(y2)==0)
                    {
                    tustakimi_ara_islemleri();
                    while (digitalRead(y2)==0)
                    {
                        delay(1);
                    }
                    delay(5);
                    basilan_tus=8;
                    digitalWrite(buzzer,0);
                    }
                                else if(digitalRead(y3)==0)
                                    {
                                    tustakimi_ara_islemleri();
                                    while (digitalRead(y3)==0)
                                    {
                                        delay(1);
                                    }
                                    delay(5);
                                    basilan_tus=9;
                                    digitalWrite(buzzer,0);
                                    }  
//---------------------------------------------------------------
    digitalWrite(x3,1);
    digitalWrite(x4,0);
    if(digitalRead(y1)==0)
    {
    tustakimi_ara_islemleri();
    while (digitalRead(y1)==0)
    {
        delay(1);
    }
    delay(5);
    basilan_tus=yildiz;
    digitalWrite(buzzer,0);
      }
                    else if(digitalRead(y2)==0)
                    {
                    tustakimi_ara_islemleri();
                    while (digitalRead(y2)==0)
                    {
                        delay(1);
                    }
                    delay(5);
                    basilan_tus=0;
                    digitalWrite(buzzer,0);
                    }
                                else if(digitalRead(y3)==0)
                                    {
                                    tustakimi_ara_islemleri();
                                    while (digitalRead(y3)==0)
                                    {
                                        delay(1);
                                    }
                                    delay(5);
                                    basilan_tus=kare;
                                    digitalWrite(buzzer,0);
                                    } 
//---------------------------------------------------------------                                    
        if( basilan_tus < 12 && basilan_tus != TUS_TAKIMI_BOSLUK_VERISI)
        {
            for (int i = 9; i > 0; i--)
            {
            tustakimi_verisi[i] = tustakimi_verisi[i-1]; 
            }
            tustakimi_verisi[0] = basilan_tus;            
            return true;
        }
        return false;
}
