#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <string.h>

#define NUM_VALS 4
typedef enum {dir_a, pow_a, dir_b, pow_b} vals;
byte motors[NUM_VALS] = {0};

ESP8266WebServer server(80);

void handleRoot();
void handleWriteMotors();
void writeMotors();
void handleDirection();
void handleStatus();
void handleNotFound();

void setup(){
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

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
  server.on("/write_motors", HTTP_POST, handleWriteMotors);
  server.on("/direction", HTTP_POST, handleDirection);
  server.on("/status", HTTP_POST, handleStatus);
  server.onNotFound(handleNotFound);          
  server.begin();
  Serial.println("HTTP server started");

  Wire.begin(D1, D2);
}

void loop(){
  server.handleClient();
}

const char ROOT_HTML[] = 
"<p>"
"<form action=\"/write_motors\" method=POST>"
"<input type=\"radio\" name=\"L\" value=\"F\" checked>Left Forwards<br />"
"<input type=\"radio\" name=\"L\" value=\"B\">Left Backwards<br />"
"<input type=\"text\" name=\"L_P\"><br />"
"<input type=\"radio\" name=\"R\" value=\"F\" checked>Right Forwards<br />"
"<input type=\"radio\" name=\"R\" value=\"B\" >Right Backwards<br />"
"<input type=\"text\" name=\"R_P\"><br />"
"<input type=\"submit\" value=\"Write Motor Powers\">"
"</form>"
"</p>"
"<p>"
"<form action=\"/direction\" method=POST>"
"<table>"
"<tr>"
"<td></td>"
"<td>"
"<input type=\"submit\" name=\"direction\" value=\"F\">Forwards<br />"
"</td>"
"<td></td>"
"</tr>"
"<tr>"
"<td>"
"<input type=\"submit\" name=\"direction\" value=\"L\">Turn Left<br />"
"</td>"
"<td>"
"<input type=\"submit\" name=\"direction\" value=\"S\">Stop Robot!!!<br />"
"<td>"
"<input type=\"submit\" name=\"direction\" value=\"R\">Turn Right<br />"
"</td>"
"</tr>"
"<tr>"
"<td></td>"
"<td>"
"<input type=\"submit\" name=\"direction\" value=\"B\">Backwards<br />"
"</td>"
"<td></td>"
"</tr>"
"</table>"
"</form>"
"</p>"
"<p>"
"<form action=\"/status\" method=POST>"
"<input type=\"submit\" value=\"Check status\">"
"</form>"
"</p>";

void handleRoot() {
  server.send(200, "text/html", ROOT_HTML);
}

void handleWriteMotors() {
  if (!server.hasArg("L") || !server.hasArg("R") || !server.hasArg("L_P") || !server.hasArg("R_P")) {
    server.send(400, "text/plain", "400: Please enter Appropriate values for each input!");
    return;
  }

  String l_f = server.arg("L");
  String r_f = server.arg("R");

  motors[dir_a] = (l_f == "F") ? HIGH : LOW;
  motors[dir_b] = (r_f == "F") ? HIGH : LOW;

  Serial.print("Writing to motors[dir_a]: ");
  Serial.println(motors[dir_a]);
  Serial.print("Writing to motors[dir_b]: ");
  Serial.println(motors[dir_b]);

  int l_pow = server.arg("L_P").toInt();
  int r_pow = server.arg("R_P").toInt();
  
  if (l_pow < 100)
    l_pow = 0;
  else if (l_pow > 255)
    l_pow = 255;

  if (r_pow < 100)
    r_pow = 0;
  else if (r_pow > 255)
    r_pow = 255;
  
  motors[pow_a] = l_pow;
  motors[pow_b] = r_pow;

  Serial.print("Writing to motors[pow_a]: ");
  Serial.println(motors[pow_a]);
  Serial.print("Writing to motors[pow_b]: ");
  Serial.println(motors[pow_b]);
  Serial.println("\n\n");

  server.sendHeader("Location", "/");
  server.send(303);
  writeMotors();
}

void writeMotors() {
  byte i;

  Wire.beginTransmission(8);
  for (i = 0; i < NUM_VALS; i++)
    Wire.write(motors[i]);
  Wire.endTransmission();
}

void handleDirection() {
  if (!server.hasArg("direction")) {
    server.send(400, "text/plain", "400: Please click the button correctly?");
    return;
  }

  String input_dir = server.arg("direction");
  motors[dir_a] = (input_dir == "F" || input_dir == "R") ? HIGH : LOW;
  motors[dir_b] = (input_dir == "F" || input_dir == "L") ? HIGH : LOW;

  Serial.print("Writing to motors[dir_a]: ");
  Serial.println(motors[dir_a]);
  Serial.print("Writing to motors[dir_b]: ");
  Serial.println(motors[dir_b]);
  
  if (input_dir == "S") {
    motors[pow_a] = LOW;
    motors[pow_b] = LOW;
  } else {
    motors[pow_a] = (input_dir == "F" || input_dir == "B") ? 255 : 180;
    motors[pow_b] = (input_dir == "F" || input_dir == "B") ? 255 : 180;
  }

  Serial.print("Writing to motors[pow_a]: ");
  Serial.println(motors[pow_a]);
  Serial.print("Writing to motors[pow_b]: ");
  Serial.println(motors[pow_b]);
  Serial.println("\n\n");

  server.sendHeader("Location", "/");
  server.send(303);
  writeMotors();
}

void handleStatus() {
  Wire.requestFrom(8, 1);
  char status = Wire.read();

  switch (status) {
    case 'g':
      server.send(200, "text/plain", "Robot gave status green!");
      break;
    case 'e':
      server.send(400, "text/plain", "The robot did not receive the status request successfully...");
      break;
    default:
      server.send(400, "text/plain", "Communication error with robot! Check the I2C connections and power supplies...");
  }
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}
