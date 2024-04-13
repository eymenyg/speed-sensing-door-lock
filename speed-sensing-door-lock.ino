#define IGN 2  // Ignition (power) state
#define LFDLA 3 // Left Front Door Lock Actuator state. Unlock - LOW; Lock - 20 ms HIGH, 280 ms LOW. Invert the state using a PNP transistor and a resistive voltage divider.
                // This seems to be the case only when the IGN is off and ETACSCM monitoring in pulses to decrease parasitic current.
                // Refactoring the code with this in mind.
#define UNLOCK_RELAY 4
#define LOCK_RELAY 5
#define VSS A0 // Vehicle Speed Signal. Analog voltage input from LM2917 output

volatile bool doorLocked = false;

//volatile long lfdlaLastChange = 0;

int speed = 0;

//long timeNow = 0;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(IGN, INPUT);
  pinMode(LFDLA, INPUT);
  pinMode(UNLOCK_RELAY, OUTPUT);
  pinMode(LOCK_RELAY, OUTPUT);
  pinMode(VSS, INPUT);

  attachInterrupt(digitalPinToInterrupt(IGN), ignOffISR, FALLING);
  //attachInterrupt(digitalPinToInterrupt(LFDLA), lfdlaISR, CHANGE);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(UNLOCK_RELAY, LOW);
  digitalWrite(LOCK_RELAY, LOW);

  if(digitalRead(LFDLA) == LOW) // locked
    doorLocked = true;
  else
    doorLocked = false;


  // timeNow = millis();
  // if(timeNow - lfdlaLastChange > 300)
  // {
  //   doorLocked = false;
  // }

  speed = analogRead(VSS);
  if(speed >= 512)
  {
    if(!doorLocked)
    {
      digitalWrite(LOCK_RELAY, HIGH);
      delay(500);
      digitalWrite(LOCK_RELAY, LOW);
    }
  }
  delay(500);
}

void ignOffISR()
{
  digitalWrite(LOCK_RELAY, LOW); // in case we get here while LOCK_RELAY is HIGH. Avoid driving LOCK_RELAY and UNLOCK_RELAY HIGH simultaneously
  noInterrupts();
  if(doorLocked)
  {
    digitalWrite(UNLOCK_RELAY, HIGH);
    delay(1000);
    digitalWrite(UNLOCK_RELAY, LOW);
  }
  interrupts();
}

// void lfdlaISR()
// {
//   if(digitalRead(LFDLA) == LOW) // locked
//     doorLocked = true;
//   else
//     doorLocked = false;
//   //lfdlaLastChange = millis();
// }
