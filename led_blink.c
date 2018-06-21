#include<ESP8266WiFi.h>
#include<ESP8266mDNS.h> 
const char* WiFiSSID = "keshav";
const char* WiFiPSK = "atjwtjs5";
 
const int LED_PIN = 16; // GPIO13
const int ANALOG_PIN =A0;
const int DIGITAL_PIN=12;
WiFiServer server(80);

void connectWifi()
{
  byte ledStatus=LOW;
  Serial.println();
  Serial.println("Connecting to: "+String(WiFiSSID));
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFiSSID,WiFiPSK);
  while(WiFi.status()!=WL_CONNECTED)
  {
    digitalWrite(LED_PIN,ledStatus);
    ledStatus=(ledStatus==HIGH)?LOW:HIGH;
    delay(100);
  }
  Serial.println("WiFi Connected\nIP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(16,LOW);
}
void setupMDNS()
{
  if(!MDNS.begin("thing"))
  {
    Serial.println("Error setting up MDNS responder!");
    while(1)
    delay(1000);
  }
  Serial.println("mDNS responder started");
}
void initHardware()
{
  
  pinMode(DIGITAL_PIN,INPUT_PULLUP);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW);
}  
void setup() {
  Serial.begin(115200);
  initHardware();
  connectWifi();
  server.begin();
  setupMDNS();
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int val = -1;
  if (request.indexOf("/led/0") != -1)  
    val=1;
  else if(request.indexOf("/led/1")!=-1)
  val=0;
  else if(request.indexOf("/read")!=-1)
  val=-2;
  if(val>=0)
  digitalWrite(LED_PIN,val);
  client.flush();
  String s="HTTP/1.1 200 OK\r\n";
  s+="Content-Type: text/html\r\n\r\n";
  s+="<!DOCTYPE HTML>\r\n<html>\r\n";
  if(val>=0)
  {
    s+="LED is now ";
    s+=(val)?"off":"on";
  } 
  else if(val==-2)
  {
    s+="Analog Pin = ";
    s+=String(analogRead(ANALOG_PIN));
    s+="<br>";
    s+="Digital Pin 12 = ";
    s+=String(digitalRead(DIGITAL_PIN));
  }
  else
  {
    s+="Invalid Request,<br> Try /led/1, /led/0, or /read.";
  }
  s+="</html>\n";
  client.print(s);
  delay(1);
  Serial.println("Client disconnected");
}
