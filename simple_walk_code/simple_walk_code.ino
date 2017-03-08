#define middle 10
#define left 11
#define right 12
#define trig 5
#define echo 6

#include <Servo.h>

Servo middleServo;
Servo leftServo;
Servo rightServo;

unsigned long echo_value=0;
unsigned long distance=10;

int positions[3];

void setup() 
{
  Serial.begin(9600);
  
  middleServo.attach(middle);
  leftServo.attach(left);
  rightServo.attach(right);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  middleServo.write(90);
  leftServo.write(90);
  rightServo.write(90);

  positions[0]=middleServo.read();
  positions[1]=leftServo.read();
  positions[2]=rightServo.read();
}

void liftOtherSide()
{
  if (middleServo.read()<=90)
  {
    for (int i=middleServo.read(); i<105; i++)
    {
      middleServo.write(i);
      delay(10);
    }
  }
  else if (middleServo.read()>90)
  {
    for (int i=middleServo.read(); i>75; i--)
    {
      middleServo.write(i);
      delay(10);
    }
  }
}

void moveLegsForwards(Servo &servo)
{
  for (int i=servo.read(); i<125; i++)
  {
    servo.write(i);
    delay(10);
  }  
}

void moveLegsBackwards(Servo servo)
{
  for (int i=servo.read(); i>55; i--)
  {
    servo.write(i);
    delay(10);
  }  
}

unsigned long ping()
{
  digitalWrite(trig, LOW);
  delay(2);
  digitalWrite(trig, HIGH);
  delay(2);
  digitalWrite(trig, LOW);

  echo_value=pulseIn(echo, HIGH);
  distance=(echo_value/58.138)*.39;
  return distance;
}

void loop() 
{
  // NOT SURE HOW WELL THIS IS GOING TO WORK...
  /*liftOtherSide();
  moveLegsForwards(leftServo);
  moveLegsBackwards(rightServo);
  delay(25);
  liftOtherSide();
  delay(25);
  moveLegsForwards(rightServo);
  moveLegsBackwards(leftServo);
  delay(25);*/
  for (int i=50; i<110; i++)
  { 
    middleServo.write(i);
    delay(10);
    
    positions[0]=middleServo.read();
    positions[1]=leftServo.read();
    positions[2]=rightServo.read();
  
    distance=ping();
  
    delay(500);
  
    for (int i=0; i<3; i++)
    {
      Serial.println(positions[i]);
    }
    
    Serial.println(distance);
  
    Serial.println("Next loop...");
  }
}
