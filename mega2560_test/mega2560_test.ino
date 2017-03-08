#define LED 13

void writeToLED(byte power, int time)
{
  digitalWrite(LED, power);
  delay(time);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i=0; i<10; i++)
  {
    writeToLED(HIGH, (10-i)*100);
    writeToLED(LOW, (i+1)*100);
  }
}
