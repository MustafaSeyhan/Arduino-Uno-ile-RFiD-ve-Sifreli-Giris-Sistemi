#include "uart_islemleri.h"

extern char rfid_buffer[BUFFER_SIZE];
extern char rfid_kimlik_degeri[KART_DATA_BOYUTU];
char gelen_veri = 0;
extern bool rfid_veri_geldimi;
uint8_t buffer_sayaci=0;
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
            while (Serial.available()) {                  
               gelen_veri = (char)Serial.read();    
              if( gelen_veri == 0x02 )
              {
                buffer_sayaci=0;                                
              }   
              rfid_buffer[buffer_sayaci] = gelen_veri ;                              
                  if(buffer_sayaci==13 && rfid_buffer[0]==0x02 && rfid_buffer[13]==0x03 )
                  {
                  buffer_sayaci=0;
                  rfid_veri_geldimi = true;
                  }
                buffer_sayaci++;  
                if(buffer_sayaci>=14) buffer_sayaci=0;
                }            
          }
//************************************************************************************************
//************************************************************************************************
uint8_t ascii_convert(uint8_t convert_data)
{
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
  case 66: gidecek_data = 0x0B;   break;
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
    uint8_t low_nibble=0;
    uint8_t high_nibble=0;
    uint8_t checksum_bayt[6];
    uint8_t x = 0;
    uint8_t checksum_degeri=0;
  for(int i=2 ; i<=10 ; i=i+2)
  {

    high_nibble=ascii_convert(rfid_buffer[i-1]);
    low_nibble = ascii_convert(rfid_buffer[i]);
    x = (i/2)-1;
    checksum_bayt[x] = 0;    
    checksum_bayt[x] = ( high_nibble << 4 ) + low_nibble;
  }

    checksum_degeri = checksum_bayt[0] ^ checksum_bayt[1] ^ 
                      checksum_bayt[2] ^ checksum_bayt[3] ^ checksum_bayt[4];
      
    high_nibble=ascii_convert(rfid_buffer[11]);//rfid okuyucudan alınan checksum baytları tek baytlık
    low_nibble = ascii_convert(rfid_buffer[12]);// veriye dönüştürülüp hesaplanan checksum ile karşılaştırılıyor.
   
    checksum_bayt[5] = 0;    
    checksum_bayt[5] = ( high_nibble << 4 ) + low_nibble;

            for( int x = 0 ; x<14; x++ )
            {
              rfid_buffer[x]=0;// bunu yapmayınca ikinci defa kart algılaması yapıyor.
            } 

    if(checksum_degeri == checksum_bayt[5])
    {
      //Serial.println("checksum değeri doğru");
      rfid_kimlik_degeri[0] = checksum_bayt[0];
      rfid_kimlik_degeri[1] = checksum_bayt[1];
      rfid_kimlik_degeri[2] = checksum_bayt[2];
      rfid_kimlik_degeri[3] = checksum_bayt[3];
      rfid_kimlik_degeri[4] = checksum_bayt[4];
      return true;
    }
    //Serial.println("checksum değeri hatalı");
    return false;                     
}  
//************************************************************************************************
//************************************************************************************************
void verileri_ekrana_yazdir(){
         uart_kesmesi_pasif();
        uint8_t sayac_loop;
        Serial.print("********* Gelen kimlik verisi ********* : ");
        for(sayac_loop=1;sayac_loop<=12; sayac_loop++)
        {
          Serial.print(rfid_buffer[sayac_loop]);
        }
          Serial.println("");
        Serial.print("Hesaplanan checksum byte : ");
        //Serial.println(checksum_kontrol(),HEX);
        rfid_veri_geldimi = false;
        delay(1000);         
    uart_kesmesi_aktif();
}
//************************************************************************************************
//************************************************************************************************

