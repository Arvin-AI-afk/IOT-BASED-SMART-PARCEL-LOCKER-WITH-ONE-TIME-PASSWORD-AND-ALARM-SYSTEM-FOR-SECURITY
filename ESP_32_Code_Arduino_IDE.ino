#include <WiFi.h> 
#include <HTTPClient.h> 
#include <WebServer.h> 
#include <EEPROM.h> 
#include <Keypad.h> 
#include <LiquidCrystal_I2C.h> 
const int ROW_NUM    = 2; //four rows 
const int COLUMN_NUM = 4; //four columns 
char keys[ROW_NUM][COLUMN_NUM] = { 
{'1','2','3', '4'}, 
{'5','E','C', 'B'} 
}; 
byte pin_rows[ROW_NUM] = {17,16}; //connect to the row pinouts of the keypad 
byte pin_column[COLUMN_NUM] = {4, 0, 2, 15}; //connect to the column pinouts of the keypad 
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, 
COLUMN_NUM ); 
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows 
String input_password; 
//Variables 
int i = 0; 
int statusCode; 
const char* ssid = "Default SSID"; 
const char* passphrase = "Default passord"; 
String st; 
String content; 
String esid; 
String epass = ""; 
//Function Decalration 
bool testWifi(void); 
void launchWeb(void); 
void setupAP(void); 
int butt =19; 
//Establishing Local server at port 80 
WebServer server(80); 
//String GOOGLE_SCRIPT_ID = 
"AKfycbywnnpJE2Qo4DXWpA2lAPcRhi86lldxIZU8IAFGkLO6OQzaSpKq"; 
String GOOGLE_SCRIPT_ID = "AKfycbzfQYB0yFBgTryafZGFHEEesXVzu47CHDLkT
i5GeKOkvL0vDTV3Zrte2j2zecQZHEJAA"; 
//--------------------------------------------------------------------- 
int reed =18; 
const int Red = 12; //red LED is on pin 4 
const int greenLed = 13; //green LED is pin 12 
String String1,String2, String3, String4; 
String String11, String22,String33,String44; 
 
WiFiClientSecure client; 
 
unsigned long prevTime_T1 = millis(); 
unsigned long prevTime_T2 = millis(); 
unsigned long prevTime_T3 = millis(); 
 
long interval_T1 = 20000; 
int ledState = LOW; 
int ledState2 = LOW; 
 
int incorrect = 0; 
int extend = 14; 
int retract = 27; 
int flag=0; 
int flag2=0; 
int door = 26; 
int speaker = 25; 
 
void setup() 
{ 
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor 
  Serial.println(); 
  Serial.println("Disconnecting current wifi connection"); 
  lcd.begin();             
  lcd.backlight(); 
  digitalWrite(door, LOW); 
  digitalWrite(speaker, LOW); 
   lcd.clear(); 
   lcd.setCursor(0, 0); 
   lcd.print("Disconnecting"); 
   lcd.setCursor(0, 1); 
   lcd.print("current wifi..."); 
  WiFi.disconnect(); 
  EEPROM.begin(512); //Initialasing EEPROM 
  delay(10); 
  Serial.println(); 
  Serial.println(); 
  Serial.println("Startup"); 
   lcd.clear(); 
   lcd.setCursor(0, 0); 
   lcd.print("Startup"); 
 
  //---------------------------------------- Read eeprom for ssid and pass 
  Serial.println("Reading EEPROM ssid"); 
   lcd.clear(); 
   lcd.setCursor(0, 0); 
   lcd.print("Reading"); 
   lcd.setCursor(0, 1); 
   lcd.print("EEPROM ssid");
for (int i = 0; i < 32; ++i) 
  { 
    esid += char(EEPROM.read(i)); 
  } 
  Serial.println(); 
  Serial.print("SSID: "); 
  Serial.println(esid); 
  Serial.println("Reading EEPROM pass"); 
   lcd.clear(); 
   lcd.setCursor(0, 0); 
   lcd.print("Reading"); 
   lcd.setCursor(0, 1); 
   lcd.print("EEPROM pass"); 
 
  for (int i = 32; i < 96; ++i) 
  { 
    epass += char(EEPROM.read(i)); 
  } 
  Serial.print("PASS: "); 
  Serial.println(epass); 
 
pinMode(butt,INPUT_PULLUP); 
pinMode(reed,INPUT_PULLUP); 
 
  WiFi.begin(esid.c_str(), epass.c_str()); 
   lcd.clear(); 
   lcd.setCursor(0, 0); 
   lcd.print("CONNECTED!"); 
   delay(200); 
 
  pinMode(Red, OUTPUT); //the red LED is an output 
  pinMode(greenLed, OUTPUT); // the green LED is an output 
  pinMode(extend, OUTPUT); 
  pinMode(retract, OUTPUT); 
 
  pinMode(door, OUTPUT); 
  pinMode(speaker, OUTPUT); 
 
  digitalWrite(Red, LOW); //turn the red LED on 
 
  input_password.reserve(32); 
 
  digitalWrite(extend, LOW); 
  digitalWrite(retract, LOW); 
 
   lcd.clear(); 
   lcd.setCursor(0, 0); 
   lcd.print("Please enter"); 
   lcd.setCursor(0, 1); 
   lcd.print("  the OTP code:"); 
} 
void loop() { 
    
if(digitalRead(butt) == LOW) 
  {
Serial.println("Connection Status Negative / D15 HIGH"); 
    Serial.println("Turning the HotSpot On"); 
     lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print("Connecting..."); 
    lcd.setCursor(0, 1); 
    lcd.print("HotSpot On"); 
    launchWeb(); 
    setupAP();// Setup HotSpot 
  } 
  else if ((WiFi.status() != WL_CONNECTED)) 
  { 
    Serial.print("."); 
    server.handleClient(); 
    delay(100); 
  } 
 
unsigned long currentTime = millis(); 
char key = keypad.getKey(); 
if (key){ 
    Serial.println(key); 
 
    if(key == 'C') { 
      input_password = ""; // reset the input password 
      lcd.clear(); 
      lcd.setCursor(0, 0); 
      lcd.print("Please enter"); 
      lcd.setCursor(0, 1); 
      lcd.print("  the OTP code:"); 
    } 
    else if(key == 'E') { 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("Loading..."); 
         HTTPClient http; 
   String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?read"; 
  
  http.begin(url.c_str()); 
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
  int httpCode = http.GET(); 
  if(httpCode <= 0){http.end(); return;} 
  //reading data comming from Google Sheet 
  String payload = http.getString(); 
  if(httpCode == 200){ 
    payload.toLowerCase(); 
    payload.trim(); 
    String1=payload.substring(payload.indexOf('a')+1,payload.indexOf('b')); 
    String11=payload.substring(payload.indexOf('*')+1,payload.indexOf('a')); 
    Serial.println("Relay1 : "+String1); 
    Serial.println("Relay11 : "+String11); 
  } 
  http.end(); 
      lcd.clear(); 
      if(input_password == String11) { 
        Serial.println("password is correct"); 
        lcd.setCursor(0, 0);
lcd.print("ACCESS"); 
        lcd.setCursor(0, 1); 
        lcd.print("GRANTED"); 
        delay(2000); 
 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("Opening..."); 
        lcd.setCursor(0, 1); 
        lcd.print("  Please wait!"); 
        digitalWrite(greenLed, HIGH); 
        digitalWrite(retract, HIGH); 
        digitalWrite(extend, LOW); 
        delay(6000); 
 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("Please place the"); 
        lcd.setCursor(0, 1); 
        lcd.print("packages inside"); 
        delay(3000); 
 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("the designated"); 
        lcd.setCursor(0, 1); 
        lcd.print("box and wait"); 
        delay(3000); 
 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("until the box"); 
        lcd.setCursor(0, 1); 
        lcd.print("is closed."); 
        delay(11000); 
 
        digitalWrite(greenLed, LOW); 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("Closing..."); 
        lcd.setCursor(0, 1); 
        lcd.print("Please wait!!"); 
        digitalWrite(Red, HIGH); 
        digitalWrite(extend, HIGH); 
        digitalWrite(retract, LOW); 
        delay(8000); 
        digitalWrite(extend, LOW); 
        digitalWrite(Red, LOW); 
 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("The door has"); 
        lcd.setCursor(0, 1); 
        lcd.print("been closed."); 
 
        String param;
param = "code5="+String(5); 
        write_to_google_sheet(param); 
    
        delay(2000); 
 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("Please enter"); 
        lcd.setCursor(0, 1); 
        lcd.print("  the OTP code:"); 
      } 
           else if(input_password == String1) { 
        Serial.println("DOOR UNLOCK 2"); 
        lcd.setCursor(0, 0); 
        lcd.print("CORRECT!"); 
        lcd.setCursor(0, 1); 
        lcd.print("DOOR UNLOCKED!"); 
 
        flag=1; 
        digitalWrite(door, HIGH); 
        digitalWrite(Red, HIGH); 
        delay(5000); 
        digitalWrite(Red, LOW); 
        digitalWrite(door, LOW); 
 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("Please enter"); 
        lcd.setCursor(0, 1); 
        lcd.print("  the OTP code:"); 
      } 
 
       else { 
        Serial.println("password is incorrect, try again"); 
        lcd.setCursor(0, 0); 
        lcd.print("INCORRECT!"); 
        lcd.setCursor(0, 1); 
        lcd.print("ACCESS DENIED!"); 
 
        incorrect ++; 
        Serial.println(incorrect); 
 
        digitalWrite(Red, HIGH); 
        delay(2000); 
        digitalWrite(Red, LOW); 
 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("Please enter"); 
        lcd.setCursor(0, 1); 
        lcd.print("  the OTP code:"); 
      } 
 
      input_password = ""; // reset the input password 
    } 
    else {
if(input_password.length() == 0) { 
        lcd.clear(); 
      } 
 
      input_password += key; // append new character to input password string 
      lcd.setCursor(input_password.length(), 0); // move cursor to new position 
      lcd.print(key);                 // print * key as hiden character 
    } 
  } 
  
  if (incorrect == 3){ 
     lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("THREE FAILED"); 
        lcd.setCursor(0, 1); 
        lcd.print("ATTEMPT!!!"); 
     String param; 
    param = "code4="+String(4); 
    write_to_google_sheet(param); 
    incorrect =0; 
  } 
  
  if (digitalRead(reed)==0 && flag == 1){ 
  Serial.println("NO Alarm"); 
  digitalWrite(speaker, LOW); 
  
  flag2 =1; 
  } 
  if (digitalRead(reed)==1 && flag2 == 1){ 
  flag =0; 
  flag2 =0; 
  } 
  if (digitalRead(reed)==0 && flag2 == 0){ 
  Serial.println("Alarm"); 
  digitalWrite(speaker, HIGH); 
   String param; 
    param = "code3="+String(3); 
    write_to_google_sheet(param); 
    Serial.println("TRIGGER ALARM"); 
  
  } 
  if (digitalRead(reed)==1 && flag == 0 && flag2 ==0){ 
  Serial.println("NO Alarm"); 
  digitalWrite(speaker, LOW); 
  
  } 
 
/* 
  if(digitalRead(reed) ==HIGH){ 
    String param; 
    param = "code3="+String(3); 
    write_to_google_sheet(param); 
    Serial.println("TRIGGER ALARM"); 
  } 
*/
} 
 
void launchWeb() 
{ 
  Serial.println(""); 
  if (WiFi.status() == WL_CONNECTED) 
    Serial.println("WiFi connected"); 
  Serial.print("Local IP: "); 
  Serial.println(WiFi.localIP()); 
  Serial.print("SoftAP IP: "); 
  Serial.println(WiFi.softAPIP()); 
  createWebServer(); 
  // Start the server 
  server.begin(); 
  Serial.println("Server started"); 
   lcd.clear(); 
   lcd.setCursor(0, 0); 
   lcd.print("Server started"); 
    
} 
 
void setupAP(void) 
{ 
  WiFi.mode(WIFI_STA); 
  WiFi.disconnect(); 
  delay(100); 
  int n = WiFi.scanNetworks(); 
  Serial.println("scan done"); 
   lcd.clear(); 
   lcd.setCursor(0, 0); 
   lcd.print("Scan done"); 
  if (n == 0){ 
    Serial.println("no networks found"); 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print("No networks found"); 
  } 
  else 
  { 
    Serial.print(n); 
    Serial.println(" networks found"); 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print(n); 
    lcd.setCursor(0, 1); 
    lcd.print("Networks found"); 
    for (int i = 0; i < n; ++i) 
    { 
      // Print SSID and RSSI for each network found 
      Serial.print(i + 1); 
      Serial.print(": "); 
      Serial.print(WiFi.SSID(i)); 
      Serial.print(" ("); 
      Serial.print(WiFi.RSSI(i)); 
      Serial.print(")");
//Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*"); 
      delay(10); 
    } 
  } 
  Serial.println(""); 
  st = "<ol>"; 
  for (int i = 0; i < n; ++i) 
  { 
    // Print SSID and RSSI for each network found 
    st += "<li>"; 
    st += WiFi.SSID(i); 
    st += " ("; 
    st += WiFi.RSSI(i); 
 
    st += ")"; 
    //st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*"; 
    st += "</li>"; 
  } 
  st += "</ol>"; 
  delay(100); 
  WiFi.softAP("ElectronicsInnovation", ""); 
  Serial.println("Initializing_softap_for_wifi credentials_modification"); 
  launchWeb(); 
  Serial.println("over"); 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print("Over"); 
 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print("SoftAP IP:"); 
    lcd.setCursor(0, 1); 
    lcd.print(WiFi.softAPIP()); 
 
} 
 
void createWebServer() 
{ 
  { 
    server.on("/", []() { 
 
      IPAddress ip = WiFi.softAPIP(); 
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]); 
      content = "<!DOCTYPE HTML>\r\n<html>Welcome to Wifi Credentials Update page"; 
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" 
value=\"scan\"></form>"; 
      content += ipStr; 
      content += "<p>"; 
      content += st; 
      content += "</p><form method='get' action='setting'><label>SSID: </label><input 
name='ssid' length=32><input name='pass' length=64><input type='submit'></form>"; 
      content += "</html>"; 
      server.send(200, "text/html", content); 
    }); 
    server.on("/scan", []() { 
      //setupAP();
IPAddress ip = WiFi.softAPIP(); 
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]); 
 
      content = "<!DOCTYPE HTML>\r\n<html>go back"; 
      server.send(200, "text/html", content); 
    }); 
 
    server.on("/setting", []() { 
      String qsid = server.arg("ssid"); 
      String qpass = server.arg("pass"); 
      if (qsid.length() > 0 && qpass.length() > 0) { 
        Serial.println("clearing eeprom"); 
        for (int i = 0; i < 96; ++i) { 
          EEPROM.write(i, 0); 
        } 
        Serial.println(qsid); 
        Serial.println(""); 
        Serial.println(qpass); 
        Serial.println(""); 
 
        Serial.println("writing eeprom ssid:"); 
        for (int i = 0; i < qsid.length(); ++i) 
        { 
          EEPROM.write(i, qsid[i]); 
          Serial.print("Wrote: "); 
          Serial.println(qsid[i]); 
        } 
        Serial.println("writing eeprom pass:"); 
        for (int i = 0; i < qpass.length(); ++i) 
        { 
          EEPROM.write(32 + i, qpass[i]); 
          Serial.print("Wrote: "); 
          Serial.println(qpass[i]); 
        } 
        EEPROM.commit(); 
 
       // content = "{\"Success\" "saved to eeprom... reset to boot into new wifi\"}"; 
        statusCode = 200; 
        ESP.restart(); 
      } 
      else { 
        //content = "{\"Error\" "404 not found\"}"; 
        statusCode = 404; 
        Serial.println("Sending 404"); 
      } 
      server.sendHeader("Access-Control-Allow-Origin", "*"); 
      server.send(statusCode, "application/json", content); 
 
    }); 
  } 
} 
 
 
void write_to_google_sheet(String params) { 
   HTTPClient http;
String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+params; 
   //Serial.print(url); 
    Serial.println("Postring GPS data to Google Sheet"); 
    //--------------------------------------------------------------------- 
    //starts posting data to google sheet 
    http.begin(url.c_str()); 
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
    int httpCode = http.GET();   
    Serial.print("HTTP Status Code: "); 
    Serial.println(httpCode); 
    //--------------------------------------------------------------------- 
    //getting response from google sheet 
    
    http.end(); 
}

