#include <LiquidCrystal.h>

//lcd pin parameters
#define rs 2      
#define en 3        //lcd enable
#define d4 4
#define d5 5
#define d6 6      
#define d7 7

//input pins
#define cup 8       //channel up
#define cdown 9     //channel down
#define vup 10      //volume up
#define vdown 11    //volume down
#define squelch 12  //squelch from the dra818u
#define tx 1
#define rx 0        //serial

#define pd 13       //power down
#define hl A0       //high/low transmit power

//DRA818U Parameters- squelch level, CTCSS, Channel no., and volume
#define sql 1
#define ctcss "0000"

byte freq = 0, vol = 6;

const String freqs[] = {"462.5625", "462.5875", "462.6125", "462.6375", "462.6625", "462.6875", "462.7125", "467.5625", "467.5875",
                        "467.6125", "467.6375", "467.6625", "467.6875", "467.7125", "462.5500", "462.5750", "462.6000", "462.6250",
                        "462.6500", "462.6750", "462.7000", "462.7250"};
                        
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
  pinMode(pd, OUTPUT);
  pinMode(hl, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(cup), chanup, FALLING);
  attachInterrupt(digitalPinToInterrupt(cdown), chandown, FALLING);
  attachInterrupt(digitalPinToInterrupt(vup), volup, FALLING);
  attachInterrupt(digitalPinToInterrupt(vdown), voldown, FALLING);
  
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

void loop() {
  recvDra();
  if(rxDone) {
    responseId();
  }
}

//0 handshake complete, 
byte responseId() {
  switch(rxBuf[]) {
    case "+DMOCONNECT:0\r\n":
      return 0;
  }
}

void recvDra() {
  static byte ndx = 0;
  char endMark = '\n';
  char rc;
  static bool rxStart = false;

  while (Serial.available() > 0 && rxDone == false) {
    rc = Serial.read();
    
    rxStart = (rc == '+' || rc == 'S') ? rxStart : true;
    
    if(rxStart) {
      rxBuf[ndx] = rc;
      ndx++;
      if (ndx >= rxBufMax - 1) {
        ndx = rxBufMax - 2;
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
