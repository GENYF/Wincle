#include <ESP8266WiFi.h>
#include <DHT.h>  
#include <SDS011.h>

//서버에 전송하기 위한 초기설정
const char* server = "api.thingspeak.com";
String apiKey = "247YATC4VVGEQT0N";
const char* MY_SSID = "GENYF WIFI";//와이파이 이름
const char* MY_PWD = "gnyf0915";//와이파이 비밀번호

SDS011 my_sds;

const int DHTPIN 0     
 
DHT dht(DHTPIN, DHT11);

int sensor = D4;
void gasValue(int val);

// 초기 설정(시리얼 통신, 핀 설정)
void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();

  pinMode(sensor, INPUT);
  pinMode(ledR, OUTPUT);  
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  
//와이파이 연결
  Serial.print("Connecting to " + *MY_SSID);

  WiFi.begin(MY_SSID, MY_PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");

  Serial.print("Measuring ...\n\n");
  delay(2000);
  my_sds.begin(D1,D2);// 핀 설정
}

void loop() {
  int i;
  float S_p25 = 0, S_p10 = 0;
  float p10,p25;
  float S_h = 0, S_t = 0;
  float h = 0, t = 0; 
  int val = 0;

  delay(10000);
  
  for(i = 0; i < 10; i++)
  {
    my_sds.read(&p25,&p10);// 미세먼지 농도 읽어오기
    h = dht.readHumidity();// 습도 읽어오기
    t = dht.readTemperature();// 온도 읽어오기
    val = digitalRead(sensor);
    gasValue(val);
    
    S_p25 += p25;
    S_p10 += p10;
    S_h += h;
    S_t += t;

    Serial.print("val = ");
    Serial.println(val);
    Serial.println("P10:  "+String(p10));
    Serial.println("P2.5: "+String(p25));
    Serial.print("T = ");
    Serial.print(t);
    Serial.println(" C");
    Serial.print("H =  ");
    Serial.print(h);
    Serial.println("%");00000
    Serial.println("\n");
    
    delay(14000);
  }
  
  S_p25 = S_p25 / 10;
  S_p10 = S_p10 / 10;
  S_h = S_h / 10;
  S_t = S_t / 10;

  //Wifi 연결
  WiFiClient client;

  Serial.println("ave_P10:  "+String(S_p10));
  Serial.println("ave_P2.5: "+String(S_p25));

  Serial.print("Temperature = ");
  Serial.print(S_t);
  Serial.println("C");
  
  Serial.print("Humidity =  ");
  Serial.print(S_h);
  Serial.println("%");
 
  if (client.connect(server, 80)) { // 서버에 전송
    Serial.println("WiFi Client connected");

    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(S_p10);
    postStr +="&field2=";
    postStr += String(S_p25);
    postStr +="&field3=";
    postStr += String(S_t);
    postStr +="&field4=";
    postStr += String(S_h);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    delay(3000);
  }
  Serial.println("Waiting...\n");
  delay(2000);// 3분 간격
  client.stop();
}

void gasValue(int val){
  if(val == LOW){
      digitalWrite(ledG, LOW);
      digitalWrite(ledR, HIGH);
  }
    else{
      digitalWrite(ledR, LOW);
      digitalWrite(ledG, HIGH);
  }

  WiFiClient client;
  
  if (client.connect(server, 80)) { // 서버에 전송
    Serial.println("gas Value transmission");

    String postStr = apiKey;
    postStr +="&field5=";
    postStr += String(val);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    delay(1000);
  }
}
