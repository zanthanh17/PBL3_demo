#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

// WiFi credentials
const char* ssid = "YourNetworkName";
const char* password = "YourPassword";

// MQTT Broker
const char* mqtt_server = "fb14e4be0db84d6080ca3ca4c737d73d.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "thanhpm175";
const char* mqtt_password = "17052003t";

// DHT Sensor
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// DS18B20 Sensor
#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // Xử lý dữ liệu nhận được từ MQTT broker (nếu cần)
}

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
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  dht.begin();
  sensors.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  float humidity = dht.readHumidity();
  float ph = analogRead(34); // Giả sử cảm biến pH kết nối với chân 34
  float oxygen = analogRead(35); // Giả sử cảm biến oxy kết nối với chân 35
  float turbidity = analogRead(36); // Giả sử cảm biến độ đục kết nối với chân 36
  float waterLevel = analogRead(39); // Giả sử cảm biến mực nước kết nối với chân 39

  // Gửi dữ liệu lên MQTT
  client.publish("home/aquarium/temperature", String(temp).c_str());
  client.publish("home/aquarium/humidity", String(humidity).c_str());
  client.publish("home/aquarium/ph", String(ph).c_str());
  client.publish("home/aquarium/oxygen", String(oxygen).c_str());
  client.publish("home/aquarium/turbidity", String(turbidity).c_str());
  client.publish("home/aquarium/waterLevel", String(waterLevel).c_str());

  // Hiển thị dữ liệu lên OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp: "); display.print(temp); display.println(" C");
  display.print("Humidity: "); display.print(humidity); display.println(" %");
  display.print("pH: "); display.println(ph);
  display.print("Oxygen: "); display.println(oxygen);
  display.print("Turbidity: "); display.println(turbidity);
  display.print("Water Level: "); display.println(waterLevel);
  display.display();

  delay(2000); // Cập nhật mỗi 2 giây
}