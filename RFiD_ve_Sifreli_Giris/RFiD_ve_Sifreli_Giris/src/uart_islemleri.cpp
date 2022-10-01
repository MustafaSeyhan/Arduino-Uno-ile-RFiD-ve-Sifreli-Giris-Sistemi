#include "uart_islemleri.h"

extern            char    rfid_buffer[BUFFER_SIZE];
extern            char    okunan_rfid_kartin_kimlik_degeri[KART_DATA_BOYUTU];
                  char    gelen_veri = 0;
extern            bool    rfid_veri_geldimi;
                  uint8_t buffer_sayaci=0;
extern  volatile  uint8_t checksum_degeri ;
//************************************************************************************************
//************************************************************************************************
void uart_kesmesi_aktif()
{
  UCSR0A &= ~(1<< RXC0);// UCSR0A KAYITÇISININ RXC0 BİTİ "0" YAPILARAK KESME BAYRAĞI SİLİNDİ.
  UCSR0B |= (1 << RXEN0) |  (1 << RXCIE0); //İlgili bitler "1" yapılarak UART kesmeleri aktif edildi   
}
//************************************************************************************************
//************************************************************************************************
void uart_kesmesi_pasif()
{
  UCSR0B &= ~((1 << RXEN0) | (1 << RXCIE0));  // İlgili bitler "0" yapılarak UART kesmeleri pasif
}
//************************************************************************************************
//************************************************************************************************
void serialEvent() {
  rfid_veri_geldimi = false;
  while (Serial.available()) {                  
      gelen_veri = (char)Serial.read();    
    if( gelen_veri == 0x02 )  //stx(start text) değeri gelmiş mi bakılıyor.
    {
      buffer_sayaci=0;// stx gelmiş. buffer başlangıç adresi 0 olarak ayarlanıyor.                                
    }   
    rfid_buffer[buffer_sayaci] = gelen_veri ; // her bir döngüde gelen veri "rfid_buffer[buffer_sayaci]" içine yükleniyor.
        if(buffer_sayaci==13 && rfid_buffer[0]==0x02 && rfid_buffer[13]==0x03 )
        {
          /*
          toplam 14 bayt veri gelmişse ve STX ve ETX verileri de gelmişse kartın yolladığı tüm bilgiler alınmıştır. 
          ana döngüde veri gelip gelmediğini kontrol etmek üzere "rfid_veri_geldimi = true" yapılır.  
          */
        buffer_sayaci=0;
        rfid_veri_geldimi = true;
        }
      buffer_sayaci++;  
      if(buffer_sayaci>=14) buffer_sayaci=0;// bir şekilde buffer sayacı alınacak max data sayısını aşarsa tedbiren sıfırlanıyor.
      }            
}
//************************************************************************************************
//************************************************************************************************
uint8_t ascii_convert(uint8_t convert_data)
{
  /*
  rfid kartta ascii olarak gelen dataların harf ve rakam değerleri döndürülüyor.
  bu işlem checksum hesaplamasında lazım oluyor.
  */
  uint8_t gidecek_data=0XCC;
  switch (convert_data)
  {
  case 48: gidecek_data = 0x00;    break;
  case 49: gidecek_data = 0x01;    break;
  case 50: gidecek_data = 0x02;    break;
  case 51: gidecek_data = 0x03;    break;
  case 52: gidecek_data = 0x04;    break;
  case 53: gidecek_data = 0x05;    break;
  case 54: gidecek_data = 0x06;    break;
  case 55: gidecek_data = 0x07;    break;
  case 56: gidecek_data = 0x08;    break;
  case 57: gidecek_data = 0x09;    break;
  case 65: gidecek_data = 0x0A;    break;
  case 66: gidecek_data = 0x0B;     break;
  case 67: gidecek_data = 0x0C;    break;
  case 68: gidecek_data = 0x0D;    break;
  case 69: gidecek_data = 0x0E;    break;
  case 70: gidecek_data = 0x0F;    break;
  default:    break;
  }
  return gidecek_data;
}
//************************************************************************************************
//************************************************************************************************
bool checksum_kontrol()
{
  /*
  125khz rfid kart her okumada 14 bayt bilgi aktarır. Bu baytların açıklaması;
alınan data :  ␂03006F60AEA2␃
BİR OKUMADA STX VE ETX DAHİL TOPLAM 14 BAYT DATA ALINIYOR.

STX 0  3  0  0  6  F  6  0  A  E  A  2  ETX
02 30 33 30 30 36 46 36 30 41 45 41 32 03 

bayt[0] 				        : STX - MESSAGE HEAD (START OF TEXT - değeri 0x02)
bayt[1]  .... bayt[2]  	: versiyon numarası(0x03) 
bayt[3]  .... bayt[10] 	: kart datası (data tag)
bayt[11] .... bayt[12]	: Checksum değeri (0xA2)
bayt[13] 	 			        : ETX - MESSAGE TAIL(END OF TEXT - değeri 0x03)

Message-Head(Kafa) 	: Bu değer 0x02 yani ASCII olarak STX değeri
Message-Tail(kuyruk): Bu değer 0x03 yani ASCII olarak ETX değeri

CHECKSUM HESAPLAMA : 
-----------------------------------------------------
 0  1  2  3  4  5  6  7  8  9 10 11 12 13. BAYTLAR 
1. adım --->>>  02 30 33 30 30 36 46 36 30 41 45 41 32 03   -> 14 BAYT VERİ ALINIR.
2. adım --->>>     30 33 30 30 36 46 36 30 41 45 41 32   	  -> BAŞTAN VE SONDAN STX VE ETX BAYTLARI ATILIR, 
3. adım --->>>     30 33 30 30 36 46 36 30 41 45  + 41 32	  -> SON İKİ BAYT CHECKSUM OLDUĞUNDAN AYRILIR.
4. adım --->>>      0  3  0  0  6  F  6  0  A  E  +  A  2	  -> ALINAN 10 BAYT VERİ ASCII DEĞERDEN HARF YADA SAYIYA DÖNÜŞÜR
5. adım --->>>   03    00    6F    60    AE + A2		        -> HER İKİ ASCII VERİ YAN YANA GELEREK 8 BİT SAYIYA DÖNÜŞÜR.	
6. adım --->>>   03 ^  00 ^  6F ^  60 ^  AE = A2		        -> OLUŞAN BU BEŞ BAYT VERİ EXOR İŞLEMİNE TABİ TUTULUR VE CHECKSUM BAYT ORTAYA ÇIKAR.
  
*/

    uint8_t low_nibble=0;
    uint8_t high_nibble=0;
    uint8_t checksum_bayt[6];
    uint8_t x = 0;
  for(int i=2 ; i<=10 ; i=i+2)
  {
/*
versiyon numarası ve kart dataları ascii koda dönüştürülüyor
*/
    high_nibble=ascii_convert(rfid_buffer[i-1]); // yukarıdaki açıklamanın 4. adım işlemi yapılıyor.
    low_nibble = ascii_convert(rfid_buffer[i]);  // yukarıdaki açıklamanın 4. adım işlemi yapılıyor.
    x = (i/2)-1;  
    checksum_bayt[x] = 0; // her iki bayt bir araya gelerek bir checksum hesaplama ara adımına dönüşüyor. yukarıdaki açıklamanın 5. adım işlemi yapılıyor.
    checksum_bayt[x] = ( high_nibble << 4 ) + low_nibble; // her iki bayt bir araya gelerek bir checksum hesaplama ara adımına dönüşüyor. yukarıdaki açıklamanın 5. adım işlemi yapılıyor.
  }

    checksum_degeri = checksum_bayt[0] ^ checksum_bayt[1] ^ checksum_bayt[2] ^ checksum_bayt[3] ^ checksum_bayt[4];
    // yukarıdaki açıklamanın 6. adımı yapılıyor. Exor ile checksum hesaplanıyor. 

    high_nibble=ascii_convert(rfid_buffer[11]);//rfid okuyucudan alınan checksum baytları tek baytlık veriye dönüştürülüp 
    low_nibble = ascii_convert(rfid_buffer[12]);// hesaplanan checksumile karşılaştırılmak üzere hazırlanıyor.  
    checksum_bayt[5] = 0;    
    checksum_bayt[5] = ( high_nibble << 4 ) + low_nibble; // karttan okunan checksum değeri checksum_bayt[5] içine yazıldı

   // Serial.println("------------------------------------------");
    if(checksum_degeri == checksum_bayt[5])// hesaplanan ve okunan değerler karışılaştırılıyor.
    {
    //  Serial.println("Checksum Değeri          : DOĞRU");
      okunan_rfid_kartin_kimlik_degeri[0] = checksum_bayt[0];
      okunan_rfid_kartin_kimlik_degeri[1] = checksum_bayt[1];
      okunan_rfid_kartin_kimlik_degeri[2] = checksum_bayt[2];
      okunan_rfid_kartin_kimlik_degeri[3] = checksum_bayt[3];
      okunan_rfid_kartin_kimlik_degeri[4] = checksum_bayt[4];
      return true;
    }
   // Serial.println("Checksum Değeri          : *** HATALI ***");
    return false;                     
}  
//************************************************************************************************
//************************************************************************************************
void verileri_ekrana_yazdir(){
         uart_kesmesi_pasif();
        uint8_t sayac_loop;
    //    Serial.print("Gelen Kimlik Verisi      : ");
        for(sayac_loop=1;sayac_loop<=12; sayac_loop++)
        {
     //     Serial.print(rfid_buffer[sayac_loop]);
        }
      //    Serial.println("");
      //  Serial.print("Hesaplanan Checksum Byte : ");
      //  Serial.println(checksum_degeri,HEX);
      //  Serial.println("------------------------------------------");
        rfid_veri_geldimi = false;
        delay(1000);         
    uart_kesmesi_aktif();
}
//************************************************************************************************
//************************************************************************************************

