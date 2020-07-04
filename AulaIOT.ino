#include <DHT.h>
#include <ESP8266WiFi.h>

const char *ssid =  "WESLEY";
const char *pass =  "1020341943";

const char* server = "api.thingspeak.com";
String apiKey = "HQ7IEU19U3UB62MY";
String thingtweetAPIKey = "G2R35RK7ECUS94ML";

WiFiClient client;

#define DHTPIN 5
DHT dht(DHTPIN, DHT11);

void setup() {
  Serial.begin(115200);
  pinMode(0, OUTPUT);
  pinMode(12, OUTPUT);
  delay(10);

  dht.begin();
  
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" degrees Celcius, Humidity: ");
  Serial.print(h);
  Serial.println("%. Send to Thingspeak.");

   if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (t > 25.0f) {
    updateTwitterStatus("Tweet de informação: graus acima de 25.");
    digitalWrite(0, HIGH);
    //digitalWrite(0, HIGH);    
  }

  if (t < 25.0f) {
    updateTwitterStatus("Tweet de informação: graus abaixo de 25.");
    digitalWrite(12, HIGH);
    //digitalWrite(12, HIGH);    
  }

  if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
  {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
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
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(h);
    Serial.println("%. Send to Thingspeak.");
  }
  client.stop();
  Serial.println("Waiting...");
  
  delay(10000);

}

void updateTwitterStatus(String tsData)
{
  if (client.connect(server, 80))
  {
    // Create HTTP POST Data
    tsData = "api_key=" + thingtweetAPIKey + "&status=" + tsData;

    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
  }
}
