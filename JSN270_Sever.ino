#include<Debug.h>
#include <JSN270.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <Servo.h>

#define SSID      "iptime 2"      // AP 이름
#define KEY       "qazwsx1234"      // AP 비밀번호
#define AUTH       "WPA2"       // your wifi network security (NONE, WEP, WPA, WPA2)
#define DHTPIN 13
#define DHTTYPE DHT11

#define USE_DHCP_IP 1

#if !USE_DHCP_IP
#define MY_IP          "와이파이쉴드(아두이노) IP" 
#define SUBNET         "255.255.255.0"
#define GATEWAY        "192.168.0.1"
#endif

#define HOST_IP        "클라이언트 호스트 IP"
#define REMOTE_PORT    7090 //port
#define PROTOCOL       "TCP"

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial mySerial(3, 2); // RX, TX
 
JSN270 JSN270(&mySerial);
Servo servo;

int chk;
float hum;  
float temp;
int servoPin = 9;
String hum1;
String temp1;
void setup() {
   char c;
   String hostname;
   char hostip[32];
  servo.attach(servoPin);

   mySerial.begin(9600);
   Serial.begin(9600);
  dht.begin();
   //Serial.println("--------- JSN270 Web Client Test --------");

   // wait for initilization of JSN270
   delay(5000);
   //JSN270.reset();
   delay(1000);

   //JSN270.prompt();
   JSN270.sendCommand("at+ver\r");
   delay(5);
   while(JSN270.receive((uint8_t *)&c, 1, 1000) > 0) {
      //Serial.print((char)c);
   }
   delay(1000);

#if USE_DHCP_IP
   JSN270.dynamicIP();
#else
   JSN270.staticIP(MY_IP, SUBNET, GATEWAY);
#endif    
    
   if (JSN270.join(SSID, KEY, AUTH)) {
      //Serial.println("WiFi connect to " SSID);
   }
   else {
      //Serial.println("Failed WiFi connect to " SSID);
      //Serial.println("Restart System");

      return;
   }
   delay(1000);

   JSN270.sendCommand("at+wstat\r");
   delay(5);
   while(JSN270.receive((uint8_t *)&c, 1, 1000) > 0) {
      //Serial.print((char)c);
   }
   delay(1000);        

   JSN270.sendCommand("at+nstat\r");
   delay(5);
   while(JSN270.receive((uint8_t *)&c, 1, 1000) > 0) {
      //Serial.print((char)c);
   }
   delay(1000);

   JSN270.sendCommand("at+nslookup=192.168.0.4\r");
   delay(5);
   while(JSN270.receive((uint8_t *)&c, 1, 1000) > 0) {
      if (c == '[') {
         break; 
      }
      else if ((c != '\r') && (c != '\n')) {
         if (c >= 32) {   // discard non-printable character
            hostname += c;
         }
      }
   }
   delay(1000);
   
   //Serial.print("Host IP is ");
   //Serial.println(hostname);
   hostname.toCharArray(hostip, hostname.length()+1);   // convert string to char array

   if (!JSN270.client(hostip, REMOTE_PORT, PROTOCOL)) {
      //Serial.print("Failed connect to ");
      //Serial.println(hostip);
      //Serial.println("Restart System");
   } else {
      //Serial.print("Socket connect to ");
      //Serial.println(hostip);
      //delay(2000);
      
      // Enter data mode
      JSN270.sendCommand("at+exit\r");
      delay(100);

      //Serial.println("connected to server");
      // Make a HTTP request:
      JSN270.println("GET /search?q=JSN270 HTTP/1.1");
      JSN270.println("Host: www.google.com");
      JSN270.println("Connection: close");
      JSN270.println();
   }
}

void loop() {
    hum =dht.readHumidity();
    temp= dht.readTemperature();
    int ht[]={hum,temp};
    hum1 = String(ht[0]);
    temp1 = String(ht[1]);
    int angle=temp;

   if(JSN270.available()) {
  //  JSN270.print(hum);
    //JSN270.print(",");
   // JSN270.print(temp);
    delay(2000); 

      Serial.print((char)JSN270.read());
   }
     Serial.println(hum);
     Serial.println(temp);
    JSN270.print(hum);
    JSN270.print(",");
   JSN270.print(temp);
   
   if((angle >= 0) &&( angle <10)){
  servo.write(50);
   }
   else if((angle >= 22) && (angle <25)){
    servo.write(70);
   }
   else if((angle >= 25) &&( angle <27)){
    servo.write(90);
   }
   else if((angle >= 27) &&( angle <30)){
    servo.write(130);
   }
   
    delay(2000);
}
