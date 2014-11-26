static void response_callback (byte status, word off, word len) {
 
  Serial.println();
  Serial.println( (char *) Ethernet::buffer + off);
  Serial.println();
  int indice=0;
  uint8_t flag = 0;
  while(indice < len){
    if(buffer[indice++]!='"')
      flag=1;
  }
  if(flag==1){
    pending_data=0;
    String risposta;
    risposta += buffer[indice];
    risposta += buffer[indice+1];
    if(risposta.equals("ok")) {
      Serial.println("parametri inviati");
    }
    else if(buffer[indice]=='e'){
      Serial.println("errore invio");
    }
  }
  else {
     Serial.println("sito inaccessibile");
  }
}


static void invioDati() {
  
    //IN
    //consiero temperature da 80 a -40 (99 a -99 ^^)
    char* s_in_temp;
    if(payload.inTemp < 10 && payload.inTemp >= 0) {
      //x.xx 
      s_in_temp = new char[5];
      dtostrf(payload.inTemp, 1, 2, s_in_temp);
    }
    else if(payload.inTemp >=10 | payload.inTemp < 0) { 
      //xx.xx e -x.xx 
      s_in_temp = new char[6];
      dtostrf(payload.inTemp, 2, 2, s_in_temp);
    }
    else {
      //-xx.xx
      s_in_temp = new char[7];
      dtostrf(payload.inTemp, 3, 2, s_in_temp);
    }
    //consiero umidita da 0 a 100
    char* s_in_hum;
    if(payload.inHum < 10 ) {
      //x.xx 
      s_in_hum = new char[5];
      dtostrf(payload.inHum, 1, 2, s_in_hum);
    }
    else if(payload.inHum >=10 ) { 
      //xx.xx 
      s_in_hum = new char[6];
      dtostrf(payload.inHum, 2, 2, s_in_hum);
    }
    else {
      //100.00
      s_in_hum = new char[7];
      dtostrf(payload.inHum, 3, 2, s_in_hum);
    }
    //OUT
    //consiero temperature da 80 a -40 (99 a -99 ^^)
    char* s_out_temp;
    
    if(payload.outTemp < 10 && payload.outTemp >= 0) {
      //x.xx 
      s_out_temp = new char[5];
      dtostrf(payload.outTemp, 1, 2, s_out_temp);
    }
    else if(payload.outTemp >=10 | payload.outTemp < 0) { 
      //xx.xx e -x.xx 
      s_out_temp = new char[6];
      dtostrf(payload.outTemp, 2, 2, s_out_temp);
    }
    else {
      //-xx.xx
      s_out_temp = new char[7];
      dtostrf(payload.outTemp, 3, 2, s_out_temp);
    }
    //consiero umidita da 0 a 100
    char* s_out_hum;
    if(payload.outHum < 10 ) {
      //x.xx 
      s_out_hum = new char[5];
      dtostrf(payload.outHum, 1, 2, s_out_hum);
    }
    else if(payload.outHum >=10 ) { 
      //xx.xx 
      s_out_hum = new char[6];
      dtostrf(payload.outHum, 2, 2, s_out_hum);
    }
    else {
      //100.00
      s_out_hum = new char[7];
      dtostrf(payload.outHum, 3, 2, s_out_hum);
    }
    //PRESSIONE
    //considero pressioni da 100 a 9999 hahaha
     //consiero umidita da 0 a 100
    char* s_out_press;
    if(payload.outPress < 1000 ) {
      //xxx.xx 
      s_out_press = new char[7];
      dtostrf(payload.outPress, 3, 2, s_out_press);
    }
    else {
      //xxxx.xx
      s_out_press = new char[8];
      dtostrf(payload.outPress, 4, 2, s_out_press);
    }
     
    sprintf(buffer, "inTemp=%s&inHum=%s&outTemp=%s&outHum=%s&press=%s", s_in_temp, s_in_hum, s_out_temp, s_out_hum, s_out_press);
    ether.browseUrl(PSTR("/arduino.php?"), buffer, website, response_callback);
    delete s_in_temp;
    delete s_in_hum;
    delete s_out_temp;
    delete s_out_hum;
    delete s_out_press;
}

uint8_t inizializeEth() {
  if(Ethernet::isLinkUp()) {
    //inizializza e testa Ethernet
    Serial.print("Setting IP ... ");
      if ( ether.dhcpSetup() ) {
        Serial.println("\t\tsuccess");
      } 
      else {
        if ( ether.staticSetup(ip) ) {
          Serial.println("\t\tsuccess");
        }
        else {
          Serial.println("\t\tfailed");
          return 0;
        }
      }
      Serial.print("Verifica sito web ... ");
      if ( ether.dnsLookup( website ) ) {
        Serial.println("\tsuccess");
        return 1;
      }
      else {
        Serial.println("\tfailed");
        return 0;
      }
    return 1;
  }
  else {
    Serial.println("cavo scollegato");
    return 0;
  }
}
