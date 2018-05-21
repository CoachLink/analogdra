#include <LiquidCrystal.h>

//lcd pin parameters
#define RS 2      
#define EN 3          //lcd enable
#define D4 4
#define D5 5
#define D6 6      
#define D7 7

//input pins
#define VUP 10        //volume up
#define CUP 8         //channel up
#define CDOWN 9       //channel down
#define VDOWN 11      //volume down
#define SQ 12         //squelch pin from the dra818u
#define TX 1
#define RX 0          //serial

#define PD 13         //power down
#define HL A0         //high/low transmit power

//DRA818U Parameters
#define sql 1         //squelch level
#define ctcss "0000"  //CTCSS, generally off

byte freq = 0, vol = 6; //frequency and volume

const String freqs[] = {"462.5625", "462.5875", "462.6125", "462.6375", "462.6625", "462.6875", "462.7125", "467.5625", "467.5875",
                        "467.6125", "467.6375", "467.6625", "467.6875", "467.7125", "462.5500", "462.5750", "462.6000", "462.6250",
                        "462.6500", "462.6750", "462.7000", "462.7250"};

enum resp{HANDSHAKE = 1, FREQSCAN = 2, GROUPSET = 3, VOLUME = 4, SETFILTER = 5}response;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

//serial parameters
const byte rxBufMax = 32;
char rxBuf[32];
bool rxDone = false;


void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  for (int i = CUP; i <= SQ; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(PD, OUTPUT);
  pinMode(HL, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(CUP), chanup, FALLING);
  attachInterrupt(digitalPinToInterrupt(CDOWN), chandown, FALLING);
  attachInterrupt(digitalPinToInterrupt(VUP), volup, FALLING);
  attachInterrupt(digitalPinToInterrupt(VDOWN), voldown, FALLING);
  
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

byte responseId() {
  char *type = strtok(rxBuf, "=\r\n");
  
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
