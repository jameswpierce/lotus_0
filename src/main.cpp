#include <Arduino.h>
#include <MIDIUSB.h>

int s0 = 10;
int s1 = 16;
int s2 = 14;
int s3 = 15;

int sig = 0;

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

float readMux(int channel)
{
  int controlPin[] = {s0, s1, s2, s3};
  int muxChannel[16][4] = {
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

  for (int i = 0; i < 4; i++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  int val = analogRead(sig) / 8;

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

int controlValues[16] = { 0 };

void loop()
{
  for (int i = 0; i < 16; i++) {
    int value = readMux(i);
    if (abs(value - controlValues[i]) > 1)
    {
      controlValues[i] = value;
      controlChange(0, i, controlValues[i]);
      MidiUSB.flush();
    }
    Serial.print("CC");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(controlValues[i]);
  }
  delay(10);
}
