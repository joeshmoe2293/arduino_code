#define LED 13

void writeLED(byte power, int time)
{
  digitalWrite(LED, power);
  delay(time);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  writeLED(HIGH, 500);
  writeLED(LOW, 500);
}
