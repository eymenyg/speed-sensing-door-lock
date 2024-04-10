#define IGN 2  // Ignition (power) state
#define LFDLA 3 // Left Front Door Lock Actuator state. Unlock - LOW; Lock - 20 ms HIGH, 280 ms LOW. Invert the state using a PNP transistor and a resistive voltage divider.
#define UNLOCK_RELAY 4
#define LOCK_RELAY 5
#define VSS A0 // Vehicle Speed Signal. Analog voltage input from LM2917 output

volatile bool doorLocked = false;

volatile long lfdlaLastChange = 0;

int speed = 0;

long timeNow = 0;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(IGN, INPUT);
  pinMode(LFDLA, INPUT);
  pinMode(UNLOCK_RELAY, OUTPUT);
  pinMode(LOCK_RELAY, OUTPUT);
  pinMode(VSS, INPUT);

  attachInterrupt(digitalPinToInterrupt(IGN), ignOffISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(LFDLA), lfdlaISR, CHANGE);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(UNLOCK_RELAY, LOW);
  digitalWrite(LOCK_RELAY, LOW);


  timeNow = millis();
  if(timeNow - lfdlaLastChange > 300)
  {
    doorLocked = false;
  }

  speed = analogRead(VSS);
  if(speed >= 512)
  {
    if(!doorLocked)
    {
      noInterrupts();
      digitalWrite(LOCK_RELAY, HIGH);
      delay(500);
      digitalWrite(LOCK_RELAY, LOW);
      interrupts();
    }
  }
  delay(500);
}

void ignOffISR()
{
  if(doorLocked)
  {
    noInterrupts();
    digitalWrite(UNLOCK_RELAY, HIGH);
    delay(1000);
    digitalWrite(UNLOCK_RELAY, LOW);
    interrupts();
  }
}

void lfdlaISR()
{
  if(digitalRead(LFDLA) == LOW) // locked
    doorLocked = true;
  lfdlaLastChange = millis();
}
