#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
const char* ssid     = "ASUS";
const char* password = "12345678";

const char* host = "192.168.43.15";

int pinDHT11 = D1;
SimpleDHT11 dht11(pinDHT11);

int rainsensor = D6;
int soilsensor = A0;
int led = D0;
int motor11 = D7;
int motor12 = D8;
int trigPin = D4;
int echoPin = D3;
long duration;
float cm;
int m;
void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");
  // dht.setup(D1);   /* D1 is used for data communication */
 pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(led, OUTPUT);
  pinMode(motor11, OUTPUT);
  pinMode(motor12, OUTPUT);

}

void loop()
{
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
    return;
  }

  int rain = digitalRead(rainsensor);
  int soil = analogRead(soilsensor);

  if (rain == 0)
  {
    Serial.println(digitalRead(rainsensor));
    Serial.println("rain detected");
  }
  else
  {
    Serial.println(digitalRead(rainsensor));
    Serial.println("rain  not detected");
  }
  digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
cm = (duration/2) / 29.1;    // Divide by 29.1 or multiply by 0.0343    
// inches = (duration/2) / 74;  // Divide by 74 or multiply by 0.0135
  
 // Serial.print(inches);
 // Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(1000);
  
  if(analogRead(soilsensor)>900)
  {
  Serial.println(analogRead(soilsensor));
  Serial.println("soil not contain water");
  }
  else
  {
    Serial.println(analogRead(soilsensor));
  Serial.println("soil contain water");
  }
  if(cm<80)
  {
  if (soil > 900 && temperature > 35 && humidity > 65)
  {
    digitalWrite(motor11, HIGH);
    digitalWrite(motor12, HIGH);
  }
  else
  {
    digitalWrite(motor11, LOW);
    digitalWrite(motor12, LOW);
  }
  }
  String url = "/demoup.php?svalue=" + String(soil) + "&rvalue=" + String(rain) + "&temp=" + String(temperature) + "&hum=" + String(humidity)+ "&cm=" + String(cm);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  // This will send the request to the server
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  Serial.println();
  Serial.println("closing connection");
  delay(2000);
}

