
// Code for HackSpace magazine
// Displays temperature and humidity values on a I2C OLED Display 128X64 0.96"
// Also shows a scrolling realtime temperature graph
// REQUIRES a DHT11 sensor and a SSD1306  display plus the following libraries:
// Changed DHT library to https://github.com/beegee-tokyo/DHTesp/blob/master//DHT_ESP32/DHT_ESP32.ino
// https://github.com/adafruit/Adafruit_SSD1306
// https://github.com/adafruit/Adafruit-GFX-Library


#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include "DHTesp.h"
#include "Ticker.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>

// Width of our display - used as the maximum stack value
#define MAXSTACK 128

// I2C address for the screen. Update this for your screen.
#define SCREENADR 0x3C

class Stack
{
  private:
    int ourList[MAXSTACK];
    int top;

  public:

    Stack() {
      top = 0;
      for (int i = 0; i <= MAXSTACK; i++)
        ourList[i] = 0;
    }

    void push(int item) {

      if (top == MAXSTACK)
        top = 0;

      ourList[top++] = item;
    }

    int peek(int x) {
      return ourList[(top + x) % MAXSTACK];
    }
};

Stack temperature_stack;
Adafruit_SSD1306 display(4);
DHTesp dht;

//adruit ADC configuration
Adafruit_ADS1115 ads(0x48);
float Voltage = 0.0;


int dhtPin = 18;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREENADR);
  display.clearDisplay();
  dht.setup(dhtPin, DHTesp::DHT22);
  Serial.begin(115200);
  ads.begin();
}

// Function to display a character
void displayNum() {
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);
  display.println("Temp:" + String(dht.getTemperature())); //+ " Hum:" + String(dht.getTempAndHumidity);
}

// Function to display the chart
void displayChart() {
  for (int x = 0; x < MAXSTACK; x++) {
    display.drawLine(x, display.height(), x, 0, BLACK);
    display.drawLine(x, display.height(), x, display.height() - temperature_stack.peek(x), WHITE);
  }
}

void readADCValue() {
  // seperate into its own subroutine
  int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result
  int16_t adc1;  // we read from the ADC, we have a sixteen bit integer as a result
  int16_t adc2;  // we read from the ADC, we have a sixteen bit integer as a result
  int16_t adc3;  // we read from the ADC, we have a sixteen bit integer as a result

  //AIN0 (MQ135f NH3,NOx, alcohol, Benzene, smoke,CO2):
  adc0 = ads.readADC_SingleEnded(0);
  Voltage = (adc0 * 0.1875)/1000;
  Serial.print("AIN0 (MQ135f NH3,NOx, alcohol, Benzene, smoke,CO2): ");
  Serial.print(adc0);
  Serial.print("\tVoltage: ");
  Serial.println(Voltage, 7);
  Serial.println();

  //AIN1 MQ2 (Methane, Butane, LPG, Smoke)
  adc1 = ads.readADC_SingleEnded(1);
  Voltage = (adc1 * 0.1875)/1000;
  Serial.print("AIN1 MQ2 (Methane, Butane, LPG, Smoke): ");
  Serial.print(adc1);
  Serial.print("\tVoltage: ");
  Serial.println(Voltage, 7);
  Serial.println();


  adc2 = ads.readADC_SingleEnded(2);
  Voltage = (adc2 * 0.1875)/1000;
  Serial.print("AIN2 MQ-7 ( Carbon Monoxide): ");
  Serial.print(adc2);
  Serial.print("\tVoltage: ");
  Serial.println(Voltage, 7);
  Serial.println();

  adc3 = ads.readADC_SingleEnded(3);
  Voltage = (adc3 * 0.1875)/1000;
  Serial.print("AIN3 MQ 131 (Ozone): ");
  Serial.print(adc3);
  Serial.print("\tVoltage: ");
  Serial.println(Voltage, 7);
  Serial.println();
  //
  //
  // Serial.print("AIN0: ");
  // Serial.print(adc0);
  // Serial.print("\tVoltage: ");
  // Serial.println(Voltage, 7);
  // Serial.println();
}


void loop() {
  float temperature = dht.getTemperature();
  Serial.print("Temperature: ");
  Serial.println(int(temperature));
  temperature_stack.push(int(dht.getTemperature()));
  displayChart();
  displayNum();
  display.display();
  delay(100);
  readADCValue();
  delay(9999);
}
