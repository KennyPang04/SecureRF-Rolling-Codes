#include <RH_ASK.h>
#include <SPI.h>

// Variable declarations & initialize drivers
const int funct1 = 10, funct2 = 11, funct3 = 12, funct4 = 13;         // Button Pins
const int receive_pin = 7;                                            // RF Receiver Pin is 7
RH_ASK rf_driver(2000, receive_pin, 0, 0);                            // Initialize RF Receiver Driver
bool state1 = false, state2 = false, state3 = false, state4 = false;  // Create states for now

void setup() {
  // Setup Serial with baud at 115200
  Serial.begin(115200);
  // Setup Pins for LEDs for now
  pinMode(funct1, OUTPUT); pinMode(funct2, OUTPUT); 
  pinMode(funct3, OUTPUT); pinMode(funct4, OUTPUT);
  // Initialize RF Driver with Parameters
  if (!rf_driver.init()) {
    Serial.println("RF Initialization Failed");
  }
}

void clearLEDs() {
  // Clear all states and LEDs
  state1 = false;
  state2 = false;
  state3 = false;
  state4 = false;
  digitalWrite(funct1, LOW);
  digitalWrite(funct2, LOW);
  digitalWrite(funct3, LOW);
  digitalWrite(funct4, LOW);
}

void simpleFSM(int funct) {
  // Simple Switch Case for FSM later
  switch (funct) {
    case 1:
      state1 = true;
      break;
    case 2:
      state2 = true;
      break;
    case 3: 
      state3 = true;
      break;
    case 4:
      state4 = true;
      break;
  }
  // FSM output is changed
  if (state1) {
    clearLEDs();
    digitalWrite(funct1, HIGH);
  } else if (state2) {
    clearLEDs();
    digitalWrite(funct2, HIGH);
  } else if (state3) {
    clearLEDs();
    digitalWrite(funct3, HIGH);
  } else if (state4) {
    clearLEDs();
    digitalWrite(funct4, HIGH);
  }
}

void loop() { 
  // Handle Receiving Msgs 
  uint8_t msg[128];      
  uint8_t length = sizeof(msg);
  if (rf_driver.recv(msg, &length)) {
    msg[length] = '\0';
    Serial.println("We received your message:");
    Serial.println((char*) msg);
    Serial.println(" ");
  }
  // Call simple FSM to change state when there is a new message read
  Serial.println(String(msg[21]-0x30));
  simpleFSM(int(msg[21]-0x30));
  delay(500);
}