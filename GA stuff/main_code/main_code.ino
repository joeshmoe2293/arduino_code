#define trig1 5
#define echo1 6
#define trig2 9
#define echo2 10

unsigned long echo_value=0;
unsigned long distance1=0;
unsigned long distance2=0;

void setup() 
{
  Serial.begin(9600);
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
  distance1=ping(trig1, echo1);

  delay(200); // Let any residual pings from the unltrasonic sensor gradually disappear
  
  distance2=ping(trig2, echo2);

  Serial.println("Sensor 1 reads: " + distance1);
  Serial.println("Sensor 2 reads: " + distance2);

  Serial.print("Average of the 2 values: " + String(averageTimes(distance1, distance2), 3));

  Serial.println("");
  
    Serial.println("Next loop...");
  }
  delay(200); // Let any residual pings from the unltrasonic sensor gradually disappear
}
