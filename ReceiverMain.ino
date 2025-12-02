#include <RH_ASK.h>
#include <SPI.h>

// Variable declarations & initialize drivers
const int funct1 = 10, funct2 = 11, funct3 = 12, funct4 = 13;         // Button Pins
const int receive_pin = 7;                                            // RF Receiver Pin is 7
RH_ASK rf_driver(2000, receive_pin, 0, 0);                            // Initialize RF Receiver Driver
bool state1 = false, state2 = false, state3 = false, state4 = false;  // Create states for now
int internal_counter = 3164; 

struct Packet {
  char* text;
  int counter;
  int function;
};
struct Packet packet;

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

void printBytes(byte *byte_array) {
  for (int i = 0; i < strlen(byte_array); i++) {
    Serial.print((byte) byte_array[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
}

void handleReceive() {
  uint8_t msg[32];    
  uint8_t length = sizeof(msg);

  if (rf_driver.recv(msg, &length)) {
    // Check the received message
    msg[length] = '\0';
    Serial.println("We received your message:");
    Serial.println((char*) msg);
    // Decrypt the message and check it
    // Split the message into our expected values to compare
    int counter = 0;
    int function = 0;
    bool done = false;
    for (int i = 0; i < length; i++) {
      if (!done) {
        if (msg[i] == '_') { 
          function = (msg[i+1] - 0x30);
          done = true;
        } else {
          counter *= 10;
          counter += (msg[i] - 0x30);
        }
      }
    }
    packet.text = msg;
    packet.counter = counter;
    packet.function = function;
    Serial.println(String(counter) + " " + String(function));
    if (counter == internal_counter+67*0 || counter == internal_counter+67*1 || 
        counter == internal_counter+67*2 || counter == internal_counter+67*3 || 
        counter == internal_counter+67*4 || counter == internal_counter+67*5 || 
        counter == internal_counter+67*6 || counter == internal_counter+67*7 || 
        counter == internal_counter+67*8 ) {
      simpleFSM(function);
      internal_counter = counter;
    }
  }
}

void loop() { 
  // Handle Receiving Msgs 
  handleReceive();
  // Call simple FSM to change state when there is a new message read
  //delay(10);
}