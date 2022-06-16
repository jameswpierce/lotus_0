#include <Arduino.h>
#include <MIDIUSB.h>

const size_t numberOfPotentiometers = 32;
const uint8_t numberOfInputsPerMultiplexer = 16;

const uint8_t s0 = 15;
const uint8_t s1 = 14;
const uint8_t s2 = 16;
const uint8_t s3 = 10;
const uint8_t controlPin[] = {s0, s1, s2, s3};

const uint8_t sigs[] = { 1, 0 };

uint8_t muxChannel[16][4] = {
  {0,0,0,0},
  {1,0,0,0},
  {0,1,0,0},
  {1,1,0,0}, 
  {0,0,1,0},
  {1,0,1,0},
  {0,1,1,0},
  {1,1,1,0},
  {0,0,0,1},
  {1,0,0,1},
  {0,1,0,1},
  {1,1,0,1},
  {0,0,1,1},
  {1,0,1,1},
  {0,1,1,1},
  {1,1,1,1}
};

uint8_t controlValues[numberOfPotentiometers] = { 0 };

void controlChange(byte channel, byte control, byte value)
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

float readMux(int channel, uint8_t sig)
{
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  uint8_t val = analogRead(sig) / 8;

  return val;
}

void setup()
{
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  Serial.begin(9600);
}



void loop()
{
  for (uint8_t i = 0; i < numberOfPotentiometers; i++) {
    uint8_t sig = sigs[i / numberOfInputsPerMultiplexer];
    uint8_t channel = i % numberOfInputsPerMultiplexer;
    uint8_t value = readMux(channel, sig);

    if (abs(value - controlValues[i]) > 1)
    {
      controlValues[i] = value;
      controlChange(1, i, controlValues[i]);
      MidiUSB.flush();
      Serial.print(sig);
      Serial.print(channel);
      Serial.print("CC");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(controlValues[i]);
    }
  }
  delay(10);
}
