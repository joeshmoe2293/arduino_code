#include <Wire.h>

#define NUM_VALS 4
#define E1 5
#define E2 6
#define M1 4
#define M2 7

typedef enum {dir_a, pow_a, dir_b, pow_b} vals;
byte input_commands[NUM_VALS] = {0};
char changed = 'n';
char status = 'g';

void stop(void);
void readInput(int numBytes);
void writeMotors(void);
void writeStatus(void);

void setup(void) { 
  byte i;
  for(i=4;i<=7;i++)
    pinMode(i, OUTPUT);  
  
  Serial.begin(19200);      //Set Baud Rate
  Serial.println("\n\nRun keyboard control");
  
  Wire.begin(8);
  Wire.onReceive(readInput);
  Wire.onRequest(writeStatus);

  stop();
} 

void loop(void) {
  if (changed == 'y') {
    writeMotors();
    changed = 'n';
  }
  
  delay(50);
}

void stop(void) {
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW);      
}

void readInput(int numBytes) {
  if (numBytes != NUM_VALS) {
    status = 'e';
    stop();
    return;
  } 
  
  byte i; 
  for (i = 0; i < numBytes; i++)
    input_commands[i] = Wire.read();

  status = 'g';
  changed = 'y';
}

void writeMotors(void) {
  digitalWrite(M1, input_commands[dir_a]);
  digitalWrite(M2, input_commands[dir_b]);

  analogWrite(E1, input_commands[pow_a]);
  analogWrite(E2, input_commands[pow_b]);
}

void writeStatus(void) {
  Wire.write(status);
}
