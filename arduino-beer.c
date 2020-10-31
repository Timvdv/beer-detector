#include <WiFi.h>
#include <WiFiMulti.h>
#include <Adafruit_NeoPixel.h>
#include <WebServer.h>
#include <ESP32Servo.h>

#define PIN 15
#define NUM 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM,PIN, NEO_GRB + NEO_KHZ800);

WebServer server(80);
WiFiMulti WiFiMulti;

char ssid[] = "secret";
char pass[] = "secret";

int servoPin = 32;
int r,g,b,data;
int pos = 0;
Servo myservo;
int animationState = 0;

void setup()
{
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object

  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  WiFiMulti.addAP(ssid, pass);

  while(WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/on", start_animation);
  server.on("/off", stop_animation);
  server.onNotFound(not_found);

  server.begin();
  pixels.begin();
}

void loop()
{
  server.handleClient();

  if(animationState == 0) {
    pixels.clear();
  }

  if(animationState == 1) {
    int posFrom = 80;
    int posTo = 130;

    for (pos = posFrom; pos <= posTo; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15ms for the servo to reach the position
    }

    for (pos = posTo; pos >= posFrom; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(15);             // waits 15ms for the servo to reach the position
    }

    animation1();
    animationState = 0;
  }
}

void static1(int r, int g, int b)
{
  for(int i=0;i<=NUM;i++)
  {
  pixels.setPixelColor(i, pixels.Color(r,g,b));
  pixels.show();
  }
}

void animation1()
{
  for(int i=0;i<NUM;i++)
  {
    pixels.setPixelColor(i, pixels.Color(255,0,0));
    pixels.show();
    delay(100);
  }
  for(int i=NUM;i>=0;i--)
  {
    pixels.setPixelColor(i, pixels.Color(0,255,0));
    pixels.show();
    delay(100);
  }
  for(int i=0;i<NUM;i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,255,255));
    pixels.show();
    delay(100);
  }
  for(int i=NUM;i>=0;i--)
  {
    pixels.setPixelColor(i, pixels.Color(255,255,0));
    pixels.show();
    delay(100);
  }
}

void start_animation() {
  animationState = 1;
  Serial.println("Started animation=");
  sendJSONMessage("true", "started animation");
}

void stop_animation() {
  animationState = 0;
  pixels.clear();
  myservo.write(100);
  Serial.println("Stopped animation=");
  sendJSONMessage("true", "turned off the lights");
}

void not_found() {
  animationState = 0;
  Serial.println("Started animation=");
  sendJSONMessage("false", "command not found");
}

void sendJSONMessage(String success, String message) {
  String json = "{ \"success\": " + success + " , \"message\": \"" + message + "\" }";
  server.send(200, "application/json", json);
}