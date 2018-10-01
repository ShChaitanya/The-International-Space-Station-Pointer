#include <ESP8266WiFi.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <stdlib.h>
#include <ESP8266HTTPClient.h>
#include<math.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

char auth[] = "blynk token";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "your-wifi-name";
char pass[] = "pass";


SoftwareSerial mySerial(D2,D3);
String  httpurl;
String  TheHiddenAnswerOfClient;
HTTPClient http;

String SendWithAnswer(String IPcache, String monmessagecache) {
httpurl = "http://";
httpurl+=IPcache;
httpurl+="/";
httpurl+=monmessagecache;
http.begin(httpurl);
http.GET();
TheHiddenAnswerOfClient = (http.getString());
http.end();
return TheHiddenAnswerOfClient;
}

DynamicJsonBuffer jsonBuffer(3000);
int parsing_result (JsonObject& monparam){
if (!monparam.success())return 0;
else
return 1;
}

void conect(){
  WiFi.disconnect();
  delay(3000);
  Serial.println("Start");
   WiFi.begin("chetan sharma","chetan2018");
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("...");

  }
  Serial.println("Connected");
  Serial.println("Your IP is:");
  Serial.println((WiFi.localIP().toString()));
  delay(3000);
  JsonObject& myobject = jsonBuffer.parseObject((SendWithAnswer("api.open-notify.org","iss-now.json")));
  if (1 == (parsing_result(myobject))) {
    Serial.println("Success");
    Serial.print("Latitude : ");
    Serial.println(myobject["iss_position"]["latitude"].as<char*>());
    Serial.print("Longitude : ");
    Serial.println(myobject["iss_position"]["longitude"].as<char*>());

  } else {
    Serial.println("Not Success");

  }
}

void calculate(float Lss,float lss) {
    float re = 6378 ;         //radius of Earth in Km
    float rs = 6778 ;        //radius of satellite orbit in Km

    const float pi_to_rad =  57.29578 ;

    //scanf("SSP ( %d , %d )\n",&Ls,&ls);
    //scanf("EP  ( %d , %d )\n",&Le,&le);
    float newLss = Lss/pi_to_rad;
    float newlss = lss/pi_to_rad;
    float Lee = 12.8531/pi_to_rad;
    float lee = 80.0684/pi_to_rad;
    float ya = acos((sin(newLss)*sin(Lee))+(cos(newLss)*cos(Lee)*cos(newlss-lee)));
    float r = re/rs ;
    float squ = sqrt ( 1 + (r*r) - (2*r*cos(ya)) );
    float ele = acos(sin(ya)/squ);
    float lsle = fabs(lee-newlss);
    float azimuth = asin( sin(lsle) * ( cos(newLss) / sin(ya) ) );
    float a = azimuth*pi_to_rad;
    if(a<0){
      a=a*(-1);
    }
    Serial.print("Azimuth Before :        ");
    Serial.println(a);
    if(Lss<=0 && lss<=0){
        a=180+a;
    }else if(Lss<=0 && lss>=0){
        a=180-a;
    }else if(Lss>=0 && lss<=0){
        a=360-a;
    }else if(Lss>=0 && lss>=0){
      if(lss<=80.0684){
        a=360-a;
      }else{
        a=a;
      }
    }
    float e = ele*pi_to_rad;
    float y = ya*pi_to_rad;
    float check = acos(re/rs)*pi_to_rad;
    float turn;
    if(y <= check){
      turn = 90-e;
    }else{
      turn = 90+e ;
      }
    Serial.print("Elivation is :");
    Serial.println(e);
    Serial.print("Azimuth is :");
    Serial.println(round(a));
    Serial.print("Gama");
    Serial.println(y);
    Serial.print("Turn : ");
    Serial.println(round(turn));
    delay(10);
    mySerial.print(round(a));            //send the two speeds
    mySerial.print(",");
    mySerial.print(round(turn));
    mySerial.print('\n');
}

void setup()
{
  Serial.begin(9600);
  conect();
  mySerial.begin(4800);
  mySerial.flush();
  WiFi.disconnect();
  delay(3000);
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
  if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    conect();
  }
  JsonObject& myobject = jsonBuffer.parseObject((SendWithAnswer("api.open-notify.org","iss-now.json")));
  if (1 == (parsing_result(myobject))) {
    Serial.println("Success");
    
    Serial.print("Latitude : ");
    float a = myobject["iss_position"]["latitude"].as<double>();
    Serial.println(a);
    
    Serial.print("Longitude : ");
    float b = myobject["iss_position"]["longitude"].as<double>();
    Serial.println(b);

    calculate(a,b);
    
  } else {
    Serial.println("Not Success");
    ESP.restart();
  }
  delay(3000);
}
