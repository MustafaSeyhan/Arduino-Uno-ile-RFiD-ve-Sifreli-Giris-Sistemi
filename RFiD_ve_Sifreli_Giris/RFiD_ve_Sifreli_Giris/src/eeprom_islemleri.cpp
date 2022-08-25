/*
* Fabrika çıkışı eeprom hafızanın dataları 0xFF dolu olarak geliyor.

* her bir rfid kart datası 5 bayt olarak saklanacak. max 200 kart saklama hafızası için yer ayrılacak.
index baytı hafızada kaç kullanıcı olduğunu tutacak, bu bayt eprom hafızanın sondan ikinci baytı olacak.
eeprom hafızanın son baytı arıza ihtimaline karşı kullanılmayacak.

* 0 - 999 aralığındaki adresler sırasıyla beşerli olarak her bir kartın verisini tutacak. 

1000 - 1004 aralığındaki 5 bayt kapı şifresini hafızada tutmak için ayrıldı.
*/
#include "eeprom_islemleri.h"
#include "buzzer_islemleri.h"

extern  volatile    uint8_t     yeni_sifre_ilk_giris[5];
extern  volatile    uint8_t     eeprom_index;
extern  volatile    uint8_t     kullanici_rfid_datalari[MAX_RFID_ALANI];
extern  volatile    uint8_t     kullanici_sifresi[5];
extern              char        rfid_kimlik_degeri[KART_DATA_BOYUTU];


//***********************************************************************************
//***********************************************************************************
void eeprom_verilerini_yukle(){
//1022 adresli eeprom alanı indexleme içn kullanılacak.
    eeprom_index = EEPROM.read(INDEX_ADRESS);
    if(eeprom_index >= 201 ) //eeprom hafıza fabrika çıkışı 0xff yüklü olarak gelir.
    {
        eeprom_index = 0 ; // daha önce hiç kayıt yapılmamış.
        EEPROM.write(INDEX_ADRESS,0);
        Serial.println("kayıtlı kullanıcı yok");
    }
    else{
        for(uint16_t i = 0 ; i < ( eeprom_index * 5 ) ; i++)
    kullanici_rfid_datalari[i]=EEPROM.read(i);
    }

    kullanici_sifresi[0] =  EEPROM.read(1000);
    kullanici_sifresi[1] =  EEPROM.read(1001);
    kullanici_sifresi[2] =  EEPROM.read(1002);
    kullanici_sifresi[3] =  EEPROM.read(1003);
    kullanici_sifresi[4] =  EEPROM.read(1004);

    if(kullanici_sifresi[0] == 0xFF || kullanici_sifresi[1] == 0xFF || kullanici_sifresi[2] == 0xFF 
    || kullanici_sifresi[3] == 0xFF || kullanici_sifresi[4] == 0xFF )
    {
        kullanici_sifresi[0]=1;
        kullanici_sifresi[1]=2;
        kullanici_sifresi[2]=3;
        kullanici_sifresi[3]=4;
        kullanici_sifresi[4]=5;
        EEPROM.write(1000,1);
        EEPROM.write(1001,2);
        EEPROM.write(1002,3);
        EEPROM.write(1003,4);
        EEPROM.write(1004,5);
    }
    Serial.print("kullanıcı şifresi : ");
    Serial.print(kullanici_sifresi[0]); Serial.print(" "); 
    Serial.print(kullanici_sifresi[1]); Serial.print(" ");
    Serial.print(kullanici_sifresi[2]); Serial.print(" ");
    Serial.print(kullanici_sifresi[3]); Serial.print(" ");
    Serial.println(kullanici_sifresi[4]);
}
//***********************************************************************************
//***********************************************************************************
void eeproma_yeni_kullanici_kaydet(){
    if(eeprom_index<199)
    {//diyelim ki önceden 3 kaydımız varmış. şimdi yeni bir kayıt eklenmek isteniyor. bu durumda index bir artar 3 olur.
        eeprom_index++;
        EEPROM.write(INDEX_ADRESS,eeprom_index);// yeni kayıt için index 1 arttı ve hafızaya yazıldı.
        EEPROM.write( ((eeprom_index*5)-5) , rfid_kimlik_degeri[0] ); // yeni kaydın ilk baytı hafızanın 10 numaralı adresine yerleşir.
        EEPROM.write( ((eeprom_index*5)-4) , rfid_kimlik_degeri[1] );// bu da 3*5=15 -> 15-5 = 10 ile sağlanır. her bayt için adres
        EEPROM.write( ((eeprom_index*5)-3) , rfid_kimlik_degeri[2] );// birer defa artar.
        EEPROM.write( ((eeprom_index*5)-2) , rfid_kimlik_degeri[3] );
        EEPROM.write( ((eeprom_index*5)-1) , rfid_kimlik_degeri[4] );// 3. datanın(indexin) son baytı hafıdaza 14 numaralı adrese yerleşir.      
    }
    else
    {
       hata_biplemesi_eeprom_hafiza_doldu();  //hafızada yer kalmamış biplemesi.
       return;   
    }
}
//***********************************************************************************
//***********************************************************************************
void eepromu_tamamen_sil(){
    for (uint16_t i=0 ; i<=999; i++)
    {
        EEPROM.write(i,0xff);
    }
    EEPROM.write(INDEX_ADRESS,0xff);
    /*
    1000 ile 1021 aralığındaki adreslerde şifre vs. bilgiler tutulacak. o adresler ellenmiyor.
    */
}

//***********************************************************************************
//***********************************************************************************
void eepromdan_bir_kullanici_sil(){
    //burası biraz karışık. tuş takımını halledince bakalım. 
}

//***********************************************************************************
//***********************************************************************************
void eeproma_yeni_sifreyi_kaydet(){
    EEPROM.write(1000 , yeni_sifre_ilk_giris[0]);
    EEPROM.write(1001 , yeni_sifre_ilk_giris[1]);
    EEPROM.write(1002 , yeni_sifre_ilk_giris[2]);
    EEPROM.write(1003 , yeni_sifre_ilk_giris[3]);
    EEPROM.write(1004 , yeni_sifre_ilk_giris[4]);
    
    kullanici_sifresi[0] =  yeni_sifre_ilk_giris[0];
    kullanici_sifresi[1] =  yeni_sifre_ilk_giris[1];
    kullanici_sifresi[2] =  yeni_sifre_ilk_giris[2];
    kullanici_sifresi[3] =  yeni_sifre_ilk_giris[3];
    kullanici_sifresi[4] =  yeni_sifre_ilk_giris[4];
}