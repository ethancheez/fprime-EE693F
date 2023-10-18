#include "RH_RF69.h"

#define RFM69_FREQ 915
#define RFM69_CS   8
#define RFM69_INT  3
#define RFM69_RST  14

RH_RF69 rfm69;
elapsedMillis timer;
uint8_t packet[] = "Haha I am attacking you";
uint32_t m_interval = 1000; // Interval to send packet in milliseconds

// Helper functions
void radioReset();

void setup() {
  pinMode(RFM69_RST, OUTPUT);

  radioReset();

  while(!rfm69.init()) {
    Serial.println("Failed to init radio");
    radioReset();
    delay(100);
  }

  rfm69.setFrequency(RFM69_FREQ);
  rfm69.setTxPower(14, true);

  Serial.println("Radio initialized!");
}

void loop() {
  if (timer > m_interval) {
    rfm69.send(&packet[0], (sizeof(packet) / sizeof(uint8_t)) - 1);
    if(!rfm69.waitPacketSent(500)) {
      Serial.println("Failed to send packet");
    }
    timer = 0;
  }
}

void radioReset() {
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
}