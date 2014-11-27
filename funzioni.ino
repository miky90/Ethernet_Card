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

static word homePage() {
  BufferFiller bfill;
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='10'/>"
    "<title>Network Node</title>"
    "<h1>Information</h1>" 
    "<p>Status: </p>"
    "<p>Ip: $D.$D.$D.$D</p>"
    "<p>Subnet Mask: $D.$D.$D.$D</p>"
    "<p>Gateway: $D.$D.$D.$D</p>"
    "<p>Up Time: $D$D:$D$D:$D$D</p>"),
      ether.myip[0], ether.myip[1], ether.myip[2], ether.myip[3],
      ether.netmask[0], ether.netmask[1], ether.netmask[2], ether.netmask[3], 
      ether.gwip[0], ether.gwip[1], ether.gwip[2], ether.gwip[3],
      h/10, h%10, m/10, m%10, s/10, s%10);
  return bfill.position();
}

static void invioDati() {
    char s_in_temp[7];
    char s_in_hum[7];
    char s_out_temp[7];
    char s_out_hum[7];
    char s_out_press[8];
    Serial.println("invio..");
    Serial.println("Free RAM: ");
    Serial.println(freeRam());
    delay(100);
    //IN
    //consiero temperature da 80 a -40 (99 a -99 ^^)
    if(payload.inTemp < 10 && payload.inTemp >= 0) {
      //x.xx 
      dtostrf(payload.inTemp, 1, 2, s_in_temp);
    }
    else if(payload.inTemp >=10 | payload.inTemp < 0) { 
      //xx.xx e -x.xx 
      dtostrf(payload.inTemp, 2, 2, s_in_temp);
    }
    else {
      //-xx.xx
      dtostrf(payload.inTemp, 3, 2, s_in_temp);
    }
    //consiero umidita da 0 a 100
    if(payload.inHum < 10 ) {
      //x.xx 
      dtostrf(payload.inHum, 1, 2, s_in_hum);
    }
    else if(payload.inHum >=10 ) { 
      //xx.xx 
      dtostrf(payload.inHum, 2, 2, s_in_hum);
    }
    else {
      //100.00
      dtostrf(payload.inHum, 3, 2, s_in_hum);
    }
    //OUT
    //consiero temperature da 80 a -40 (99 a -99 ^^)
    
    if(payload.outTemp < 10 && payload.outTemp >= 0) {
      //x.xx 
      dtostrf(payload.outTemp, 1, 2, s_out_temp);
    }
    else if(payload.outTemp >=10 | payload.outTemp < 0) { 
      //xx.xx e -x.xx 
      dtostrf(payload.outTemp, 2, 2, s_out_temp);
    }
    else {
      //-xx.xx
      dtostrf(payload.outTemp, 3, 2, s_out_temp);
    }
    //consiero umidita da 0 a 100
    if(payload.outHum < 10 ) {
      //x.xx 
      dtostrf(payload.outHum, 1, 2, s_out_hum);
    }
    else if(payload.outHum >=10 ) { 
      //xx.xx 
      dtostrf(payload.outHum, 2, 2, s_out_hum);
    }
    else {
      //100.00
      dtostrf(payload.outHum, 3, 2, s_out_hum);
    }
    //PRESSIONE
    //considero pressioni da 100 a 9999 hahaha
     //consiero umidita da 0 a 100
    if(payload.outPress < 1000 ) {
      //xxx.xx 
      dtostrf(payload.outPress, 3, 2, s_out_press);
    }
    else {
      //xxxx.xx
      dtostrf(payload.outPress, 4, 2, s_out_press);
    }
     
    sprintf(buffer, "inTemp=%s&inHum=%s&outTemp=%s&outHum=%s&press=%s", s_in_temp, s_in_hum, s_out_temp, s_out_hum, s_out_press);
    ether.browseUrl(PSTR("/arduino.php?"), buffer, website, response_callback);
}
int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
