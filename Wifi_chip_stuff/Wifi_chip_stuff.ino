#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

//ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

void handleRoot();
void handleServoInput();
void handleSanityCheck();
void handle3();
void handle90();
void handle177();
void handleNotFound();

void setup(){
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  /*const char* ssid = "TELUS1558";
  const char* pwd =  "eaa21c4f6d";
  wifiMulti.addAP(ssid, pwd);
  Serial.println("Connecting ...");
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(250);
    Serial.print('.');
  }
  
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());               
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());*/

  const char* ssid = "ESP8266";
  const char* pwd = "testing123";

  WiFi.softAP(ssid, pwd);
  Serial.print("Access point ");
  Serial.print(ssid);
  Serial.print(" started!\n IP address:\t");
  Serial.println(WiFi.softAPIP());

  if (MDNS.begin("esp8266")) {              
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);        
  server.on("/position", HTTP_POST, handleServoInput); 
  server.on("/sanity_check", HTTP_POST, handleSanityCheck);
  server.on("/3", HTTP_POST, handle3);
  server.on("/90", HTTP_POST, handle90);
  server.on("/177", HTTP_POST, handle177);
  server.onNotFound(handleNotFound);          
  server.begin();
  Serial.println("HTTP server started");

  Wire.begin(D1, D2);
}

void loop(){
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", "<p><form action=\"/position\" method=\"POST\"><input type=\"text\" name=\"s_pos\" placeholder=\"Servo Position\"><input type=\"submit\" value=\"Submit\"></form></p><p><form action=\"/sanity_check\" method=\"POST\"><input type=\"submit\" value=\"Check Servo Position lines up with input position\"></form></p><p><table><tr><td><form action=\"/3\" method=\"POST\"><input type=\"submit\" value=\"3 degrees\"></input></form></td><td><form action=\"/90\" method=\"POST\"><input type=\"submit\" value=\"90 degrees\"></input></form></td><td><form action=\"/177\" method=\"POST\"><input type=\"submit\" value=\"177 degrees\"></input></form></td></tr></table></p><p>Try entering a position for the servo between 3 and 177...</p>");
}

void handleServoInput() {
  if (server.hasArg("s_pos") && server.arg("s_pos") != NULL) {
    int pos = server.arg("s_pos").toInt();

    if (pos >= 3 && pos <= 177) {
      server.sendHeader("Location","/");
      server.send(303);
  
      Wire.beginTransmission(8);
      Wire.write(pos);
      Wire.endTransmission();
    } else {
      server.send(401, "text/plain", "401: Invalid servo position");
    }
  } else {
    server.send(400, "text/plain", "400: Please enter a valid servo position");
  }
}

void handleSanityCheck() {
  Wire.requestFrom(8, 1);
  
  char input = Wire.read();

  switch (input) {
    case 'S':
      server.send(200, "text/plain", "Successfully wrote to the input position!");
      break;
    case 'F':
      server.send(400, "text/plain", "There was an error in writing to the desired value!");
      break;
    default:
      server.send(400, "text/plain", "There was a communication error with the servo controlling arduino...");
  }
}

void handle3() {
  server.sendHeader("Location","/");
  server.send(303);

  Wire.beginTransmission(8);
  Wire.write(3);
  Wire.endTransmission();
}

void handle90() {
  server.sendHeader("Location","/");
  server.send(303);

  Wire.beginTransmission(8);
  Wire.write(90);
  Wire.endTransmission();
}

void handle177() {
  server.sendHeader("Location","/");
  server.send(303);

  Wire.beginTransmission(8);
  Wire.write(177);
  Wire.endTransmission();
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}
