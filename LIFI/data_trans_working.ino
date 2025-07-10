// Arduino Laser Communication System
// Transmits and receives ASCII strings via laser

// Pin definitions
int laserPin = 2;    // Laser (Transmitter)
int photoPin = A0;   // Photodiode (Receiver)

// Communication parameters
const int bitDelay = 8;      // 5ms per bit (adjust based on reliability)
const int MAX_LENGTH = 100;  // Maximum message length

// Transmission variables
char inputString[MAX_LENGTH];  // Buffer for incoming serial data
bool stringComplete = false;   // Flag to indicate complete string received
bool transmitting = false;     // Flag to indicate transmission in progress
int charIndex = 0;             // Current character in transmission
int bitIndex = 0;              // Current bit in transmission
byte currentByte;              // Current byte being transmitted

// Reception variables
byte receivedByte = 0;         // Current byte being received
int receivedBitIndex = 0;      // Current bit position in reception
char receivedString[MAX_LENGTH]; // Buffer for received message
int receivedCharIndex = 0;     // Index for received characters
unsigned long lastBitTime = 0; // Timing for bit reception
int errorCount = 0;            // Count of transmission errors

void setup() {
  pinMode(laserPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("ASCII Laser Communication System");
  Serial.println("Enter text to transmit:");
  
  // Initialize input buffer
  memset(inputString, 0, MAX_LENGTH);
}

void loop() {
  // Check for user input
  while (Serial.available() > 0 && !stringComplete) {
    char inChar = (char)Serial.read();
    
    // Add character to buffer if not newline and buffer not full
    if (inChar != '\n' && inChar != '\r') {
      if (strlen(inputString) < MAX_LENGTH - 1) {
        inputString[strlen(inputString)] = inChar;
      }
    } else {
      // Newline received, start transmission
      stringComplete = true;
      transmitting = true;
      charIndex = 0;
      bitIndex = 0;
      
      // Reset received string buffer
      memset(receivedString, 0, MAX_LENGTH);
      receivedCharIndex = 0;
      errorCount = 0;
      
      Serial.print("Transmitting: ");
      Serial.println(inputString);
    }
  }
  
  // Transmit data if we have a complete string
  if (transmitting) {
    // Get current character to transmit
    if (charIndex < strlen(inputString)) {
      currentByte = inputString[charIndex];
      
      // Transmit one bit at a time
      digitalWrite(laserPin, bitRead(currentByte, 7 - bitIndex) ? HIGH : LOW);
      
      // Read the photodiode to check what was received
      float Vout = (analogRead(photoPin) * 5.0) / 1023.0;
      byte receivedBit = Vout > 0.4 ? 1 : 0; // Thresholding
      
      // Store the bit in our received byte
      bitWrite(receivedByte, 7 - bitIndex, receivedBit);
      
      // Check if bit was received correctly
      if (bitRead(currentByte, 7 - bitIndex) != receivedBit) {
        errorCount++;
      }
      
      // Delay for stable transmission
      delay(bitDelay);
      
      // Move to next bit
      bitIndex++;
      
      // If we've sent all 8 bits of this character
      if (bitIndex >= 8) {
        // Store the received character
        receivedString[receivedCharIndex++] = (char)receivedByte;
        
        // Reset for next character
        bitIndex = 0;
        charIndex++;
        receivedByte = 0;
      }
    } else {
      // End of transmission
      transmitting = false;
      receivedString[receivedCharIndex] = '\0'; // Null terminate
      
      // Display results
      Serial.println("\nTransmission complete!");
      Serial.print("Sent:     ");
      Serial.println(inputString);
      Serial.print("Received: ");
      Serial.println(receivedString);
      Serial.print("Bit errors: ");
      Serial.print(errorCount);
      Serial.println(" bits");
      
      // Calculate and display error rate
      float errorRate = (float)errorCount / (strlen(inputString) * 8) * 100;
      Serial.print("Error rate: ");
      Serial.print(errorRate, 2);
      Serial.println("%");
      
      // Reset for next message
      memset(inputString, 0, MAX_LENGTH);
      stringComplete = false;
      Serial.println("\nEnter text to transmit:");
    }
  }
}