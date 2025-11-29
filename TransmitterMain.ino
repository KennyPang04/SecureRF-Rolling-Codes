#include <RH_ASK.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <cppQueue.h>

// Create Structs that are needed for Buttons and Transmitting/Queue
struct Packet {
  long counter;
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
            struct Packet packetCode;
            packetCode.counter = 14;
            packetCode.function = function;
            transmitterQueue.push(&packetCode);
            Serial.println("Queued");
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
    Serial.print(pk.counter);
    Serial.print(" ");
    Serial.println(pk.function);

    // Send a 32 byte message for now (needs to be encrypted for phase 3)
    char msg[32];
    sprintf(msg, "This is our message: %d", pk.function);
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