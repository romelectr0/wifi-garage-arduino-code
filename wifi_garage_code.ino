
#include <EtherCard.h>
int closedelay; //le delais de fermeture
int opendelay; // le delais d'ouverture
boolean door;
int button = 5;//la pin du boutton
int openpin = 9;//la pin de la sortie du signal pour ouvrir la porte
int closepin = 7;//la pin de la sortie du signal pour fermer la porte
int stoped = 4;
int stopstate = digitalRead(stoped);
int buttonstate = digitalRead(button);
#define STATIC 1
#if STATIC
static byte myip[] = { 192,168,1,200 };//ip du systeme en local
static byte gwip[] = { 192,168,1,1 };//ip du routeur en local
#endif

static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };//adresse mac

byte Ethernet::buffer[500];
char pageopen[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
  "<head><title>"
  "</title><style type='text/css'>body {background-color: black;}#button{margin-top: 300px;margin-left: 700px;color: white;background-color: blue;border-radius: 10px;width: 100px;}</style></head>"
  "<body>"
      "<a href='http://192.168.1.200/?door=open'><input id='button' type='submit' color='red' value='open'/></a>"
      "<a herf='http://192.168.1.200/?door=close'><input id='button' type='submit' value='close'/></a>"
    "</em></p>"
  "</body>"
"</html>"
;


char pageclose[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
  "<head><title>"
  "</title><style type='text/css'>body {background-color: black;}#button{margin-top: 300px;margin-left: 700px;color: white;background-color: blue;border-radius: 10px;width: 100px;}</style></head>"
  "<body>"
      "<a href='http://192.168.1.200/?door=open'><input id='button' type='submit' value='open'/></a>"
      "<a herf='http://192.168.1.200/?door=close'><input id='button' type='submit' color='red' value='close'/></a>"
    "</em></p>"
  "</body>"
"</html>"
;//la page html

void setup(){
  door = false;
  Serial.begin(9600);
  pinMode(button,INPUT);
  pinMode(closepin,OUTPUT);
  pinMode(openpin,OUTPUT);
  Serial.println("\n[backSoon]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
#endif
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip); 
  
}
boolean manualdoor () {
  if (buttonstate == LOW) {
   if (buttonstate == HIGH) {
    if (door) {
      digitalWrite(closepin,HIGH);
      delay(closedelay);
      digitalWrite(closepin,LOW);
      return false;
    }
    else if (!(door)) {
      digitalWrite(openpin,HIGH);
      delay(opendelay);
      digitalWrite(openpin,LOW);
      return true;
    }
   }
   }
}

void loop(){
  while(true) {
    word len = ether.packetReceive();
    word pos = ether.packetLoop(len);
     if (!(door)) {
       if (ether.packetLoop(ether.packetReceive())) {
        memcpy_P(ether.tcpOffset(), pageclose, sizeof pageclose);
        ether.httpServerReply(sizeof pageclose - 1);
       }
      }
      if (door) {
   if(ether.packetLoop(ether.packetReceive())) {
     memcpy_P(ether.tcpOffset(),pageopen,sizeof pageopen);
     ether.httpServerReply(sizeof pageopen - 1);
   }
 }
  door = manualdoor();
  if(strstr((char *)Ethernet::buffer + pos, "GET /?door=open") != 0) { // la requette get pour ouvrir la porte
      if (!(door)) {//on verifie si la porte était fermer
        digitalWrite(openpin,HIGH);
        delay(opendelay);
        digitalWrite(openpin,LOW);
      }
  }
  if(strstr((char *)Ethernet::buffer + pos,"GET /?door=close") != 0) {//et la requette get pour la fermer
    if (door) {//on verifie si la porte était ouverte
      digitalWrite(closepin,HIGH);
      delay(closedelay);
      digitalWrite(closepin,LOW);
    }
  }
  if (stopstate == HIGH) {
    digitalWrite(closepin,HIGH);
    delay(closedelay);
    digitalWrite(closepin,LOW);
  }
  }
  
}
