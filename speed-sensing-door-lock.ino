#include "Led.h"
#include <Bounce2.h>

#define ALT_L 2  // Alternator L (Light) state
#define BUTTON 3 // Button for controlling lock state with external 10k pull-up
#define LFDLA 4 // Left Front Door Lock Actuator state
#define RDLA 5 // Rear door lock actuator state
#define LOCK_RELAY 6 //Lock relay output
#define UNLOCK_RELAY 7 // Unlock relay output
#define STATUS_LED 8 // Status LED to indicate lock state
#define VSS A0 // Vehicle Speed Signal. Analog voltage input from LM2917 output

Bounce2::Button button = Bounce2::Button();
Led statusLED;

// Global variables
volatile bool engineTurnedoff = false;
int LEDLastSetState = -1;
int speed = 0;
const int relayTimer = 400; // how long to keep relays on in milliseconds
const int LEDTimer = 500; // LED blinking interval in milliseconds

// Function declarations
int checkLockStatus();
void setLED(int doorLockStatus);
void setLock(bool newStatus);
void buttonPressed();


void setup() {
  pinMode(ALT_L, INPUT);
  //pinMode(BUTTON, INPUT);
  pinMode(LFDLA, INPUT);
  pinMode(RDLA, INPUT);
  pinMode(LOCK_RELAY, OUTPUT);
  pinMode(UNLOCK_RELAY, OUTPUT);
  //pinMode(STATUS_LED, OUTPUT);
  pinMode(VSS, INPUT);

  button.attach (BUTTON, INPUT);
  button.interval(10); // debounce interval in milliseconds
  button.setPressedState(LOW);

  statusLED.init(STATUS_LED, LEDTimer);
  statusLED.setLightOff(); // initialize off

  attachInterrupt(digitalPinToInterrupt(ALT_L), ALT_L_ISR, FALLING);
}

void loop() {
  if(engineTurnedoff) // ALT_L signal went LOW and flag set by ALT_L_ISR
  {
    digitalWrite(LOCK_RELAY, LOW);
    digitalWrite(UNLOCK_RELAY, HIGH);
    delay(relayTimer * 2); // Normally, when the ignition is switched off the main power capacitor would empty here and therefore the delay and writing low is actually unnecessary.
                           // However, the edge case of engine stall would keep the unlock relay turned on indefinitely in the absence of this low write.
                           // Assume the worst and consider a post-crash engine stall. Double the normal time to guarantee unlock upon engine stall.
    digitalWrite(UNLOCK_RELAY, LOW);

    engineTurnedoff = false;
  }

  // Status LED
  setLED(checkLockStatus());
  statusLED.loop();

  // Button
  button.update();
  if(button.pressed())
  {
    if(checkLockStatus() == 0) // all unlocked
      setLock(true);
    else
      setLock(false);
  }

  // Speed Sensing Door Lock logic
  if(checkLockStatus() != 3 && digitalRead(ALT_L) == HIGH) // not all doors are locked and engine running
  {
    speed = analogRead(VSS);
    if(speed >= 512) // 2.5V, TODO: will be adjusted later
      setLock(true);
  }
}

int checkLockStatus() {
  int doorLockStatus = 0;

  if(digitalRead(LFDLA) == LOW) // Front doors locked
    doorLockStatus += 1;

  if(digitalRead(RDLA) == LOW) // Rear doors locked
    doorLockStatus += 2;

  return doorLockStatus; // 0: all unlocked; 1: front locked, rear unlocked; 2: front unlocked, rear locked, ETACS won't allow so impossible; 3: all locked
}

void setLED(int doorLockStatus) {
  if(doorLockStatus == LEDLastSetState)
    return;

  switch(doorLockStatus)
  {
    case 0:
    case 1:
    case 2:
      statusLED.init(STATUS_LED, LEDTimer);
      statusLED.startBlinking();
      break;
    case 3:
      statusLED.setLightOn(); // turn on the LED if doors are locked
      break;
    default:
      statusLED.init(STATUS_LED, LEDTimer/2);
      statusLED.startBlinking(); // hopefully never happens
  }

  LEDLastSetState = doorLockStatus;
}

void setLock(bool newStatus) { // true: lock; false: unlock
  int setRelay = newStatus ? LOCK_RELAY : UNLOCK_RELAY;
  int otherRelay = newStatus ? UNLOCK_RELAY : LOCK_RELAY;

  digitalWrite(otherRelay, LOW); // avoid driving both relays simultaneously to prevent blowing fuses
  delay(15); // small grace period
  digitalWrite(setRelay, HIGH);
  delay(relayTimer);
  digitalWrite(setRelay, LOW);
}

void ALT_L_ISR() {
  engineTurnedoff = true;
}
