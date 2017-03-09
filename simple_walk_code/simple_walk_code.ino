<<<<<<< HEAD
#define trig1 5
#define echo1 6
#define trig2 9
#define echo2 10

unsigned long echo_value=0;
unsigned long distance1=0;
unsigned long distance2=0;
=======
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
>>>>>>> c1deb4125dc7e7ac626f90665b20770e71e208b2

void setup() 
{
  Serial.begin(9600);
<<<<<<< HEAD

  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);

  distance1=ping(trig1, echo1);
  distance2=ping(trig2, echo2);

  Serial.println("Starting...");
  Serial.println("Initial values: ");
  Serial.println("Sensor 1 reads: " + distance1);
  Serial.println("Sensor 2 reads: " + distance2);
  Serial.println("");
=======
  
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
>>>>>>> c1deb4125dc7e7ac626f90665b20770e71e208b2
}

unsigned long ping(int trig, int echo)
{
  digitalWrite(trig, LOW);
  delay(2);
  digitalWrite(trig, HIGH);
  delay(2);
  digitalWrite(trig, LOW);

  echo_value=pulseIn(echo, HIGH);
  return (echo_value/58.138)*.39;
}

unsigned long averageTimes(unsigned long distance1, unsigned long distance2)
{
  return (distance1+distance2)/2;
}

void loop() 
{
<<<<<<< HEAD
  distance1=ping(trig1, echo1);

  delay(200); // Let any residual pings from the unltrasonic sensor gradually disappear
  
  distance2=ping(trig2, echo2);

  Serial.println("Sensor 1 reads: " + distance1);
  Serial.println("Sensor 2 reads: " + distance2);

  Serial.print("Average of the 2 values: " + String(averageTimes(distance1, distance2), 3));

  Serial.println("");
  
  delay(200); // Let any residual pings from the unltrasonic sensor gradually disappear
=======
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
>>>>>>> c1deb4125dc7e7ac626f90665b20770e71e208b2
}
