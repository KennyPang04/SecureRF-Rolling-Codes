#include <RH_ASK.h>
#include <SPI.h>

// Variable declarations & initialize drivers
const int funct1 = 10, funct2 = 11, funct3 = 12, funct4 = 13;         // Button Pins
const int receive_pin = 7;                                            // RF Receiver Pin is 7
RH_ASK rf_driver(2000, receive_pin, 0, 0);                            // Initialize RF Receiver Driver
AESLib aesLib;
bool state1 = false, state2 = false, state3 = false, state4 = false;  // Create states for now
int internal_counter = 3164; 

struct Packet {
  char* hashed;
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
  uint8_t ciphertext[32];    
  uint8_t length = sizeof(ciphertext);
  char decryptedtext[32]; 

  if (rf_driver.recv(ciphertext, &length)) {
    // Check the received message
    ciphertext[length] = '\0';
    Serial.println("We received your message:");
    Serial.println((char*) ciphertext);
    // Decrypt the message and check it
    decryption(ciphertext, decryptedtext);
    Serial.println(decryptedtext);
    // Split the message into our expected values to compare
    int counter = 0;
    int function = 0;
    bool done = false;
    for (int i = 0; i < length; i++) {
      if (!done) {
        if (decryptedtext[i] == '_') { 
          function = (decryptedtext[i+1] - 0x30);
          done = true;
        } else {
          counter *= 10;
          counter += (decryptedtext[i] - 0x30);
        }
      }
    }
    packet.hashed = ciphertext;
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