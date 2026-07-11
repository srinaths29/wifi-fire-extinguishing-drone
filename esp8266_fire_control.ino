#include<ESP8266WiFi.h> 
#include<ESP8266WebServer.h> 
#include<SPI.h> 
#include<Wire.h> 
#include <Adafruit_Sensor.h> 
#include <Adafruit_BMP280.h> 
#include<Adafruit_GFX.h> 
#include<Adafruit_SSD1306.h> 
#include <Servo.h>  
#include<DHTesp.h> 
 
#define SOUND_VELOCITY 0.034 
#define CM_TO_INCH 0.393701 
#define SEALEVELPRESSURE_HPA (1013.25) 
 
 
//--------------------------------------------------------------------- 
 
ESP8266WebServer server(80); 
Adafruit_SSD1306 display(128,64, &Wire,-1); 

Adafruit_BMP280 bmp; 
const char ssid[]="charan_cd"; 
const char password[]="123456789"; 
Servo myservo1; 
DHTesp dht; 
 
 
const int temp=0,flame=14,trig=12 ,echo=13,BUZZ=2; 
//0=d3,14=d5,12=d6,13=d7,2=d4 
const int ledPin = 16;  
int flame_val=0; 
 
long duration; 
float distanceCm; 
float distanceInch; 
float humidity ; 
float temperature; 
float pres = bmp.readPressure()/100; 
float alti = bmp.readAltitude(SEALEVELPRESSURE_HPA);  
int flag1 = 0; 
String flam,ball,obj; 
 
//--------------------------------------------------------------------- 
void setup()  
{ 
int i; 
 
//buzzer is connected to D8 pin of nodemcu 
pinMode(BUZZ,OUTPUT); 
pinMode(flame,INPUT); 
pinMode(trig,OUTPUT); 
pinMode(echo,INPUT); 
dht.setup(0, DHTesp::DHT22); // Connect DHT sensor to GPIO 5 
 
   
 
for(i=0;i<2;i++) 
{ 
digitalWrite(BUZZ,HIGH); 
delay(50); 
digitalWrite(BUZZ,LOW); 
delay(50); 
} 
 
Serial.begin(9600); 
 
  //connect to your local wi-fi network 
  WiFi.begin(ssid, password); 
 
  //check wi-fi is connected to wi-fi network 
 
  while (WiFi.status() != WL_CONNECTED) { 
  delay(1000); 
  Serial.print("."); 
  } 
  Serial.println(""); 
  Serial.println("WiFi connected..!"); 
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP()); 
 
  server.on("/page",page); 
 
  server.begin(); 
   
  Serial.println("HTTP server started"); 
 
myservo1.attach(15);//connect a servo to D8 pin of nodemcu 
myservo1.write(180); 
 
 
//connect the SCK and SDA pins of OLED to D1 and D2 pins of nodemcu respectively 
Wire.begin(); 
display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
 if (!bmp.begin(0x76)) { 
     // connection error or device address wrong! 
     Serial.println(F("Could not find a valid BMP280 sensor, check wiring!")); 
     display.setCursor(34, 23); 
     display.print("Connection"); 
     display.setCursor(49, 33); 
     display.print("Error"); 
     display.display();        // update the display 
     while (1) // stay here 
       delay(1000); 
   } 
 
display.clearDisplay(); 
 
display.setTextSize(3); 
display.setTextColor(WHITE);  
display.setCursor (2,25); 
display.println("WELCOME"); 
display.display(); 
delay(1000); 
display.clearDisplay(); 
 
display.setTextSize(2); 
display.setTextColor(WHITE);  
display.setCursor (46,0); 
display.println("FIRE"); 
display.setCursor(20,25); 
display.println("EXTIGUISH"); 
display.setCursor (17,45); 

 
display.println(" SYSTEM "); 
display.display(); 
delay (1000); 
display.clearDisplay(); 
 
 
 
} 
//--------------------------------------------------------------------- 
 
 
void loop() 
{ 
  server.handleClient(); 
  delay(1000); 
  humidity = dht.getHumidity(); 
  temperature = dht.getTemperature(); 
  pres = bmp.readPressure()/100; 
  alti = bmp.readAltitude(SEALEVELPRESSURE_HPA)-450.00; 
 
  distance(); 
   
  flame_val=digitalRead(flame); 
 
 
 
   
  if(flag1==1) 
  { 
    ball="DEPLOYED"; 
  } 
  else 
  { 
    ball="READY"; 
  } 
 
  if(flame_val==0||temperature>70) 
  { 
    flam="HIGH"; 
    digitalWrite(BUZZ, HIGH); 
    delay(100); 
    digitalWrite(BUZZ, LOW); 
    delay(100); 
  } 
  else 
  { 
    flam="REDUCING"; 
  } 
display.clearDisplay(); 
display.setTextSize(1); 
 
display.setTextColor(WHITE); 
display.setCursor (0,0); 
display.println("TEMPERATURE:"); 
display.print(temperature); 
display.setCursor (0,17); 
display.println("FLAME STATUS:"); 
display.print(flam); 
display.setCursor (0,38); 
display.println("EXTINGUISHER:"); 
display.print(ball); 
display.display(); 
delay(1000); 
 
   
} 
//--------------------------------------------------------------------- 
void distance(){ 
    digitalWrite(trig, LOW); 
  delayMicroseconds(2); 
  // Sets the trigPin on HIGH state for 10 micro seconds 
  digitalWrite(trig, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trig, LOW); 
   
  // Reads the echoPin, returns the sound wave travel time in microseconds 
  duration = pulseIn(echo, HIGH); 
   
  // Calculate the distance 
  distanceCm = duration * SOUND_VELOCITY/2; 
   
  // Convert to inches 
  distanceInch = distanceCm * CM_TO_INCH; 
 
  if(distanceInch<25) 
  { 
    obj="BLOCKED"; 
    digitalWrite(BUZZ,HIGH); 
    delay(100); 
    digitalWrite(BUZZ,LOW ); 
    delay(100); 
  } 
  else 
  { 
    obj="CLEAR"; 
  } 
} 
//--------------------------------------------------------------------- 
 
void page() 
{ 

   
String mycode="<p><font size=\"4\" color=\"#80ced6\">"+String("TEMPERATURE: 
")+"</font><mark>"+ float(temperature)+"</mark><br><font size=\"4\" 
color=\"#80ced6\">"+String("HUMIDITY: ")+"</font><mark>"+ 
float(humidity)+"%</mark><br><font size=\"4\" color=\"#80ced6\">"+String("ALTITUDE: 
")+"</font><mark>"+ float(alti)+"M</mark><br><font size=\"4\" 
color=\"#80ced6\">"+String("PRESSURE: ")+"</font><mark>"+ 
float(pres)+"</mark><br><font size=\"4\" color=\"#80ced6\">"+String("PATHWAY: 
")+"</font><mark>"+ String(obj)+"</mark><br><font size=\"4\" 
color=\"#80ced6\">"+String("FIRE STATUS: ")+"</font><mark>"+ 
String(flam)+"</mark><br><font size=\"4\" color=\"#80ced6\">"+String("EXTINGUISHER: 
")+"</font><mark>"+String(ball)+"</mark><br></p>"; 
mycode= "<html><head><title>fire extinguisher</title></head><body><body 
bgcolor=\"#00008b\"><font size=\"7\" 
color=\"#ADD8E6\"><center><u><b><h1>STATUS</h1></b></u></font><b>"+mycode+"<fo
rm><button type=\"submit\" name=\"state\" value=\"1\">DROP</button><button 
type=\"submit\" name=\"state\" 
value=\"0\">RELOAD</button></form></center></b><meta http-equiv=\"refresh\" 
content=\"2\"></body></html>"; 
server.send(200,"text/html",mycode); 
 
if(server.arg("state")=="1") 
{ 
  myservo1.write(-180); 
  flag1=1; 
} 
if(server.arg("state")=="0") 
{ 
  myservo1.write(180); 
  flag1=0; 
} 
} 