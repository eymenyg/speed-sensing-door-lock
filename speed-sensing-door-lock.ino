#define ALT_L 2  // Alternator L (Light) state
#define BUTTON 3 // External button for controlling lock state
#define LFDLA 4 // Left Front Door Lock Actuator state
#define RDLA 5 // Rear door lock actuator state
#define LOCK_RELAY 6 //Lock relay output
#define UNLOCK_RELAY 7 // Unlock relay output
#define STATUS_LED 8 // Status LED to indicate lock state
#define VSS A0 // Vehicle Speed Signal. Analog voltage input from LM2917 output

volatile bool doorLocked = false;

int speed = 0;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(ALT_L, INPUT);
  pinMode(BUTTON, INPUT);
  pinMode(LFDLA, INPUT);
  pinMode(RDLA, INPUT);
  pinMode(LOCK_RELAY, OUTPUT);
  pinMode(UNLOCK_RELAY, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(VSS, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(UNLOCK_RELAY, LOW);
  digitalWrite(LOCK_RELAY, LOW);

  if(digitalRead(LFDLA) == LOW) // locked
    doorLocked = true;
  else
    doorLocked = false;

  speed = analogRead(VSS);
  if(!doorLocked && speed >= 512)
  {
    digitalWrite(UNLOCK_RELAY, LOW); // set unlock_relay LOW before attempting to drive lock_relay HIGH
    digitalWrite(LOCK_RELAY, HIGH);
    delay(500);
    digitalWrite(LOCK_RELAY, LOW);
  }

  delay(500);
}