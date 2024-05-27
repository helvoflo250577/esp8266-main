#include <ESP8266WiFi.h>           // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
/*#include <SPI.h>
#define BME_SCK 14
#define BME_MISO 12
#define BME_MOSI 13
#define BME_CS 15*/

#define SEALEVELPRESSURE_HPA (1003.7)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void setup() {
  Serial.begin(115200);
  Serial.println(F("BME280 test"));

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 10000;

  Serial.println();
}
const int AirValue = 680;   //you need to replace this value with Value_1
const int WaterValue = 313;  //you need to replace this value with Value_2
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent=0;


//MariaDB settings
IPAddress server_addr(xxx,xxx,xx,xxx);  // IP of the MySQL *server* here
char user[] = "root";              // MySQL user login username
char password[] = "xxxxxxxx";        // MySQL user login password

// MariaDB query
char INSERT_SQL[] = "INSERT INTO Soil.Measurements (soilpercentage,Sensorid) VALUES (%f,%d)";
char query[128];

char INSERT_SQL2[] = "INSERT INTO bme280.measurements (temp,druk,luchtvochtigheid) VALUES (%f,%f,%f)";
char query2[128];

// WiFi settings
char ssid[] = "Asus";         // your SSID
char pass[] = "Sheila27!";     // your SSID Password
WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;




void loop()
{
  Serial.begin(115200);
  //while (!Serial); // wait for serial port to connect. Needed for Leonardo only
  Serial.println("I'm awake!!!");
  delay(5000);
  // Begin WiFi section
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // print out info about the connection:
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);
 
  //read soil moisture

float soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
Serial.println(soilMoistureValue);
float soilpercentage = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
Serial.println(soilpercentage);
  //change sensorid here
  int sensorid = 1;
delay(500);
//read bme280
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  float temp=(bme.readTemperature());
delay(500);

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
  float druk=(bme.readPressure() / 100.0F);
delay(500);

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
delay(500);

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  float luchtvochtigheid=(bme.readHumidity());
delay(500);  
  //Put sensordata in database
  
  sprintf(query, INSERT_SQL, soilpercentage,sensorid);
  Serial.println("Recording data.");
  Serial.println(query);
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  sprintf(query, INSERT_SQL, soilpercentage,sensorid);
delay(500);
  sprintf(query2, INSERT_SQL2, temp, druk, luchtvochtigheid);
  Serial.println("Recording data.");
  Serial.println(query2);
  cur_mem->execute(query2);
  sprintf(query2, INSERT_SQL2, temp, druk, luchtvochtigheid);

  delete cur_mem;

  //disconnect wifi
  WiFi.disconnect();
  //wait 5 seconds for WiFi to disconnect
  delay (5000);
  Serial.print("WiFi status changed to: ");
  Serial.println(WiFi.status());
  //delay(1800000);
   // Deep sleep mode for 30 minutes, the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
  Serial.println("I'm going into deep sleep mode for 30 minutes");
  ESP.deepSleep(3600e6);
}