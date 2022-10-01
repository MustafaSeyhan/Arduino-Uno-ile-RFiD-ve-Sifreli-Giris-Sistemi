/*
* Fabrika çıkışı eeprom hafızanın dataları 0xFF dolu olarak geliyor.

* her bir rfid kart datası 5 bayt olarak saklanacak.  100 kart için hafızadan yer ayrıldı.
kayitli_kart_sayisi baytı hafızada kaç kullanıcı olduğunu tutacak, 

* 0 - 999 aralığındaki adresler sırasıyla beşerli olarak her bir kartın verisini tutacak. 

1000 - 1004 aralığındaki 5 bayt kapı şifresini hafızada tutmak için ayrıldı.
*/
#include "eeprom_islemleri.h"
#include "buzzer_islemleri.h"

extern  volatile    uint8_t     yeni_sifre_ilk_giris[];
extern  volatile    uint8_t     kayitli_kart_sayisi;
extern  volatile    uint8_t     kayitli_rfid_datalari[MAX_RFID_ALANI];
extern  volatile    uint8_t     kullanici_sifresi[];
extern              uint8_t     okunan_rfid_kartin_kimlik_degeri[KART_DATA_BOYUTU];


void eeprom_ara_islemleri_verileri_temizle()
{
    /*
    fabrika çıkışı eeprom hafızalar 0xFF ile dolu olarak geldiğinden silme işleminde de hafıza alanları bu sayıyla dolduruluyor.
    */
    okunan_rfid_kartin_kimlik_degeri[0]=255;
    okunan_rfid_kartin_kimlik_degeri[1]=255;
    okunan_rfid_kartin_kimlik_degeri[2]=255;
    okunan_rfid_kartin_kimlik_degeri[3]=255;
    okunan_rfid_kartin_kimlik_degeri[4]=255;
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void eeprom_datalari_yukle(bool yazilar_print_edilsinmi){
//--------------------------------------------------------------------------------------------------
/*
    bu kısımda daha önceden kaydedilmiş RFid kartların bilgileri eepromdan çekiliyor.
    bir rfid kart yazılım algoritması ile 5 baytlık veriler şeklinde saklanıyor. dolayısıyla 100 rf ide kart için 
    5x100 = beşyüz bayta ihtiyaç var. MAX_RFID_ALANI sabiti de bu değeri saklıyor.
*/
    kayitli_kart_sayisi=0;

        for(uint16_t i=0; i<MAX_RFID_ALANI; i=i+5)
        {
            /*
            her beş bayt bir kart için id bilgisini tutar.
            */
        kayitli_rfid_datalari[i]  =EEPROM.read(i);
        kayitli_rfid_datalari[i+1]=EEPROM.read(i+1);
        kayitli_rfid_datalari[i+2]=EEPROM.read(i+2);
        kayitli_rfid_datalari[i+3]=EEPROM.read(i+3);
        kayitli_rfid_datalari[i+4]=EEPROM.read(i+4);
        if( kayitli_rfid_datalari[i]!=255 && kayitli_rfid_datalari[i+1]!=255 && kayitli_rfid_datalari[i+2]!=255 
            && kayitli_rfid_datalari[i+3]!=255 && kayitli_rfid_datalari[i+4]!=255 )
            {
                kayitli_kart_sayisi++;            
                }                     
        }       
    
    if( kayitli_kart_sayisi > 0 && yazilar_print_edilsinmi == true)
        {
      //  Serial.print(kayitli_kart_sayisi);
      //  Serial.println(" Adet Kartın ID Bilgileri Yüklendi.");                            
        }
        else if( kayitli_kart_sayisi == 0 && yazilar_print_edilsinmi == true)
            {
        //    Serial.println("Hafızada Kayıtlı Kart Yok.");
            }

//--------------------------------------------------------------------------------------------------
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
    
  /*  if(yazilar_print_edilsinmi)
    {
    Serial.flush();
    Serial.print("KULLANICI ŞİFRESİ : ");
    Serial.print(kullanici_sifresi[0]); Serial.print(" "); 
    Serial.print(kullanici_sifresi[1]); Serial.print(" ");
    Serial.print(kullanici_sifresi[2]); Serial.print(" ");
    Serial.print(kullanici_sifresi[3]); Serial.print(" ");
    Serial.println(kullanici_sifresi[4]);
    } */

}
//***********************************************************************************************************************
//***********************************************************************************************************************
void eeproma_yeni_kullanici_kaydet(){
//--------------------------------------------------------------------------------------------------
/*
*  bu döngüde hafızada yeni kartı kaydetmek için boş alan varmı diye kontrol ediliyor.
*/  uint16_t kaydedilecek_eeprom_alaninin_baslangic_adresi=0;
    bool bos_eeprom_alani_varmi=false;
    for( uint16_t i=0 ; i < MAX_RFID_ALANI ; i=i+5 )//toplam kaydedilebilecek kart sayısı = MAX_RFID_ALANI/5 her kart datası 5 bayt ile saklanır.
    {
       if(  EEPROM.read(i)==255 && EEPROM.read(i+1)==255 && EEPROM.read(i+2)==255 && EEPROM.read(i+3)==255 &&
            EEPROM.read(i+4)==255)
       {
        bos_eeprom_alani_varmi=true;
        kaydedilecek_eeprom_alaninin_baslangic_adresi = i ;
        break;
       }
    }    
//--------------------------------------------------------------------------------------------------       
/*
*   bu döngüde kaydedilmek istenen kart daha önceden kaydedilmiş mi diye kontrol ediliyor.
*   eğer daha önce kaydedilmişse bu kart için yeni bir alan tahsis edilmeyecek ancak
*   kullanıcının kafası karışmasın diye kaydedilmiş gibi onaylama sesi çıkartılacak.
*/ 
    bool kart_daha_once_kaydedilmismi = false;
    for (uint16_t i =0 ; i<MAX_RFID_ALANI ; i=i+5)
    {
        if( kayitli_rfid_datalari[i]   == okunan_rfid_kartin_kimlik_degeri[0] && 
            kayitli_rfid_datalari[i+1] == okunan_rfid_kartin_kimlik_degeri[1] &&
            kayitli_rfid_datalari[i+2] == okunan_rfid_kartin_kimlik_degeri[2] &&
            kayitli_rfid_datalari[i+3] == okunan_rfid_kartin_kimlik_degeri[3] &&
            kayitli_rfid_datalari[i+4] == okunan_rfid_kartin_kimlik_degeri[4]  )
            {
                kart_daha_once_kaydedilmismi=true;
             //   Serial.println("Bu Kart Daha Önce Kaydedilmiş");
                break;
            }                
    }
//--------------------------------------------------------------------------------------------------        

    if(bos_eeprom_alani_varmi == true )
    {
        if(kart_daha_once_kaydedilmismi == false )
        {
        /*
        hafızada boş alan var ise ve kaydedilmek istenen kart daha önceden kaydedilmemişse boş alanın başlangıç adresinin tutulduğu 
        "kaydedilecek_eeprom_alaninin_baslangic_adresi" değişkeninin gösterdiği adresten itibaren veriler eeproma yazılır.
        */
        EEPROM.write(  kaydedilecek_eeprom_alaninin_baslangic_adresi    , okunan_rfid_kartin_kimlik_degeri[0] ); // yeni kaydın ilk baytı hafızanın 10 numaralı adresine yerleşir.
        EEPROM.write( (kaydedilecek_eeprom_alaninin_baslangic_adresi+1) , okunan_rfid_kartin_kimlik_degeri[1] );// bu da 3*5=15 -> 15-5 = 10 ile sağlanır. her bayt için adres
        EEPROM.write( (kaydedilecek_eeprom_alaninin_baslangic_adresi+2) , okunan_rfid_kartin_kimlik_degeri[2] );// birer defa artar.
        EEPROM.write( (kaydedilecek_eeprom_alaninin_baslangic_adresi+3) , okunan_rfid_kartin_kimlik_degeri[3] );
        EEPROM.write( (kaydedilecek_eeprom_alaninin_baslangic_adresi+4) , okunan_rfid_kartin_kimlik_degeri[4] );// 3. datanın(indexin) son baytı hafıdaza 14 numaralı adrese yerleşir.              
     //   Serial.println("Kart Hafızaya Kaydedildi.");
        eeprom_datalari_yukle(false);
        onay_biplemesi();
        }
        else
            {
           // Serial.println("Boş hafıza alanı var ama kart daha önce kayıtlı olduğu için tekrar kaydedilmedi.");
            onay_biplemesi();
            }
    }
    else 
        {
          //  Serial.println("Hafıza Dolu, Kayıt Yapılamadı.");
            hata_biplemesi_eeprom_hafiza_doldu();
        }
    eeprom_ara_islemleri_verileri_temizle();
    delay(1500);//kullanıcı kartı kaydeder kaydetmez kapının açılmasını engelliyor.
    bos_eeprom_alani_varmi=false;
    kart_daha_once_kaydedilmismi=false;
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void eepromu_tamamen_sil(){
    for (uint16_t i=0 ; i<MAX_RFID_ALANI; i++)
    {
        EEPROM.write(i,0xff);
        kayitli_rfid_datalari[i]=0;
    }
    /*
    tanimlamalar.h dosyasında belirtilen MAX_RFID_ALANI değeri rifd kullanıcı sayısı x 5 bayt tan gelen sayıdır.
    çünkü her rfid kartının datası 5 bayt veri ile saklanır.
    1000 ile 1021 aralığındaki adreslerde şifre vs. bilgiler tutulacak. o adresler ellenmiyor.
    */
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void eepromdan_bir_kullanici_sil(){
    bool bu_kart_hafizada_kayitlimi = false;
    uint16_t silinecek_kartin_hafiza_baslangic_adresi=0;
    for (uint16_t i =0 ; i<MAX_RFID_ALANI ; i=i+5)
    {
        /*
        bu for döngüsünün içinde silinmek istenen kartın daha önceden hafızaya kayıtlı olup
        olmadığı kontrol ediliyor. Eğer beşin katı herhangi bir adresten itibaren eşleşen 
        beş adet data olursa bu bir kartın id numarası demektir. Bu durudma false olan
        bu_kart_hafizada_kayitlimi değişkeni true yapılarak kartın kayıtlı olduğu bilgisi kodlanır.
        */
        if( kayitli_rfid_datalari[i]   == okunan_rfid_kartin_kimlik_degeri[0] && 
            kayitli_rfid_datalari[i+1] == okunan_rfid_kartin_kimlik_degeri[1] &&
            kayitli_rfid_datalari[i+2] == okunan_rfid_kartin_kimlik_degeri[2] &&
            kayitli_rfid_datalari[i+3] == okunan_rfid_kartin_kimlik_degeri[3] &&
            kayitli_rfid_datalari[i+4] == okunan_rfid_kartin_kimlik_degeri[4]  )
            {
                bu_kart_hafizada_kayitlimi=true;
                silinecek_kartin_hafiza_baslangic_adresi = i;
                break;
            }                
    }
    
    if(bu_kart_hafizada_kayitlimi == true)
    {
        /*
        Silinmek istenen kart hafızada mevcutmuş. Burada silinmesi istenen kartın 5 baytlık id sinin ilk baytının adrsi
        "silinecek_kartin_hafiza_baslangic_adresi" değişkeni tutulur. Ve bu değişkenin gösterdiği adresten itibaren
        beş bayt veri silinir(silme işlemi 255(0xFF) ile doldurmaktır).
        */
        EEPROM.write(silinecek_kartin_hafiza_baslangic_adresi , 255);
        EEPROM.write(silinecek_kartin_hafiza_baslangic_adresi+1 , 255);
        EEPROM.write(silinecek_kartin_hafiza_baslangic_adresi+2 , 255);
        EEPROM.write(silinecek_kartin_hafiza_baslangic_adresi+3 , 255);
        EEPROM.write(silinecek_kartin_hafiza_baslangic_adresi+4 , 255);
        bu_kart_hafizada_kayitlimi=false;
        eeprom_datalari_yukle(false);
        onay_biplemesi();
       // Serial.println("Kart silindi.");
    }
    else
    {
        // kart hafızada kayıtlı değilmiş, işlem yapılmaz.
       // Serial.println("Bu kart hafızada kayıtlı değil.");
        hata_biplemesi();
    }
    eeprom_ara_islemleri_verileri_temizle();
    delay(1500);
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void eeproma_yeni_sifreyi_kaydet(){
    /*
    tuş takımında girilen şifrenin değiştirilmesi işleminde verilerin eeprom hafızaya kaydedildiği program bloğu.
    */
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