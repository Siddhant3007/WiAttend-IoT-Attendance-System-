#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// WiFi credentials
#define WIFI_SSID "Siddhant's S23"
#define WIFI_PASSWORD "0987654321"

// Firebase project credentials
#define DATABASE_URL "https://attendance-bc312-default-rtdb.firebaseio.com/" 
#define DATABASE_SECRET "7uOVbvESpGRHa6NzdvrJeFWV6zfelswA6PDIAJnr"  // Replace with your Database Secret

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi!");

  // Configure Firebase
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;  // Use Database Secret for authentication

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase setup completed!");
}

void loop() {
  char buffer[20] = "";  // Buffer to store incoming serial data

  if (Serial.available() > 0) {
    int len = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1); // Read until newline
    buffer[len] = '\0';  // Ensure it's a valid string

    if (len > 0) {  // Check if data is not empty
      Serial.print("Sending to Firebase: ");
      Serial.println(buffer);

      // Send data to Firebase
      if (Firebase.RTDB.pushString(&fbdo, "/serial_data", buffer)) {
        Serial.println("Data sent successfully!");
      } else {
        Serial.print("Firebase Error: ");
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println("Received empty data, skipping Firebase update.");
    }
  }
}
