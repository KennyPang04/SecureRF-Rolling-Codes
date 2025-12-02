#include <AESLib.h>
AESLib aesLib;

// Message to Encrypt and Decrypt 
const char *msg = "Test Messaged";         // Change Me!!!
// Create Buffers
char ciphertext[128];
char decryptedtext[128];    

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
  // Copy the message into a 128 byte char array for AES
  char plaintext[128];
  sprintf((char*)plaintext, "%s", message);
  // Call encryption function and return the length
  uint16_t length = aesLib.encrypt((byte*)plaintext, sizeof(plaintext), (byte*)ciphertext, aes_key, sizeof(aes_key), iv1);
  // Was used for debugging purposes
  // Serial.println(message);
  // Serial.println(strlen(ciphertext));
  // Serial.println(sizeof(aes_key));
  return length;
}

uint16_t decryption(byte *cipher, char *decrypted) {
  // Initialize vector for randomization and key
  byte aes_key[16] = {
    0x5A, 0xD4, 0xEA, 0x7C, 0x03, 0xFB, 0x80, 0x66,
    0xC0, 0xFF, 0xEE, 0xAE, 0xC0, 0x2C, 0xF0, 0x0D
  };
  byte iv2[16] = {
    0xC3, 0x0C, 0xAF, 0x92, 0x2D, 0x55, 0xBC, 0x1E,
    0xA3, 0x7F, 0x2E, 0x91, 0x4B, 0xC6, 0x58, 0xE0
  };
  // Call decryption function and return the length
  uint16_t length = aesLib.decrypt((byte*)cipher, 128, (byte*)decrypted, aes_key, sizeof(aes_key), iv2);

  return length;
}

void printBytes(byte *byte_array) {
  for (int i = 0; i < strlen(byte_array); i++) {
    Serial.print((byte) byte_array[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
}

void setup() {
  Serial.begin(9600);
  Serial.println("Start");
  // Call encryption function (this will be done on the transmitter side)
  encryption(msg, ciphertext);
  printBytes(ciphertext);
  // Delay just for testing
  delay(500);
  Serial.println("\nNext Line");
  // Call decryption function (this will be done on the receiver side)
  Serial.println(decryption(ciphertext, decryptedtext)); 
  Serial.println(decryptedtext);
}

void loop() { // Program is also in here to test multiple times
  // Call encryption function (this will be done on the transmitter side)
  // encryption(msg, ciphertext);
  // printBytes(ciphertext);
  // // Delay just for testing
  // delay(500);
  // Serial.println("\nNext Line");
  // // Call decryption function (this will be done on the receiver side)
  // Serial.println(decryption(ciphertext, decryptedtext)); 
  // Serial.println(decryptedtext);
  delay(5000);
}