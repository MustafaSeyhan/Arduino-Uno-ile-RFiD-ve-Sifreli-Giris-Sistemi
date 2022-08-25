
#include <Arduino.h>

#include <tanimlamalar.h>
#include <uart_islemleri.h>
#include <buzzer_islemleri.h>
#include <eeprom_islemleri.h>
#include <tustakimi.h>

volatile  bool            yeni_sifreler_eslesiyormu=false;
volatile  bool            tum_sayilar_girilmismi=false;
volatile  bool            yeni_sifre_ilk_defa_girildimi=false;
volatile  bool            sifre_degistirme_islemi_basladimi=false;
volatile  uint8_t         basilan_tus=TUS_TAKIMI_BOSLUK_VERISI;
volatile  bool            tusa_basildimi=false;
volatile  bool            tustakimi_sayaci_kullanilsinmi=false;
volatile  uint8_t         tustakimi_verisi[10];
volatile  uint8_t         kullanici_sifresi[5];
volatile  uint8_t         yeni_sifre_ilk_giris[5];
volatile  uint8_t         yeni_sifre_ikinci_giris[5];
volatile  unsigned long   eskizaman=0;

volatile  bool            rfid_veri_geldimi = false;  // whether the string is complete
volatile  bool            veri_alimi_hazirmi= false;
volatile  bool            kapi_kilitlimi = true; // kullanıcı tarafından şifre girilip kapı kilitlenmiş ise TRUE dir.

volatile  uint8_t         eeprom_index=0;
volatile  uint8_t         kullanici_rfid_datalari[MAX_RFID_ALANI];
volatile  uint8_t         checksum_degeri = 0;
          char            rfid_buffer[BUFFER_SIZE];
          char            rfid_kimlik_degeri[KART_DATA_BOYUTU] ;

          uint8_t         checksum_hesapla();
          uint8_t         ascii_convert( uint8_t );


//*****************************************************************************
//*****************************************************************************
void kapiyi_ac()
{
      digitalWrite(led_kirmizi,0);
      digitalWrite(role,1);  
      for( int i = 0 ;  i < 7; i++)
      {
        digitalWrite(led_yesil,0);
        digitalWrite(buzzer,1);
        delay(40);
        digitalWrite(led_yesil,1);
        digitalWrite(buzzer,0);
        delay(40);        
      }
      digitalWrite(role,0);
}
//*****************************************************************************
//*****************************************************************************
void setup() {
  pinMode(buzzer,OUTPUT);
  pinMode(role,OUTPUT);
  pinMode(led_kirmizi,OUTPUT);
  pinMode(led_yesil,OUTPUT);
  pinMode(x1, OUTPUT);  
  pinMode(x2, OUTPUT);  
  pinMode(x3, OUTPUT);  
  pinMode(x4, OUTPUT);  
  pinMode(y1, INPUT);  
  pinMode(y2, INPUT);  
  pinMode(y3, INPUT);  

  pinMode(btn_kapi_ac,INPUT);
  pinMode(btn_kapi_kilitle,INPUT);
 
  digitalWrite(x1,1);
  digitalWrite(x2,1);
  digitalWrite(x3,1);
  digitalWrite(x4,1); 
  digitalWrite(led_kirmizi,1);
  digitalWrite(led_yesil,0);

  Serial.begin(9600);// initialize serial:
  Serial.println("veri almaya hazır :");

  eeprom_verilerini_yukle();
}
//*****************************************************************************
//*****************************************************************************
void loop() {
//-----------------------------------------------------------------------------
        if(tustakimi_kontrol())
        {
            if( basilan_tus==kare && sifre_kontrol()==true && sifre_degistirme_islemi_basladimi==false)
            {
              /*
              son basılan tuş kare ise ve şifre kontrol doğru ise ve şu anda şifre değiştirme menüsünde değilse
              kullanıcı şifreyi girip kilidi açmak istiyor demektir.
              */
              tustakimi_sayaci_kullanilsinmi=false;//gecikme uyarısı iptal.
              kapi_kilitlimi=false;
              kapiyi_ac();// bu fonksiyonda aynı anda buzzer uyarısı da var
              tus_takimi_verilerini_bosalt();
              Serial.println("kilit açıldı");
            }

            if( basilan_tus==kare && sifre_kontrol()==false && sifre_degistirme_kodu_kontrol()==false && sifre_degistirme_islemi_basladimi==false)
            {
              /*
              Şifre hatalı ise ve kullanıcı şifre değiştirme modunda değilse kareye basıldığı anda girilen şifre hatalıdır,
              hiçbir işlem yapılmaz.
              */
              tustakimi_sayaci_kullanilsinmi=false;
              hata_biplemesi();
              Serial.println("Şifre hatalı");
            }

          if( basilan_tus==kare && sifre_degistirme_kodu_kontrol()==true && sifre_degistirme_islemi_basladimi==false)
            {
              /*
              kullanıcı şifre değiştirme menüsüne ilk defa girmek istemiş. gerekli kodu yazıp kare tuşuna basmış.
              */
              yeni_sifre_ilk_defa_girildimi=false;
              sifre_degistirme_islemi_basladimi=true;
              tustakimi_sayaci_kullanilsinmi=true;
              tus_takimi_verilerini_bosalt();
              onay_biplemesi();
              Serial.println("ŞİFRE DEĞİŞTİRME KODU DOĞRU GİRİLDİ.");
            }
          //..........................................................................................
          if( basilan_tus==kare && sifre_degistirme_islemi_basladimi==true)
            {
              /*
              şifre değiştirme menüsüne girdikten sonra yapılacak işlemler
              1 - beş haneli yeni şifreni gir ve kare tuşuna bas, onay sesi gelecektir.
              2 - beş haneli yeni şifreni tekrar gir ve tekrar kare tuşuna bas hata yok ise onay sesi gelir ve şifre değişir. 
              */
              if( yeni_sifre_ilk_defa_girildimi == false)
              {
                /*
                burada yeni şifre ilk defa giriliyor.
                */
                tum_sayilar_girilmismi=true;// kare hariç eksiksiz 5 tane rakam girilirse aşağıdaki kontrolde bu değişken true kalır.
                for (int i = 1; i < 6 ; i++)
                {
                      if(tustakimi_verisi[i]>9) 
                      { 
                        tum_sayilar_girilmismi=false; // ilk şifre girilirken kare tuşundan önce 5 ten az sayıda rakam girilmiş. HATA!
                        Serial.println("ilk defa girilen yeni şifre hatalı.");
                        }
                }
                    if(tum_sayilar_girilmismi == true) // kare tuşundan önce yeni şifre ilk defa eksiksiz olarak girilmiş.
                    {
                          yeni_sifre_ilk_giris[4] = tustakimi_verisi[1] ; // birazdan yapılacak kontroller için girilen şifre
                          yeni_sifre_ilk_giris[3] = tustakimi_verisi[2] ; // hafızaya alınıyor.
                          yeni_sifre_ilk_giris[2] = tustakimi_verisi[3] ;
                          yeni_sifre_ilk_giris[1] = tustakimi_verisi[4] ;
                          yeni_sifre_ilk_giris[0] = tustakimi_verisi[5] ;
                          yeni_sifre_ilk_defa_girildimi=true; // şifre ikinci defa girilme kontrolü bu değişkenle sağlanacak.
                          onay_biplemesi();
                          Serial.println("ilk defa girilen yeni şifreler hafızaya alındı.");
                    }
                    else{
                      sifre_degistirme_islemi_basladimi=false; // ilk defa girilen yeni şifrede eksik hane olduğu için işlem 
                      yeni_sifre_ilk_defa_girildimi=false;    // iptal ediliyor. Gerekirse şifre değiştirme kodu yeniden girilerek
                      hata_biplemesi(); // işlemler tekrarlanabilir.
                    }
              }
                    else{   
                        /*
                        daha önce yeni şifre bir defa girilmiş. bu else içinde yeni şifrenin ikinci defa giriş kontrolü yapılıyor.
                        */
                      tum_sayilar_girilmismi=true;
                      for (int i = 1; i < 6 ; i++)
                      {
                            if(tustakimi_verisi[i]>9)   // yukarıdaki kısım ile aynı.
                            { 
                              tum_sayilar_girilmismi=false; 
                              Serial.println("ikinci defa girilen yeni şifre hatalı.");
                              }
                      }
                          if(tum_sayilar_girilmismi == true)
                          {
                          yeni_sifre_ikinci_giris[4] = tustakimi_verisi[1] ; // kullanıcı ikinci defa da eksiksiz olarak 5 haneli şifre girmiş.
                          yeni_sifre_ikinci_giris[3] = tustakimi_verisi[2] ;
                          yeni_sifre_ikinci_giris[2] = tustakimi_verisi[3] ;
                          yeni_sifre_ikinci_giris[1] = tustakimi_verisi[4] ;
                          yeni_sifre_ikinci_giris[0] = tustakimi_verisi[5] ;
                          onay_biplemesi();
                          yeni_sifre_ilk_defa_girildimi=false;
                          sifre_degistirme_islemi_basladimi=false;
                          Serial.println("ikinci defa girilen yeni şifreler hafızaya alındı.");
                          
                          yeni_sifreler_eslesiyormu=true;
                          for (int i = 0; i < 5; i++)
                          {
                            if(yeni_sifre_ilk_giris[i] != yeni_sifre_ikinci_giris[i]) // ilk girilen yeni şifre ile ikinci defa girilenler
                              {                                   // karşılaştırılıyor.
                                yeni_sifreler_eslesiyormu = false;
                                }
                          }
                                if(yeni_sifreler_eslesiyormu==true)// ilk girilen yeni şifre ile ikinci defa girilen yeni şifre uyuşmuş. hata yok.
                                {
                                tustakimi_sayaci_kullanilsinmi=false;// işlem bittiğinde bu sayaç iptal edilip uyarı vermesi engellenecek.
                                eeproma_yeni_sifreyi_kaydet(); // yeni şifre eeproma kaydedildi ve kullanıma hazır.
                                Serial.println("şifre kaydedildi.");
                                }
                                else{
                                    sifre_degistirme_islemi_basladimi=false;  // ilk girilen yeni şifre ile ikinci girilen yeni şifre uyuşmamış
                                    yeni_sifre_ilk_defa_girildimi=false;      // hata mesajı verilip tüm işlemler iptal edilip ana menüye dönülüyor.
                                    tustakimi_sayaci_kullanilsinmi=false;
                                    Serial.println("şifreler uyuşmuyor, işlem iptal edildi.");
                                    hata_biplemesi();
                                    }
                          }
                          else{   
                              sifre_degistirme_islemi_basladimi=false; // ilk defa girilen yeni şifrede eksik hane olduğu için işlem 
                              yeni_sifre_ilk_defa_girildimi=false;    // iptal ediliyor. Gerekirse şifre değiştirme kodu yeniden girilerek
                              hata_biplemesi(); // işlemler tekrarlanabilir.
                          }
                    }
              tus_takimi_verilerini_bosalt();                  
           }
        }
//-----------------------------------------------------------------------------
        if(digitalRead(btn_kapi_ac) == 0 )
        {
          if(kapi_kilitlimi == false)
          {
        delay(50);
        while(digitalRead(btn_kapi_ac) == 0) { delay(5); digitalWrite(buzzer,1); }
        digitalWrite(buzzer,0);
        delay(50);
        kapiyi_ac();
        Serial.println("KİLİT AÇILDI");

          }
          else
          {
            hata_biplemesi();
          }
        }  
//-----------------------------------------------------------------------------
        if(digitalRead(btn_kapi_kilitle) == 0 )
        {
        delay(50);
        while(digitalRead(btn_kapi_kilitle) == 0) { delay(5); digitalWrite(buzzer,1); }
        digitalWrite(buzzer,0);
        delay(100);
        onay_biplemesi();     
        tus_takimi_verilerini_bosalt();        
        kapi_kilitlimi=true;
        digitalWrite(role,0);
        digitalWrite(led_kirmizi,1);
        digitalWrite(led_yesil,0);
        Serial.println("KAPI KİLİTLENDİ");
        }          
//-----------------------------------------------------------------------------
        if(tustakimi_sayaci_kullanilsinmi == true && millis() > (eskizaman + 5000))
        {
          hata_biplemesi();
          sifre_degistirme_islemi_basladimi=false;
          yeni_sifre_ilk_defa_girildimi=false;
          tusa_basildimi=false;              
          tustakimi_sayaci_kullanilsinmi=false;
          tus_takimi_verilerini_bosalt();
          Serial.println("ZAMAN AŞIMI OLUŞTU. İŞLEMLER İPTAL EDİLDİ");
        }
//-----------------------------------------------------------------------------
        if (rfid_veri_geldimi) 
        {
          uart_kesmesi_pasif();
          if(checksum_kontrol())
          {
           // Serial.println("checksum değeri doğru");
            onay_biplemesi();
          }
          else 
          {
            //Serial.println("checksum değeri hatali");
            hata_biplemesi();
          }
          rfid_veri_geldimi=false;
          //verileri_ekrana_yazdir();        
          delay(500);
          uart_kesmesi_aktif();
        }
//-----------------------------------------------------------------------------        
  }



        



