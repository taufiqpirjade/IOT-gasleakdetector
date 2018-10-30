#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(2,3);      // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin
float sensor=A0;
float gas_value;
String apiKey = "YZEP4Y9EB4BG3AP7";     // replace with your channel's thingspeak WRITE API key
String ssid="YOURWIFINAME";    // Wifi network SSID
String password ="YOURWIFIPASSWORD";  // Wifi network password
boolean DEBUG=true;
#define VOLTAGE_MAX 5.0
#define VOLTAGE_MAXCOUNTS 1023.0
//======================================================================== showResponse
void showResponse(int waitTime){
long t=millis();
char c;
while (t+waitTime>millis()){
if (espSerial.available()){
c=espSerial.read();
if (DEBUG) Serial.print(c);
}
}
}
//=======================connection to thinkspeak.com=================================================
boolean thingSpeakWrite(float value1){
String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
cmd += "184.106.153.149";                               //    184.106.153.149
cmd += "\",80";
espSerial.println(cmd);
if (DEBUG) Serial.println(cmd);
String getStr = "GET /update?api_key=";   // prepare GET string
getStr += apiKey;
getStr +="&field1=";
getStr += String(value1);
//getStr +="&field2=";
//getStr += String(value2);
// ...
getStr += "\r\n";
// send data length
cmd = "AT+CIPSEND=";
cmd += String(getStr.length());
espSerial.println(cmd);
Serial.println(DEBUG);
espSerial.println(getStr);
if (DEBUG)  Serial.println(cmd);
delay(100);
return true;
}
//================================================================================ setup
void setup() {                
DEBUG=true;           // enable debug serial
//----------my sensor code-----------
pinMode(LED_BUILTIN, OUTPUT);
pinMode(sensor,INPUT);
Serial.begin(9600); 
espSerial.begin(115200);  // enable software serial
// Your esp8266 module's speed is probably at 115200. 
// For this reason the first time set the speed to 115200 or to your esp8266 configured speed 
// and upload. Then change to 9600 and upload again
//espSerial.println("AT+RST");         // Enable this line to reset the module;
//showResponse(1000);
//espSerial.println("AT+UART_CUR=9600,8,1,0,0");    // Enable this line to set esp8266 serial speed to 9600 bps
//showResponse(1000);
espSerial.println("AT+CWMODE=1");   // set esp8266 as client
showResponse(1000);
espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
showResponse(5000);
if (DEBUG)  Serial.println("Setup completed");
}
// ======================================================================= loop
void loop() {
//---------my gas sensor value reading-------------------
gas_value=analogRead(sensor);
Serial.println(gas_value);
delay(1);  //delay in between reads for stability 
if(gas_value>250){
// initialize digital pin LED_BUILTIN as an output.
digitalWrite(LED_BUILTIN, HIGH);
//delay(250);
// digitalWrite(LED_BUILTIN, LOW);
//delay(250);
float t = gas_value*(VOLTAGE_MAX / VOLTAGE_MAXCOUNTS); //convert gas value to voltages
if (isnan(t)) {
if (DEBUG) Serial.println("Failed to read from MQ5");
}
else{
if (DEBUG)  Serial.println("Voltage="+String(t));
thingSpeakWrite(t);             // Write values to thingspeak
}
// thingspeak needs 15 sec delay between updates, */    
delay(20000);  
}
else{
digitalWrite(LED_BUILTIN, LOW);
}
}

