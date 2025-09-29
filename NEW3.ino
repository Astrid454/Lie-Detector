#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// DHT11
#define DHTPIN A0        // Pinul conectat la DHT11
#define DHTTYPE DHT11    // Tipul senzorului DHT11
DHT dht(DHTPIN, DHTTYPE);

// PulseOximeter
#define REPORTING_PERIOD_MS  7000
#define PULSE_THRESHOLD      70
#define HUMIDITY_THRESHOLD   50

// LED-uri
#define GREEN_LED 12
#define RED_LED   13

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
PulseOximeter pox;


float pulse_sum = 0;
float pulse_avg = 0;

float humidity_sum = 0;
float humidity_avg = 0;

int is_lying = -1;
int count = 0;

float Heart_rate;
float Humidity;
float Temperature;

uint32_t tsLastReport = 0;
uint32_t startTime = 0; // Start timer for 15 seconds
bool is_measurement_in_progress = false; // Flag to track if measurement is in progress

// Callback pentru detectarea pulsului
void onBeatDetected()
{
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
}

void setup()
{
    Serial.begin(115200);
    dht.begin(); // Inițializare DHT11

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    delay(1000);

    // Inițializare PulseOximeter
    lcd.setCursor(0, 1);
    if (!pox.begin()) {
        lcd.print("FAILED");
        while (1);
    } else {
        lcd.print("SUCCESS");
    }
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pox.setOnBeatDetectedCallback(onBeatDetected);

    startMeasurement(); // Start collecting data
}

void loop()
{
    // Actualizează PulseOximeter
    pox.update();
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);

    if (is_measurement_in_progress) {
        // Collect data for 15 seconds
        if (millis() - startTime <= 15000) {
            Heart_rate = pox.getHeartRate();
            Humidity = dht.readHumidity();
            Temperature = dht.readTemperature();

            if (isnan(Humidity) || isnan(Temperature)) {
                Serial.println("Eroare la citirea senzorului DHT!");
                return;
            }

            Serial.print("Heart rate: ");
            Serial.println(Heart_rate);
            Serial.print("Humidity: ");
            Serial.print(Humidity);
            Serial.println(" %");
            Serial.print("Temperature: ");
            Serial.print(Temperature);
            Serial.println(" °C");

            lcd.setCursor(0, 0);
            lcd.print("HR: ");
            lcd.print(Heart_rate);
            lcd.print(" bpm");

            lcd.setCursor(0, 1);
            lcd.print("Humi: ");
            lcd.print(Humidity);
            lcd.print(" %");

            if (Heart_rate >= 60) {
                pulse_sum += Heart_rate;
                humidity_sum += Humidity;
                count++;
            }
        } else {
            // After 15 seconds, process the data and decide
            pulse_avg = pulse_sum / count;
            humidity_avg = humidity_sum / count;

            Serial.println("Average Humidity: " + String(humidity_avg));
            Serial.println("Average Heart Rate: " + String(pulse_avg));

            if (humidity_avg > HUMIDITY_THRESHOLD && pulse_avg > PULSE_THRESHOLD) {
                is_lying = 1; // Lie
            } else {
                is_lying = 0; // Truth
            }

            // Display result on LCD
            if (is_lying == 1) {
                indicate_lying();
            } else {
                indicate_truth();
            }

            // Reset for next measurement
            resetMeasurement();
        }
    }
}

// Function to start the 15 second measurement
void startMeasurement()
{
    pulse_sum = 0;
    humidity_sum = 0;
    count = 0;
    is_measurement_in_progress = true;  // Start the measurement process
    startTime = millis(); // Record the start time
}

// Reset the measurement variables
void resetMeasurement()
{
    is_measurement_in_progress = false;
    pulse_sum = 0;
    humidity_sum = 0;
    count = 0;
}

// Indicate lying on the LCD and LED
void indicate_lying()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You are lying!");
    digitalWrite(RED_LED, HIGH);
    is_lying = -1;  // Reset lying status
}

// Indicate truth on the LCD and LED
void indicate_truth()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You are telling");
    lcd.setCursor(0, 1);
    lcd.print("  the truth!");
    digitalWrite(GREEN_LED, HIGH);
    is_lying = -1;  // Reset truth status
}
