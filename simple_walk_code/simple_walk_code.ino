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
  leftServo.write(100);
  rightServo.write(90);

  positions[0]=middleServo.read();
  positions[1]=leftServo.read();
  positions[2]=rightServo.read();
}

void liftOtherSide()
{
  if (middleServo.read()<=90)
  {
    middleServo.write(100);
  }
  else if (middleServo.read()>90)
  {
    middleServo.write(80);
  }
}

void moveLeftLegsForwards()
{
  for (int i=leftServo.read(); i>80; i--)
  {
    leftServo.write(i);
  }  
}

void moveLeftLegsBackwards()
{
  for (int i=leftServo.read(); i<145; i++)
  {
    leftServo.write(i);
  }  
}

void moveRightLegsForwards()
{
  for (int i=rightServo.read(); i<125; i++)
  {
    rightServo.write(i);
  }  
}

void moveRightLegsBackwards()
{
  for (int i=rightServo.read(); i>70; i--)
  {
    rightServo.write(i);
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
  liftOtherSide();
  delay(50);
  moveLeftLegsForwards();
  moveRightLegsBackwards();
  delay(50);
  liftOtherSide();
  delay(50);
  moveRightLegsForwards();
  moveLeftLegsBackwards();
  delay(50);
  /*
  for (int i=70; i<125; i++)
  { 
    rightServo.write(i);
    delay(10);
    
    positions[0]=middleServo.read();
    positions[1]=leftServo.read();
    positions[2]=rightServo.read();
  
    distance=ping();
  
    delay(250);
  
    for (int i=0; i<3; i++)
    {
      Serial.println(positions[i]);
    }
    
    Serial.println(distance);
  
    Serial.println("Next loop...");
  }*/
}
