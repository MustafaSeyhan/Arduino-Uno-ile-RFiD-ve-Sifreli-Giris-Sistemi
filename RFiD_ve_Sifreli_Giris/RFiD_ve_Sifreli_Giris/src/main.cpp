#include "Arduino.h"
#include "tanimlamalar.h"
#include "uart_islemleri.h"
#include "buzzer_islemleri.h"
#include "eeprom_islemleri.h"
#include "tustakimi.h"
#include "kontrol_islemleri.h"

volatile  bool            tum_rfid_kartlar_silinecekmi=false;
volatile  bool            sisteme_yeni_kart_eklenecekmi=false;
volatile  bool            bir_kullanici_karti_silinecekmi=false;
volatile  bool            yeni_sifreler_eslesiyormu=false;
volatile  bool            tum_sayilar_girilmismi=false;
volatile  bool            yeni_sifre_ilk_defa_girildimi=false;
volatile  bool            sifre_degistirme_islemi_basladimi=false;
volatile  bool            zaman_asimi_sayaci_kullanilsinmi=false;
volatile  bool            rfid_veri_geldimi = false;  
volatile  bool            veri_alimi_hazirmi= false;
volatile  bool            kapi_kilitlimi = true; // kullanıcı tarafından şifre girilip kapı kilitlenmiş ise TRUE dir.

volatile  uint8_t         basilan_tus=TUS_TAKIMI_BOSLUK_VERISI;
volatile  uint8_t         tustakimi_verisi[10];
volatile  uint8_t         kullanici_sifresi[5];
volatile  uint8_t         yeni_sifre_ilk_giris[5];
volatile  uint8_t         yeni_sifre_ikinci_giris[5];
volatile  uint8_t         kayitli_kart_sayisi=0;
volatile  uint8_t         kayitli_rfid_datalari[MAX_RFID_ALANI];
volatile  uint8_t         checksum_degeri = 0;
          uint8_t         okunan_rfid_kartin_kimlik_degeri[KART_DATA_BOYUTU] ;
          uint8_t         checksum_hesapla();

          char            rfid_buffer[BUFFER_SIZE];

volatile  unsigned long   eskizaman=0;

//*****************************************************************************
//*****************************************************************************
void setup() {
  pinMode(buzzer,     OUTPUT);
  pinMode(role,       OUTPUT);
  pinMode(led_kirmizi,OUTPUT);
  pinMode(led_yesil,  OUTPUT);
  pinMode(x1,         OUTPUT);  
  pinMode(x2,         OUTPUT);  
  pinMode(x3,         OUTPUT);  
  pinMode(x4,         OUTPUT);  
  pinMode(y1,         INPUT);  
  pinMode(y2,         INPUT);  
  pinMode(y3,         INPUT);  

  pinMode(btn_kapi_ac,      INPUT);
  pinMode(btn_kapi_kilitle, INPUT);
 
  digitalWrite(x1,1);
  digitalWrite(x2,1);
  digitalWrite(x3,1);
  digitalWrite(x4,1); 
  digitalWrite(led_kirmizi,1);
  digitalWrite(led_yesil,0);

  Serial.begin(9600);// initialize serial:
  eeprom_datalari_yukle(true);
//  Serial.println("veri almaya hazır :");
  acilis_biplemesi();
}
//*****************************************************************************
//*****************************************************************************
void loop() {
      if(tustakimi_kontrol()) // herhangi bir tuşa basılmışsa buraya girer
      {
        kart_silme_istegi_kontrolu_yap(); // Sistemde kayıtlı bir kartın silinmesi isteği sorgulanıyor..

        kart_ekleme_istegi_kontrolu_yap(); // Sisteme yeni kart ekleme isteğinin olup olmadığı sorgulanıyor.  

        tum_rfid_kartlarin_silinmesi_istegi_kontrolu_yap(); // hafızaya kayıtlı tüm rfid kartların silinme isteği kontrolü

        sifre_degistirme_islemleri_kontrolu_yap();//kullanıcı kilit açma şifresini değiştirme kodunu mu girmiş kontrol edilir. 

        sifreli_giris_istegi_kontrolu_yap();//kullanıcı kilit açma şifresini mi girmiş kontrol edilir.
     
      }
    
    kapi_acma_buton_kontrolu_yap(); // buton ile kapının açma isteği kontrol ediliyor.

    kapi_kapatma_buton_kontrolu_yap(); // buton ile kapının kapatılma isteği kontrol ediliyor.

    zaman_asimi_kontrolu_yap(); // bir şekilde butona basıldıktan sonra 5000msn içinde işlem yapılmaz ise sistem başa dönüyor.  

    rfid_veri_kontrolu_yap();
}



        



