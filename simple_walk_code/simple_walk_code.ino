#define middle A0
#define left A1
#define right A2
#define trig 5
#define echo 6

#include <Servo.h>

Servo middleServo;
Servo leftServo;
Servo rightServo;

unsigned long echo_value=0;
unsigned long distance=0;

void setup() 
{
  Serial.begin(9600);
  
  middleServo.attach(middle);
  leftServo.attach(left);
  rightServo.attach(right);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  distance=ping();
}

void liftOtherSide()
{
  if (middleServo.read()<=90)
  {
    for (int i=middleServo.read(); i<105; i++)
    {
      middleServo.write(i);
      delay(250);
    }
  }
  else if (middleServo.read()>90)
  {
    for (int i=middleServo.read(); i>75; i--)
    {
      middleServo.write(i);
      delay(250);
    }
  }
}

void moveLegsForwards(Servo &servo)
{
  for (int i=servo.read(); i<105; i++)
  {
    servo.write(i);
    //delay(10);
  }  
}

void moveLegsBackwards(Servo servo)
{
  for (int i=servo.read(); i>75; i--)
  {
    servo.write(i);
    //delay(10);
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
  while (distance>5)
  {
    liftOtherSide();
    delay(25);
    moveLegsForwards(leftServo);
    moveLegsBackwards(rightServo);
    delay(25);
    liftOtherSide();
    delay(25);
    moveLegsForwards(rightServo);
    moveLegsBackwards(leftServo);
    delay(25);
  }
}
