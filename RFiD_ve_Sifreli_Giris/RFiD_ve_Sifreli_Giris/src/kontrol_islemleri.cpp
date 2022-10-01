#include "kontrol_islemleri.h"

extern  volatile    unsigned long   eskizaman;

extern  volatile    uint8_t         kullanici_sifresi[];
extern  volatile    uint8_t         yeni_sifre_ilk_giris[];
extern  volatile    uint8_t         yeni_sifre_ikinci_giris[];
extern  volatile    uint8_t         tustakimi_verisi[];
extern  volatile    uint8_t         kayitli_rfid_datalari[];
extern  volatile    uint8_t         basilan_tus;
extern              uint8_t         okunan_rfid_kartin_kimlik_degeri[];

extern  volatile    bool            rfid_veri_geldimi;
extern  volatile    bool            bir_kullanici_karti_silinecekmi;
extern  volatile    bool            zaman_asimi_sayaci_kullanilsinmi;
extern  volatile    bool            sifre_degistirme_islemi_basladimi;
extern  volatile    bool            sisteme_yeni_kart_eklenecekmi;
extern  volatile    bool            tum_rfid_kartlar_silinecekmi;
extern  volatile    bool            kapi_kilitlimi; 
extern  volatile    bool            yeni_sifre_ilk_defa_girildimi;
extern  volatile    bool            tum_sayilar_girilmismi;
extern  volatile    bool            yeni_sifreler_eslesiyormu;



//***********************************************************************************************************************
//***********************************************************************************************************************
bool sifre_kontrol(){
/*
    tuş takımı verisinin 0. baytı şu anda kare bilgisi ile dolu olduğu için
    şifre kontrol işlemi, tustakimi_verisi[] dizisinin 1. baytından başlıyor.
    Şifremiz diyelim ki 12345 olsun. eeprom hafızdan alınan şifre bilgisinin 4. baytı 5 ile dolu ancak tuş takımında veri kaydırması
    sağdan sola doğru olduğundan # tuşu tustakimi_verisi[0] nolu baytında, 5 bilgisi tustakimi_verisi[1] nolu baytında oluyor.
    Haliyle aşağıdaki karşılaştırmalarda 1. bay 1. baytla değil simetriği ile kontrol ediliyor.

*/
   if(tustakimi_verisi[1]==kullanici_sifresi[4] && tustakimi_verisi[2]==kullanici_sifresi[3] &&
      tustakimi_verisi[3]==kullanici_sifresi[2] && tustakimi_verisi[4]==kullanici_sifresi[1] &&
      tustakimi_verisi[5]==kullanici_sifresi[0])
      {
        return true;
      }
      return false;
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void  kontrol_verilerini_temizle()
{
      bir_kullanici_karti_silinecekmi=false; 
      sisteme_yeni_kart_eklenecekmi=false;
      tum_rfid_kartlar_silinecekmi=false;
      sifre_degistirme_islemi_basladimi=false;  // ilk girilen yeni şifre ile ikinci girilen yeni şifre uyuşmamış
      yeni_sifre_ilk_defa_girildimi=false;      // hata mesajı verilip tüm işlemler iptal edilip ana menüye dönülüyor.
      zaman_asimi_sayaci_kullanilsinmi=false;
      tus_takimi_verilerini_bosalt();  
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void kapi_ac()
{
  /*
  kapı açma şifresi doğru girilip # tuşuna basılmış yada 
  kayıtlı bir rfid kart okutulmuş yada
  şifre daha önceden girilip şifre açıkmış ve kullanıcı kapıyı açma butonuna basmış.
  */
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
//***********************************************************************************************************************
//***********************************************************************************************************************
bool rfid_kart_kontrolu_yap()
{
  /*
  kullanıcı bir kartı okutup kapıyı açmak istediğinde kartın 5 baytlık id bilgisi ile hafızadaki tüm kartların
  id bilgileri karşılaştırılıyor. veriler 5 bayt halinde saklandığı için kontroller de beşer baytlık gruplar halinde 
  yapılıyor. Bir eşleşme yakalandığı anda for döngüsü kırılıp geri dönülüyor.
  */
    bool okunan_kart_ile_eslesme_varmi=false;
    
    for (uint16_t i =0 ; i<MAX_RFID_ALANI ; i=i+5)
    {
        if( kayitli_rfid_datalari[i]   == okunan_rfid_kartin_kimlik_degeri[0] && 
            kayitli_rfid_datalari[i+1] == okunan_rfid_kartin_kimlik_degeri[1] &&
            kayitli_rfid_datalari[i+2] == okunan_rfid_kartin_kimlik_degeri[2] &&
            kayitli_rfid_datalari[i+3] == okunan_rfid_kartin_kimlik_degeri[3] &&
            kayitli_rfid_datalari[i+4] == okunan_rfid_kartin_kimlik_degeri[4]  )
            {
                okunan_kart_ile_eslesme_varmi=true;
             //   Serial.println("Kayıtlı Kullanıcı");
                break;
            }                
    }
    return okunan_kart_ile_eslesme_varmi;
}
//***********************************************************************************************************************
//***********************************************************************************************************************
bool kullanici_karti_silme_kodu_kontrol(void){
/*
    Burada kullanıcının bir kartı silmeyi isteyip istemedğini anlamak için kart silme kodunu girmiş mi diye kontrol 
    ediliyor.
*/
    bool kullanici_karti_silme_kodu_dogrumu=true;
    int j=9;
    for(int i=0 ; i<10 ; i++)
    {
    if(kullanici_karti_silme_kodu[j] != tustakimi_verisi[i])
    {
      /*
      girilen kodda herhangi bir tuş verisinde hata bulduğu anda bu durum false olarak işaretleniyor.
      */
        kullanici_karti_silme_kodu_dogrumu=false;
    }  
    j--;  
    }

    if(kullanici_karti_silme_kodu_dogrumu == true)   
    {
        return true;// şifre değiştirme kodu girilmiş.
    }
        return false;
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void kart_silme_istegi_kontrolu_yap()
{
  /*
  kart silme isteği kontrolünün başlangıcı. Burada silme kodu sorgulanır. daha önce şifre değiştirme işlemi başlamamışsa
  ve kart silme kodu girilmişse bu durum "bir_kullanici_karti_silinecekmi=true" ile ifade edilir. Bundan sonra zaman aşımı
  süresi içinde silinecek kartın okutulması beklenir. 
  */
    if( basilan_tus == kare && kullanici_karti_silme_kodu_kontrol()==true && sifre_degistirme_islemi_basladimi==false)
    {
        bir_kullanici_karti_silinecekmi = true;
        zaman_asimi_sayaci_kullanilsinmi=true;
        tus_takimi_verilerini_bosalt();
        onay_biplemesi();
      //  Serial.println("SİLİNECEK KULLANICI KARTINI OKUTUN");    
        /*
        kullanıcı kartı silme işleminin bundan sonraki kısmı RFiD kontrollerinin yapıldığı kısımda devam edidyor.
        */        
    }
}
//***********************************************************************************************************************
//***********************************************************************************************************************
bool kullanici_karti_ekleme_kodu_kontrol(){
    /*
        kullanıcının kart eklemeyi isteyip istemediğinin kod kontorlü burada yapılır. 
    */
    bool kullanici_karti_ekleme_kodu_dogrumu=true;
    int j=9;
    for(int i=0 ; i<10 ; i++)
    {
    if(kullanici_karti_ekleme_kodu[j] != tustakimi_verisi[i])
    {
        kullanici_karti_ekleme_kodu_dogrumu=false;
    }  
    j--;  
    }

    if(kullanici_karti_ekleme_kodu_dogrumu == true)   
    {
        return true;// şifre değiştirme kodu girilmiş.
    }
        return false;
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void kart_ekleme_istegi_kontrolu_yap()
{
    /*
  kart ekleme isteği kontrolünün başlangıcı. Burada ekleme kodu sorgulanır. daha önce şifre değiştirme işlemi başlamamışsa
  ve kart ekleme kodu girilmişse bu durum "sisteme_yeni_kart_eklenecekmi=true" ile ifade edilir. Bundan sonra zaman aşımı
  süresi içinde eklenecek kartın okutulması beklenir. 
  */
    if( basilan_tus == kare && kullanici_karti_ekleme_kodu_kontrol()==true && sifre_degistirme_islemi_basladimi==false)
    {
        sisteme_yeni_kart_eklenecekmi = true;
        zaman_asimi_sayaci_kullanilsinmi=true;
        tus_takimi_verilerini_bosalt();
        onay_biplemesi();
       // Serial.println("EKLENECEK KARTI OKUTUN");       
        /*
        kullanıcı kartı ekleme işleminin bundan sonraki kısmı RFiD kontrollerinin yapıldığı kısımda devam edidyor.
        */      
    }
}
//***********************************************************************************************************************
//***********************************************************************************************************************
bool tum_rfid_kayitlarini_silme_kodu_kontrol(){
  /*
    kullanıcının tüm kartları hafızdan silmeyi isteyip istemediğinin kod kontorlü burada yapılır. 
  */
    bool tum_kullanici_rfid_kayitlarini_silme_kodu_dogrumu=true;
    int j=9;
    for(int i=0 ; i<10 ; i++)
    {
    if(tum_rfid_kartlari_silme_kodu[j] != tustakimi_verisi[i])
    {
        tum_kullanici_rfid_kayitlarini_silme_kodu_dogrumu=false;
    }  
    j--;  
    }

    if(tum_kullanici_rfid_kayitlarini_silme_kodu_dogrumu == true)   
    {
        return true;// kullanici rfid kayitlarini silme kodu girilmiş.
    }
        return false;
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void tum_rfid_kartlarin_silinmesi_istegi_kontrolu_yap()
{
/*
Tüm RFiD kartların silinip silinmeme isteği sorgulanıyor. Tüm hafızanın silinmesi için gerekli olan kodun 
iki defa girilmesi isteniyor. Bundan dolayı aşağıda tum_rfid_kartlar_silinecekmi değişkeni ilk kod girildiğinde 
false den true ye dönüyor. kod ikinci defa girildiğinde yazılım tum_rfid_kartlar_silinecekmi değişkeninin içeriği 
false ise birinci defa şifre girildiğini anlayarak ikinci kod girişinde verileri siliyor.
*/
    if( basilan_tus == kare &&  tum_rfid_kayitlarini_silme_kodu_kontrol()==true 
    && sifre_degistirme_islemi_basladimi==false && tum_rfid_kartlar_silinecekmi==false)
      {
        tum_rfid_kartlar_silinecekmi = true;
        zaman_asimi_sayaci_kullanilsinmi=true;
        tus_takimi_verilerini_bosalt();
        onay_biplemesi();
      //  Serial.println("Kartları silme kodunu tekrar girin.");            
      }
      else if( basilan_tus == kare && tum_rfid_kayitlarini_silme_kodu_kontrol()==true 
      && sifre_degistirme_islemi_basladimi==false && tum_rfid_kartlar_silinecekmi==true)
          {
              tum_rfid_kartlar_silinecekmi = false;
              zaman_asimi_sayaci_kullanilsinmi=false;
              tus_takimi_verilerini_bosalt();
              onay_biplemesi();
              eepromu_tamamen_sil();
              onay_biplemesi();
              onay_biplemesi();
              onay_biplemesi();
           //   Serial.println("TÜM RFID KARTLAR SİLİNDİ...");            
          }
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void sifreli_giris_istegi_kontrolu_yap()
{
/*
son basılan tuş kare ise ve şifre kontrol doğru ise ve şu anda şifre değiştirme menüsünde değilse, ve kart ekleme veya çıkarma
gibi işlemler yapılmıyorsa kullanıcının girdiği şifre göz önünde bulundurlup kapı açılır. 
*/          
    if( basilan_tus == kare && sifre_kontrol()==true && sifre_degistirme_islemi_basladimi==false && tum_rfid_kartlar_silinecekmi == false
       && bir_kullanici_karti_silinecekmi==false && sisteme_yeni_kart_eklenecekmi==false )
    {
        zaman_asimi_sayaci_kullanilsinmi=false;//gecikme uyarısı iptal.
        kapi_kilitlimi=false;
        kapi_ac();// bu fonksiyonda aynı anda buzzer uyarısı da var
        tus_takimi_verilerini_bosalt();
       // Serial.println("kilit açıldı");
    } 
/*
Hatalı şifre girişi kontrol kısmı. Şifre hatalı ise ve kullanıcı şifre değiştirme modunda 
değilse kareye basıldığı anda girilen şifre hatalıdır, hiçbir işlem yapılmaz.
*/            
    else if( basilan_tus == kare && sifre_kontrol()==false && sifre_degistirme_kodu_kontrol()==false && sifre_degistirme_islemi_basladimi==false)
    {
      zaman_asimi_sayaci_kullanilsinmi=false;
      hata_biplemesi();
      tus_takimi_verilerini_bosalt();
    //  Serial.println("Şifre hatalı");
    }       
}
//***********************************************************************************************************************
//***********************************************************************************************************************
bool sifre_degistirme_kodu_kontrol(void){
  /*
    kullanıcının tuş takımı şifresini değiştirmeyi isteyip istemediğinin kod kontorlü burada yapılır. 
  */  
    bool sifre_degistirme_kodu_dogrumu=true;
    int j=9;
    for(int i=0 ; i<10 ; i++)
    {
    if(sifre_degistirme_kodu[j] != tustakimi_verisi[i])
    {
        sifre_degistirme_kodu_dogrumu=false;
    }  
    j--;  
    }

    if(sifre_degistirme_kodu_dogrumu == true)   
    {
        return true;// şifre değiştirme kodu girilmiş.
    }
        return false;
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void sifre_degistirme_islemleri_kontrolu_yap()
{
  /*
    Şifre değiştirme menüsüne giriş kontrolü kullanıcı şifre değiştirmek için şifre değiştirme menüsüne girme kodunu
    tuşlamış kareye basmış.
  */
    if( basilan_tus == kare && sifre_degistirme_kodu_kontrol()==true && sifre_degistirme_islemi_basladimi==false)
      {
        yeni_sifre_ilk_defa_girildimi=false;
        sifre_degistirme_islemi_basladimi=true;
        zaman_asimi_sayaci_kullanilsinmi=true;
        tus_takimi_verilerini_bosalt();
        onay_biplemesi();
     //   Serial.println("BEŞ HANELİ YENİ ŞİFRENİZİ GİRİP # TUŞUNA BASIN.");
      }
    //..........................................................................................
    if( basilan_tus == kare && sifre_degistirme_islemi_basladimi==true)
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
                if(tustakimi_verisi[i]>9 || tustakimi_verisi[6] != TUS_TAKIMI_BOSLUK_VERISI) 
                { 
                  tum_sayilar_girilmismi=false; // ilk şifre girilirken kare tuşundan önce 5 ten az sayıda rakam girilmiş. HATA!
                  }                             // yada 5 ten fazla tuşa basılmış mı kontrol ediyoruz. 6. bayt dolu ise
                                                // kullanıcı kare hariç 5 ten fazla şifre hanesi girmiş demektir.
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
                 //   Serial.flush();
                 //   Serial.println("BEŞ HANELİ ŞİFRENİZİ TEKRAR GİRİP # TUŞUNA BASIN.");
              }
              else{
                zaman_asimi_sayaci_kullanilsinmi=false;
                sifre_degistirme_islemi_basladimi=false; // ilk defa girilen yeni şifrede eksik hane olduğu için işlem 
                yeni_sifre_ilk_defa_girildimi=false;    // iptal ediliyor. Gerekirse şifre değiştirme kodu yeniden girilerek
              //  Serial.println("Eksik veya fazla rakam girildi.");
                hata_biplemesi(); // işlemler tekrarlanabilir.
              }
        }
              else{   
                  /*
                  daha önce yeni şifre bir defa girilmiş. bu else içinde yeni şifrenin ikinci defa giriş kontrolü yapılıyor.
                  */
                  tum_sayilar_girilmismi=true;// kare hariç eksiksiz 5 tane rakam girilirse aşağıdaki kontrolde bu değişken true kalır.
                  for (int i = 1; i < 6 ; i++)
                  {
                        if(tustakimi_verisi[i]>9 || tustakimi_verisi[6] != TUS_TAKIMI_BOSLUK_VERISI) 
                        { 
                          tum_sayilar_girilmismi=false; // ilk şifre girilirken kare tuşundan önce 5 ten az sayıda rakam girilmiş. HATA!
                          }                             // yada 5 ten fazla tuşa basılmış mı kontrol ediyoruz. 6. bayt dolu ise
                                                        // kullanıcı kare hariç 5 ten fazla şifre hanesi girmiş demektir.
                  }

                    if(tum_sayilar_girilmismi == true)
                    {
                    yeni_sifre_ikinci_giris[4] = tustakimi_verisi[1] ; // kullanıcı ikinci defa da eksiksiz olarak 5 haneli şifre girmiş.
                    yeni_sifre_ikinci_giris[3] = tustakimi_verisi[2] ;
                    yeni_sifre_ikinci_giris[2] = tustakimi_verisi[3] ;
                    yeni_sifre_ikinci_giris[1] = tustakimi_verisi[4] ;
                    yeni_sifre_ikinci_giris[0] = tustakimi_verisi[5] ;
                    onay_biplemesi();
                    kontrol_verilerini_temizle();
                                        
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
                          zaman_asimi_sayaci_kullanilsinmi=false;// işlem bittiğinde bu sayaç iptal edilip uyarı vermesi engellenecek.
                          eeproma_yeni_sifreyi_kaydet(); // yeni şifre eeproma kaydedildi ve kullanıma hazır.
                      //    Serial.println("YENİ ŞİFRE KAYDEDİLDİ.");
                          }
                          else{
                              sifre_degistirme_islemi_basladimi=false;  // ilk girilen yeni şifre ile ikinci girilen yeni şifre uyuşmamış
                              yeni_sifre_ilk_defa_girildimi=false;      // hata mesajı verilip tüm işlemler iptal edilip ana menüye dönülüyor.
                              zaman_asimi_sayaci_kullanilsinmi=false;
                           //   Serial.println("şifreler uyuşmuyor, işlem iptal edildi.");
                              hata_biplemesi();
                              }
                    }
                    else{   
                        zaman_asimi_sayaci_kullanilsinmi=false;
                        sifre_degistirme_islemi_basladimi=false; // ilk defa girilen yeni şifrede eksik hane olduğu için işlem 
                        yeni_sifre_ilk_defa_girildimi=false;    // iptal ediliyor. Gerekirse şifre değiştirme kodu yeniden girilerek
                      //  Serial.println("Eksik veya fazla rakam girildi.");
                        hata_biplemesi(); // işlemler tekrarlanabilir.
                    }
              }
        tus_takimi_verilerini_bosalt();                  
      }
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void kapi_acma_buton_kontrolu_yap()
{
        if(digitalRead(btn_kapi_ac) == 0 ) 
        {
          /*
          kapı açılma butonuna basıldığı sırada şifre daha önceden girilmişse kapi_kilitlimi değişkeni false durumundadır.
          buradan şifrenin girilmiş ve kapının kilitli olmadığını anlarız. Bu şartlar altında kapı açılır.
          */
          if(kapi_kilitlimi == false)
          {
        delay(50);
        while(digitalRead(btn_kapi_ac) == 0) { delay(5); digitalWrite(buzzer,1); }
        digitalWrite(buzzer,0);
        delay(50);
        kapi_ac();
      //  Serial.println("KİLİT AÇILDI");
          }
          else
          {
            hata_biplemesi();
          }
          kontrol_verilerini_temizle(); 
        }   
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void kapi_kapatma_buton_kontrolu_yap()
{
        if(digitalRead(btn_kapi_kilitle) == 0 )
        {
          /*
          şartlar ne olursa olsun kapı kilitlenir.
          */
        delay(50);
        while(digitalRead(btn_kapi_kilitle) == 0) { delay(5); digitalWrite(buzzer,1); }
        digitalWrite(buzzer,0);
        delay(100);
        onay_biplemesi();          
        kapi_kilitlimi=true;
        digitalWrite(role,0);
        digitalWrite(led_kirmizi,1);
        digitalWrite(led_yesil,0);
        kontrol_verilerini_temizle();
     //   Serial.println("KAPI KİLİTLENDİ");
        }   
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void zaman_asimi_kontrolu_yap()
{
  /*
  Herhangi bir işlemde zaman aşımı kontrolü yapılacaksa bu fonksiyon çağrılır.
  diyelimki bir tuşa basıldı. Bu basma anında "eskizaman" kayıtçısı millis fonksiyonunun getirdiği değer ile doldurulur.
  kullanıcı zaman aşımı süresi içinde tekrar butona basarsa "eskizaman" kayıtçısı tekrar o anki değer ile doldurulur.
  eğer zaman aşımı süresinde zaman_asimi_sayaci_kullanilsinmi = false yapılmadıysa ve süre dolduysa tüm işlemler iptal edilir
  program varsayılan duruma döner.
  */
        if(zaman_asimi_sayaci_kullanilsinmi == true && millis() > (eskizaman + ZAMAN_ASIMI_SURESI))
        {
          hata_biplemesi();
          kontrol_verilerini_temizle();
        //  Serial.println("ZAMAN AŞIMI OLUŞTU. İŞLEMLER İPTAL EDİLDİ");
        }    
}
//***********************************************************************************************************************
//***********************************************************************************************************************
void rfid_veri_kontrolu_yap()
{
  /*
  uart_islemleri.cpp modülündeki serialEvent() fonksiyonu her kart okumasında kesme ile çalışır.
  kart okuma işlemi başarılı ise  "rfid_veri_geldimi = true" olur.
  */
     if (rfid_veri_geldimi) 
      {        
      eskizaman = millis(); // her kart okuma eski zamanı şimdiye eşitler. 

        uart_kesmesi_pasif(); /* işlemler devam ederken arka arkaya kart okutsalar da karışıklık olmasın diye kesmeler
                              edilir ve güvenlik seviyesi arttırılır.
                              */
        if(checksum_kontrol())// okunan kartın bildileri doğru alınmış mı diye checksum kontrolüy
        {
            if(sisteme_yeni_kart_eklenecekmi==true)
            {
              eeproma_yeni_kullanici_kaydet();
              kontrol_verilerini_temizle();
            }
            else if(bir_kullanici_karti_silinecekmi==true)
                  {
                    eepromdan_bir_kullanici_sil();
                    kontrol_verilerini_temizle();
                  }
                  else if( rfid_kart_kontrolu_yap())
                        {                  
                          kontrol_verilerini_temizle();
                          kapi_ac();
                          digitalWrite(led_kirmizi,1);
                          digitalWrite(led_yesil,0);
                   //       Serial.println("KİLİT AÇILDI");
                        }
                        else
                          {
                            kontrol_verilerini_temizle();
                      //      Serial.println("Tanımsız Kart");
                            hata_biplemesi();
                          }
        }
        else 
        {
        //  Serial.println("checksum değeri hatali");
          hata_biplemesi();
        }
        rfid_veri_geldimi=false;
        verileri_ekrana_yazdir();        
        delay(500);
        uart_kesmesi_aktif();
      }    
}
