#define BLYNK_TEMPLATE_ID "TMPL618OqrTXg"
#define BLYNK_TEMPLATE_NAME "PBL3demo"
#define BLYNK_AUTH_TOKEN "4Sly-C35Dvbbi8FWkc9mmCpMfGfK0NJl"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <BlynkSimpleEsp32.h>
#include <math.h>
#include <string.h>

// WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN;
const char* ssid = "NHATRO BM T1";
const char* password = "nhatro123456t1";

// // MQTT Broker
// const char* mqtt_server = "fb14e4be0db84d6080ca3ca4c737d73d.s1.eu.hivemq.cloud";
// const int mqtt_port = 8883;
// const char* mqtt_user = "thanhpm175";
// const char* mqtt_password = "17052003t";

// DHT Sensor
// #define DHTPIN 4
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// DS18B20 Sensor
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// // MQTT Client
// WiFiClient espClient;
// PubSubClient client(espClient);

// void callback(char* topic, byte* payload, unsigned int length) {
//   // Xử lý dữ liệu nhận được từ MQTT broker (nếu cần)
// }

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, password);
  setup_wifi();
  // client.setServer(mqtt_server, mqtt_port);
  // client.setCallback(callback);
  // dht.begin();
  // sensors.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
//       Serial.println("connected");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       delay(5000);
//     }
//   }
// }

//cam bien do duc cua nuoc
float volt = 0.0;  
float NTU = 0.0;
float redondeo(float p_entera, int p_decimal)  
{  
  float multiplicador = powf( 10.0f, p_decimal);  //redondeo a 2 decimales  
  p_entera = roundf(p_entera * multiplicador) / multiplicador;  
  return p_entera;  
}

void turbidity_sensor(){
  volt = 0;
  for(int i = 0; i < 800; i++){
    volt += ((float)analogRead(2)/1024)*5*2.41;
  }
  volt = volt / 800;
  volt = redondeo(volt, 1);
  if (volt < 2.5){
    NTU = 3000;
  }
  else
  {
    NTU = -1120.4 * sqrt(volt) + 5742.3 * volt - 4352.9;
  }

Serial.print(volt);
Serial.print(" V");
Serial.print("\t");
Serial.print(NTU);
Serial.println(" NTU");
Blynk.virtualWrite(V1, NTU);
}

//cam bien pH 0-14
unsigned long int avgValue;
float phValue = 0.0;
int buf[10],temp;
void pH_sensor(){
  for(int i=0;i<10;i++)       
  { 
    buf[i]=analogRead(2);
    delay(10);
  }
  for(int i=0;i<9;i++)       
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      
    avgValue+=buf[i];
  float phVol=(float)avgValue*5.0/1024/4.3; 
  float phValue = -5.70 * phVol + 29.5;  
  phValue = 14.2 - phValue;                 
  Serial.print("    pH:");  
  Serial.print(phValue);
  Serial.println(" ");
  Blynk.virtualWrite(V2, phValue);
}

//cam bien nhiet do
void temparature_sensor(){
   sensors.requestTemperatures();
   float temp = sensors.getTempCByIndex(0);
   Serial.print(temp);
   Serial.println(" C");
   Blynk.virtualWrite(V0, temp);
}

//cam bien Oxy
void Oxy(){
  
}

void loop() {
  Blynk.run();
  // if (!client.connected()) {
  //   reconnect();
  // }
  // client.loop();]
  turbidity_sensor();
  pH_sensor();
  temparature_sensor();
  
  // float humidity = dht.readHumidity();
  // float ph = analogRead(34); // Giả sử cảm biến pH kết nối với chân 34
  // float oxygen = analogRead(35); // Giả sử cảm biến oxy kết nối với chân 35
  // float waterLevel = analogRead(39); // Giả sử cảm biến mực nước kết nối với chân 39

  // // Gửi dữ liệu lên MQTT
  // client.publish("home/aquarium/temperature", String(temp).c_str());
  // client.publish("home/aquarium/humidity", String(humidity).c_str());
  // client.publish("home/aquarium/ph", String(ph).c_str());
  // client.publish("home/aquarium/oxygen", String(oxygen).c_str());
  // client.publish("home/aquarium/turbidity", String(turbidity).c_str());
  // client.publish("home/aquarium/waterLevel", String(waterLevel).c_str());

  //Hiển thị dữ liệu lên OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp: "); display.print(temp); display.println(" C");
  display.print("pH: "); display.println(phValue);
  //display.print("Oxygen: "); display.println(oxygen);
  display.print("Turbidity: "); display.println(NTU);
  //display.print("Water Level: "); display.println(waterLevel);
  display.display();

  delay(2000); // Cập nhật mỗi 2 giây
}