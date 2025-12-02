#include <RH_ASK.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <cppQueue.h>
#include <AESLib.h>

// Create Structs that are needed for Buttons and Transmitting/Queue
struct Packet {
  char* hashed;
  int counter;
  int function;
};
struct Button {
  int pin;
  bool lastStable, lastRead;
  uint32_t lastChange, lastFire;
};

// Create constants for pins & initialize drivers and queue
const int funct1 = 5, funct2 = 4, funct3 = 3, funct4 = 2; // Button Pins
const int led = 6, transmit_pin = 7;                      // LED Pin is 6 and RF Transmitter Pin is 7
const int rs=13, en=12, d4=11, d5=10, d6=9, d7=8;         // LCD Pins are 8-13
RH_ASK rf_driver(2000, 0, transmit_pin, 0);              
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
cppQueue transmitterQueue(sizeof(Packet), 8, FIFO);
AESLib aesLib;

int counter = 3164;
const char msg[32];  
char ciphertext[32];    

uint16_t encryption(char *message, char *ciphertext) {
  // Initialize vector for randomization and key
  byte aes_key[16] = {
    0x5A, 0xD4, 0xEA, 0x7C, 0x03, 0xFB, 0x80, 0x66,
    0xC0, 0xFF, 0xEE, 0xAE, 0xC0, 0x2C, 0xF0, 0x0D
  };
  byte iv1[16] = {
    0xC3, 0x0C, 0xAF, 0x92, 0x2D, 0x55, 0xBC, 0x1E,
    0xA3, 0x7F, 0x2E, 0x91, 0x4B, 0xC6, 0x58, 0xE0
  };
  // Copy the message into a 32 byte char array for AES
  char plaintext[32];
  sprintf((char*)plaintext, "%s", message);
  // Call encryption function and return the length
  uint16_t length = aesLib.encrypt((byte*)plaintext, sizeof(plaintext), (byte*)ciphertext, aes_key, sizeof(aes_key), iv1);
  // Was used for debugging purposes
  // Serial.println(message);
  // Serial.println(strlen(ciphertext));
  // Serial.println(sizeof(aes_key));
  return length;
}

void printBytes(byte *byte_array) {
  for (int i = 0; i < strlen(byte_array); i++) {
    Serial.print((byte) byte_array[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
}

// Button array for all buttons on the bread board
Button buttons[] = {
  {funct1, HIGH, HIGH, 0, 0},
  {funct2, HIGH, HIGH, 0, 0},
  {funct3, HIGH, HIGH, 0, 0},
  {funct4, HIGH, HIGH, 0, 0},
};
void handleButtons() {
  // Get current time for computations 
  uint32_t currentTime = millis();
  // Loop through each button for reads
  for (auto &button: buttons) {
    int currentRead = digitalRead(button.pin);
    if (currentRead != button.lastRead) { button.lastChange = currentTime; }

    if ((currentTime - button.lastChange) > 30) {
      if (currentRead != button.lastStable) {
        button.lastStable = currentRead;

        if (button.lastStable == LOW) {
          int function = 255;
          switch (button.pin) {
            case funct1:
              Serial.println("First Functionality Button was Pressed");
              function = 1;
              break;
            case funct2:
              Serial.println("Second Functionality Button was Pressed");
              function = 2;
              break;
            case funct3:
              Serial.println("Third Functionality Button was Pressed");
              function = 3;
              break;
            case funct4:
              Serial.println("Forth Functionality Button was Pressed");
              function = 4;
              break;
          }
          if (function != 255) {
            // Handle Encryption
            snprintf(msg, 32, "%d_%d", counter, function);
            encryption(msg, ciphertext);
            // Store everything in a packet
            struct Packet packetCode;
            packetCode.hashed = ciphertext;
            packetCode.counter = counter;
            packetCode.function = function;
            // Store the packet into a queue to get transmitted
            transmitterQueue.push(&packetCode);
            Serial.println("Queued");
            Serial.println(counter);
            Serial.println(function);
            Serial.println(ciphertext);
            // Increment counter to prevent simple relay attacks
            counter += 67;
          }
        }
      }
    }
    button.lastRead = currentRead;
  }
}

void handleTransmit() {
  // Keep the task running until program is ended
  Packet pk;
  if (transmitterQueue.pop(&pk)) {
    // Read the current Packet from queue
    // Serial.print(pk.counter);
    // Serial.print(" ");
    // Serial.println(pk.function);

    // Send a 32 byte message to receiver
    char msg[32];
    strncpy(msg, pk.hashed, sizeof(msg));
    Serial.println(pk.hashed);
    rf_driver.send((uint8_t *)msg, strlen(msg));
    rf_driver.waitPacketSent();

    // Output handling such as LED and LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sent Message");
    lcd.setCursor(0, 1);
    lcd.print(String(pk.counter) + " " + String(pk.function));
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    Serial.println("Done Sending");
  }
}

void setup() {
  // Setup Serial with baud at 115200
  Serial.begin(115200);
  // Setup Pins for Buttons
  pinMode(funct1, INPUT_PULLUP); pinMode(funct2, INPUT_PULLUP); 
  pinMode(funct3, INPUT_PULLUP); pinMode(funct4, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  // Initialize RF Driver with Parameters
  if (!rf_driver.init()) {
    Serial.println("RF Initialization Failed");
  }
  // Set up LCD and reset anything on it
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD Started");
}

void loop() { 
  // Call functions cause FreeRTOS tasks not working
  handleButtons();
  handleTransmit();
}