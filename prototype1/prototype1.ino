#include <LiquidCrystal.h>

const unsigned int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7; //lcd
const unsigned int sql = 1;
const char ctcss[] = "0000";
const unsigned int cup = 8, cdown = 9, vup = 10, vdown = 11, pttr = 12, squelch = 13; //buttons
const unsigned int tx = 1, rx = 0; //serial
const unsigned int pttw = A0, pd = A1, hl = A2; //dra818u
unsigned int freq = 0, vol = 6;
const String freqs[] = {"462.5625", "462.5875", "462.6125", "462.6375", "462.6625", "462.6875", "462.7125", "467.5625", "467.5875", "467.6125", "467.6375", "467.6625", "467.6875", "467.7125", "462.5500", "462.5750", "462.6000", "462.6250", "462.6500", "462.6750", "462.7000", "462.7250"};
bool transmit = false;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//serial parameters
const byte rxBufMax = 32;
char rxBuf[32];
bool rxDone = false;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  for (int i = cup; i <= squelch; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(pttw, OUTPUT);
  pinMode(pd, OUTPUT);
  pinMode(hl, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(cup), chanup, FALLING);
  attachInterrupt(digitalPinToInterrupt(cdown), chandown, FALLING);
  attachInterrupt(digitalPinToInterrupt(vup), volup, FALLING);
  attachInterrupt(digitalPinToInterrupt(vdown), voldown, FALLING);
  attachInterrupt(digitalPinToInterrupt(pttr), pttt, FALLING);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

}

void loop() {
  recvDra();
  
}

void recvDra() {
  static byte ndx = 0;
  char endMark = '\n';
  char startMark = '+';
  char rc;
  static bool rxStart = false;

  while (Serial.available() > 0 && rxDone== false) {
    rc = Serial.read();
    
    rxStart = (rc==startMark)?rxStart:true;
    
    if(rxStart == true) {
      rxBuf[ndx] = rc;
      ndx++;
      if (ndx >= rxBufMax) {
        ndx = rxBufMax - 1;
      }
      if(rc == endMark) {
        rxBuf[ndx] = '\0'; // terminate the string
        ndx = 0;
        rxDone = true;
        rxStart = false;
      }
    }
  }
}

//functions
int reconfig() {
  //pins

  //serial
  //volume
  //setgroup

}

//interrupts

void chanup() {
  static unsigned long lastchanup = 0;
  if (millis() - lastchanup > 10) {
    freq = (freq > 21) ? 0 : (freq + 1);
    String sdata = "AT+DMOSETGROUP=1," + freqs[freq] + "," + freqs[freq] + "," + ctcss + "," + sql + "," + ctcss + '\r' + '\n';
    Serial.print(sdata);
  }
  lastchanup = millis();
}

void chandown() {
  static unsigned long lastchandown = 0;
  if (millis() - lastchandown > 10) {
    freq = (freq == 0) ? 22 : (freq - 1);
    String sdata = "AT+DMOSETGROUP=1," + freqs[freq] + "," + freqs[freq] + "," + ctcss + "," + sql + "," + ctcss + '\r' + '\n';
    Serial.print(sdata);
  }
  lastchandown = millis();
}

void volup() {
  static unsigned long lastvolup = 0;
  if (millis() - lastvolup > 10) {
    vol = (vol == 8) ? 0 : (vol + 1);
    String sdata = "AT+DMOSETVOLUME=" + vol + '\r' + '\n';
    Serial.print(sdata);
  }
  lastvolup = millis();
}

void voldown() {
  static unsigned long lastvoldown = 0;
  if (millis() - lastvoldown > 10) {
    vol = (vol == 0) ? 8 : (vol - 1);
    String sdata = "AT+DMOSETVOLUME=" + vol + '\r' + '\n';
    Serial.print(sdata);
  }
  lastvoldown = millis();
}

void pttt() {
  static unsigned long lastptt = 0;
  if (millis() - lastptt > 10) {
    transmit = !transmit;
    digitalWrite(pttw, !transmit);
  }
  lastptt = millis();
}
