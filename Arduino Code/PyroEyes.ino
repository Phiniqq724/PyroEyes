#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

String send_data;

String update_data() {
    WiFiClientSecure client;
    client.setInsecure();  // Ignore SSL certificate validation
    HTTPClient https;
    String response="";
  //bool response=false;

    Serial.printf("[Firebase] UPDATE with data : %s\n", send_data.c_str());
    if (https.begin(client, "https://dhthaza-default-rtdb.asia-southeast1.firebasedatabase.app/Dht.json")) {  // HTTPS
    https.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = https.PUT(send_data);
    if (httpCode > 0) {
        response = https.getString();
      //response=true;
        Serial.printf("[Firebase] UPDATE response code: %d\n", httpCode);
      //Serial.println(response); // debug
        } else {
            Serial.printf("[Firebase] UPDATE error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
    } else {
        Serial.printf("[Firebase] Unable to connect\n");
    }
    return response;
}


void setup() {
    Serial.begin(9600);
    dht.begin();
    WiFi.begin("PyroEyes", "jayajayajaya");
    while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
    }
}

void loop() {
    double h = dht.readHumidity();
  // Read temperature as Celsius (the default)
    double t = dht.readTemperature();
    float sensorValue = analogRead(A0);
    Serial.print("Analog Value =");
    Serial.println(sensorValue);

    float voltage = (sensorValue / 1023) * 5;
    Serial.print("Voltage =");
    Serial.print(voltage);
    Serial.println(" V");

    float wind_speed = mapfloat(voltage, 0.4, 2, 0, 32.4);
    float speed_mph = ((wind_speed *3600)/1609.344);
    Serial.print("Wind Speed =");
    Serial.print(wind_speed);
    Serial.println("m/s");
    Serial.print(speed_mph);
    Serial.println("mph");
    delay(2000);

  // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) ) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }
    send_data = (String("{") + "\"Temp\":"+t+","+"\"Humidity\":"+h+","+"\"Wind\":"+"\""+speed_mph+"\""+ "}");
    update_data();

}


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

