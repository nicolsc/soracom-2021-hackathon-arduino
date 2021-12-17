#include "secrets.h"
#include <MKRGSM.h>
#include <DHT.h>
#include <TinyGPSPlus.h>
#include <Modem.h>

#define VERSION "1.0.0"

//Milliseconds between 2 samplings. 300000 ms = 5 mins
#define SAMPLING_DELAY 300000

#define APN "soracom.io"
#define LOGIN "sora"
#define PWD "sora"

//Sensors configuration : temperature on "D4"
#define DHTPIN 4
#define DHTTYPE DHT11

//GPS Config
//Plug grove sensor to "SERIAL" port
TinyGPSPlus GPS;

//Temp sensor
DHT dht(DHTPIN, DHTTYPE);
float temperature;
float humidity;

//GSM libs
GSM gsmAccess;
GPRS gprsAccess;
GSMClient client;
GSMModem modem;

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);
  dht.begin();
  delay(3000);
  modem.begin();
  Serial.print("Your ICCID is \t");
  Serial.println(modem.getICCID());
}
void loop()
{
  while (Serial1.available() > 0) {
    if (GPS.encode(Serial1.read()))
    {
      if (GPS.location.isValid()) {
        getTemperature();
        displayLocation();
        displayTemperature();
        if (connection()) {
          sendData();
        }
        else {
          Serial.println("Unable to establish connection :(");
        }
        delay(SAMPLING_DELAY);
      }
      else {
        Serial.println("...Waiting for GPS fix...");
        delay(5000); //Wait 5 seconds before trying to get GPS fix
      }
    }
  }

}
void checkWiring() {
  if (millis() > 5000 && GPS.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
}
void displayLocation() {
  Serial.print("Position: ");
  Serial.print(GPS.location.lat(), 6);
  Serial.print(F(","));
  Serial.print(GPS.location.lng(), 6);
  Serial.println("");
}
void getTemperature() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
}
bool connection() {
  Serial.print("Connecting GSM network...");
  if (gsmAccess.begin() != GSM_READY) {
    Serial.println("GSM Access Error :(");
    return false;
  }

  Serial.println("GSM OK");
  if (gprsAccess.attachGPRS(APN, LOGIN, PWD) != GPRS_READY) {
    Serial.println("GPRS Attach : Error");
    return false;
  }
  else {
    Serial.println("GPRS Attach OK");
  }
  return true;
}
void displayTemperature() {
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %\t");
  Serial.print("Temperature: ");
  Serial.println(temperature);
}
String getStructuredData() {

  String json = "{\"deviceid\":\"" + modem.getICCID() + "\", \"temperature\":" + (isnan(temperature) ? "null" : String(temperature)) + ", \"humidity\":" + (isnan(humidity) ? "null" : String(humidity)) + ", \"latitude\":" + String(GPS.location.lat(), 6) + ", \"longitude\":" + String(GPS.location.lng(), 6) + "}";
  Serial.println(json);
  return json;
}
void sendData() {


  String postData = getStructuredData();
  request("POST", "uni.soracom.io", "/hackathon-team3", postData,8888);
  request("POST", "soracom-hackathon-team3.herokuapp.com", "/api/transmissions", postData, 80);

}
bool request(String method, String url, String path, String data, int port) {
  Serial.println("Connecting and sending POST request to " + url);
  int res_connect;

  char charArray[50];
  url.toCharArray(charArray, 50);

  res_connect = client.connect(charArray, port);

  // If connection is successful, POST JSON data
  if (res_connect) {
    client.println("POST "+path+" HTTP/1.1");
    client.println("Host: " + url);
    client.println("User-Agent: SoracomHackathon/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);

    Serial.println("POST OK");

    Serial.println("Receiving response...");
    boolean test = true;
    while (test) {
      // if there are incoming bytes available, print them
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }

      // if the server's disconnected, stop the client:
      if (!client.connected()) {
        Serial.println("Shutting down GSMClient: disconnected");
        client.stop();
        test = false;
      }
    }

    return true;
  }
  else {
    Serial.println("Unable to connect to client");
    return false;
  }
}

/*
  Read input serial
*/
int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}
