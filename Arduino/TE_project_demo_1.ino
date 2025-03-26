#include <Adafruit_Fingerprint.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128 , 64 , &Wire , -1);

int u = 0;
int relay = 5;
int buzzer = 4; // Buzzer connected to pin 4

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
#include <SoftwareSerial.h>
// For UNO and others without hardware serial, we must use software serial...
SoftwareSerial mySerial(2, 3);  // pin #2 is IN from sensor, pin #3 is OUT
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Define an array to store roll numbers and their associated names
struct Student {
  int rollNumber;
  String name;
};

Student students[] = {
  {1, "Harshali"},
  {2, "Siddhant"},
  {3, "Krushna"},
  {4, "ayush"},
  {5, "soham"},
  {6, "kamlesh"},

  {7, "Diana"}
};

void setup() {

  


  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT); // Set buzzer pin as output

  Serial.begin(9600);
  while (!Serial); // Wait for Serial to be ready
  delay(1000);
  // Serial.println("\n\nAdafruit finger detect test");



 


  // Initialize fingerprint sensor
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    // Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  // Serial.println(F("Reading sensor parameters"));
  // finger.getParameters();
  // Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  // Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  // Serial.print(F("Security level: ")); Serial.println(finger.security_level);

  

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.println("Sensor doesn't contain any fingerprint data.");
  } else {
    Serial.print("Sensor contains "); Serial.print(finger.templateCount);
    Serial.println(" templates");
  }
  Serial.println("Waiting for valid finger...");




}

void loop() {



  getFingerprintID();
  delay(1000); // Avoid running at full speed
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) {
    // Serial.println("\nNo finger detected");
    
    return p;
  } else if (p != FINGERPRINT_OK) {
    return p;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_PURPLE, 10);
    delay(1000);

    if (u == 0) {
      digitalWrite(relay, HIGH);
      u = 1;
    } else {
      digitalWrite(relay, LOW);
      u = 0;
    }
    

    int rollNo = finger.fingerID;
    String name = getNameByRollNumber(rollNo);

    // Print roll number and name
    Serial.print("Roll no.: ");
    Serial.print(rollNo);
    Serial.print(", Name: ");
    Serial.println(name);

    

    // Beep the buzzer when a roll number is detected
   for (int i = 0; i < 2; i++) {
      digitalWrite(buzzer, HIGH); // Turn buzzer on
      delay(100);                 // Beep duration
      digitalWrite(buzzer, LOW);  // Turn buzzer off
      delay(100);                 // Small pause between beeps
    }
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");

    // Beep the buzzer twice when no match is found
    for (int i = 0; i < 10; i++) {
      digitalWrite(buzzer, HIGH); // Turn buzzer on
      delay(50);                 // Beep duration
      digitalWrite(buzzer, LOW);  // Turn buzzer off
      delay(50);                 // Small pause between beeps
    }
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return finger.fingerID;
}

// Function to find the name by roll number
String getNameByRollNumber(int rollNo) {
  for (int i = 0; i < sizeof(students) / sizeof(students[0]); i++) {
    if (students[i].rollNumber == rollNo) {
      return students[i].name;
    }
  }
  return "Unknown"; // Return "Unknown" if roll number is not found
}
