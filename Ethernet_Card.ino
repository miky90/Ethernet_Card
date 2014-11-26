/*
*  File:     Ethernet_Card.ino
*  Author:   Marco Michielotto
*  Date:     01/06/2014
*  Hardware: Arduino Pro Mini(5V, 16Mhz)w/ ATmega328,
*            ENC28J60 on SPI connection (see below)
*            NRF2401L+ on SPI connection (see below)
*/

//Connect the nRF24L01 to Arduino like this.
// \code
//                 Arduino      NRF2401L+
//                 3V3----------VCC   (3.3v) 
//             pin D8-----------CE    (chip enable in)
//          SS pin D9-----------CSN   (chip select in)
//         SCK pin D13----------SCK   (SPI clock in)
//        MOSI pin D11----------SDI   (SPI Data in)
//        MISO pin D12----------SDO   (SPI data out)
//                              IRQ   (Interrupt output, not connected)
//                 GND----------GND   (ground in)
// \endcode
//Connect the ENC28J60 to Arduino like this.
//
//                 Arduino      ENC28J60
//                 3V3----------VCC
//             pin RESET--------RESET
//          SS pin D10----------CS    (chip select in)
//         SCK pin D13----------SCK   (SPI clock in)
//        MOSI pin D11----------SI    (SPI Data in)
//        MISO pin D12----------SO    (SPI data out)
//                              INT   ()
//                              WDL   (not connected)
//                              CLKOUT(not connected)
//                 GND----------GND   (ground in)

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <EtherCard.h> 
#include <Time.h>
#include <Timezone.h>

#define MY_ADDRESS 00
#define METEO_ADDRESS 01
// nRF24L01(+) radio attached using Getting Started board 
RF24 radio(8,9);

// Network uses that radio
RF24Network network(radio);

// Structure of our payload
struct payload_Meteo
{
  time_t time;
  float inTemp;
  float inHum;
  float outTemp;
  float outHum;
  float outPress;
};

//Ethernet parameters
static byte mac[] = {0x00,0x50,0x56,0xAE,0xB3,0xA6};
static byte ip[] = {192,168,1,150};
byte Ethernet::buffer[700];
#define ENC28J60_CS 10
char website[] PROGMEM= "www.web-plus.it"; // www.TUO_SITO_WEB.it
char buffer[70];
unsigned long time_last = 0; 
uint8_t connesso = 0;
uint8_t pending_data = 0;
byte interval = 30; // seconds - intervello di tempo tra un invio di dati ed il successivo verso il server

void setup()
{ 
  Serial.begin(9600);
  Serial.println("REBOOT");
  
  SPI.begin();
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  network.begin(/*channel*/ 125, /*node address*/ MY_ADDRESS);
  
  //testa scheda ethernet
  Serial.print("Verifica Enc28j60 ... ");
  if ( ether.begin( sizeof Ethernet::buffer, mac, ENC28J60_CS) )
    Serial.println("\tsuccess");
  else {
    Serial.println("\tfailed");
  }
  
  connesso = inizializeEth();
  
  Serial.println();
  Serial.print("Setup eseguito in :");
  Serial.print(millis()/1000);
  Serial.println("s");
  Serial.println();
  Serial.println();
}

payload_Meteo payload;

void loop()
{
  // Pump the network regularly
  network.update();
   // Is there anything ready for us?
  if( network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
      network.read(header,&payload,sizeof(payload));
      Serial.print("Received packet ");
      Serial.print("from: ");
      Serial.print(header.from_node); 
      Serial.print(" - time: ");
      Serial.print(payload.time);
      Serial.print(" inTemp: ");
      Serial.print(payload.inTemp);
      Serial.print(" inHum: ");
      Serial.print(payload.inHum);
      Serial.print(" outTemp: ");
      Serial.print(payload.outTemp);
      Serial.print(" outHum: ");
      Serial.print(payload.outHum);
      Serial.print(" Press: ");
      Serial.println(payload.outPress);
      pending_data = 1;
  }
  connesso = Ethernet::isLinkUp();
  if(connesso) {
    ether.packetLoop(ether.packetReceive());
    if(pending_data) {
      if (millis()/1000 > time_last){
        if ( (time_last = millis()/1000) % interval == 0 ){
          invioDati();
        }
      }
    }
  }
  else {
    if (millis()/1000 > time_last){
        if ( (time_last = millis()/1000) % interval == 0 ){
          connesso = inizializeEth(); 
        }
      }
  }
}
