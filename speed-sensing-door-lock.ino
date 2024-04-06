#define ALT_L 2
#define LFDLA 3
#define UNLOCK_RELAY 4
#define LOCK_RELAY 5
#define VSS A0

volatile bool doorLocked = false;
volatile bool engineRunning = false;

int speed = 0;
int prevSpeed = 0;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(ALT_L, INPUT);
  pinMode(LFDLA, INPUT);
  pinMode(UNLOCK_RELAY, OUTPUT);
  pinMode(LOCK_RELAY, OUTPUT);
  pinMode(VSS, INPUT);

  attachInterrupt(digitalPinToInterrupt(ALT_L), engineStoppedISR, FALLING);

  if(digitalRead(ALT_L) == HIGH)
    engineRunning = true;

  if(digitalRead(LFDLA) == LOW)
    doorLocked = true;

   speed = analogRead(VSS);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(ALT_L) == HIGH)
    engineRunning = true;
  else
    engineRunning = false;

  if(digitalRead(LFDLA) == LOW)
    doorLocked = true;
  else
    doorLocked = false;

  digitalWrite(LOCK_RELAY, LOW);
  digitalWrite(UNLOCK_RELAY, LOW);
  
  if(engineRunning && !doorLocked)
  {
    speed = analogRead(VSS);
    if(speed >= 250 && speed <= 400) //Check if the speed is high enough. TODO: values
    {
      if(speed >= prevSpeed + 20) //Check if we're accelerating or decelerating. TODO: values
      {
        digitalWrite(UNLOCK_RELAY, LOW);
        delay(100);
        
        digitalWrite(LOCK_RELAY, HIGH);
        delay(500);
        digitalWrite(LOCK_RELAY, LOW);
      }
    }
    prevSpeed = speed;
  }
  
  delay(1000);
}

void engineStoppedISR()
{
  if(digitalRead(LFDLA) == LOW) //door is locked
    digitalWrite(UNLOCK_RELAY, HIGH);
}
