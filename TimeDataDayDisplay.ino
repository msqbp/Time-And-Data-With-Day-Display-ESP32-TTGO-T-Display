#include <TFT_eSPI.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "xyz";
const char* password = "xyz";

const long utcOffsetInSeconds = 2 * 3600; // Central European Time (UTC+2)

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Define TFT pins for TTGO T-Display 1st gen
#define TFT_CS    15
#define TFT_RST   -1 // Not used
#define TFT_DC    33

TFT_eSPI tft = TFT_eSPI(); // Create a TFT_eSPI object

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize TFT display
  tft.init();
  tft.setRotation(1); // Adjust rotation if needed
  tft.fillScreen(TFT_BLACK);

  // Initialize NTP client
  timeClient.begin();
  timeClient.update();
}

void loop() {
  timeClient.update();
  
  // Display current time and date
  displayTimeDate();

  delay(1000); // Update every second
}

void displayTimeDate() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM); // Set text datum to middle center
  tft.setTextColor(TFT_WHITE);

  // Set a moderate font size
  int textSize = 3;
  tft.setTextSize(textSize);

  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();

  // Format the time string
  String timeStr = ((hours < 10) ? " " : "") + String(hours) + ":" + ((minutes < 10) ? "0" : "") + String(minutes) + ":" + ((seconds < 10) ? "0" : "") + String(seconds);

  // Get the epoch time
  unsigned long epochTime = timeClient.getEpochTime();
  // Convert epoch time to a readable date
  String dateStr = epochToDateString(epochTime);
  
  // Get the day of the week
  String dayStr = getDayOfWeek(epochTime);

  // Calculate the width and height of the text
  int16_t textHeight = tft.fontHeight(textSize);

  // Calculate the positions to center the text horizontally
  int16_t x = tft.width() / 2;
  int16_t yTime = (tft.height() - textHeight) / 2 - 30;
  int16_t yDate = yTime + textHeight + 30; // Adding a little spacing between lines
  int16_t yDay = yDate + textHeight + 30;  // Adding a little more spacing

  // Display the time, date, and day of the week on separate lines
  tft.drawString(timeStr, x, yTime);
  tft.drawString(dateStr, x, yDate);
  tft.drawString(dayStr, x, yDay);
}

// Function to convert epoch time to a readable date string
String epochToDateString(unsigned long epochTime) {
  struct tm *timeInfo;
  time_t t = epochTime;
  timeInfo = localtime(&t);
  char buffer[20];
  sprintf(buffer, "%02d/%02d/%02d", timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900); // Day/Month/Year format
  return String(buffer);
}

// Function to get the day of the week from epoch time
String getDayOfWeek(unsigned long epochTime) {
  const char *dayNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  time_t t = epochTime;
  struct tm *timeInfo;
  timeInfo = localtime(&t);
  int dayOfWeek = timeInfo->tm_wday;
  return String(dayNames[dayOfWeek]);
}


