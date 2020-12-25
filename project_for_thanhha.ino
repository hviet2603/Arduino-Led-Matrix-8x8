#include <LedControl.h>
#include <string.h>
int DIN = 9;
int CS =  8;
int CLK = 7;
int sndSrc = 6;   // use a second source for the switch
int switchPin = 5;
int RIGHT = 0; 
int LEFT = 1;
int UP = 0; 
int DOWN = 1;

LedControl lc = LedControl(DIN,CLK,CS,0);

int switchState = 0;
int lastSwitchState = 0;
int counter = -1;

// Facial Expressions
byte smile[8] = {0x3C,0x42,0xA5,0x81,0xA5,0x99,0x42,0x3C};
byte neutral[8] = {0x3C,0x42,0xA5,0x81,0xBD,0x81,0x42,0x3C};
byte sad[8] = {0x3C,0x42,0xA5,0x81,0x99,0xA5,0x42,0x3C};

//Symbols
byte heart[8] = {0x0,0x66,0xFF,0xFF,0xFF,0x7E,0x3C,0x18};
byte tree[8] = {0x18,0x3C,0x7E,0x18,0x3C,0x7E,0x18,0x18};
byte right[8] = {0x0,0x08,0x0C,0x7E,0x7E,0x0C,0x08,0x0};
byte left[8] = {0x0,0x10,0x30,0x7E,0x7E,0x30,0x10,0x0};
byte up[8] = {0x0,0x18,0x3C,0x7E,0x18,0x18,0x18,0x0};
byte down[8] = {0x0,0x18,0x18,0x18,0x7E,0x3C,0x18,0x0};

//Letters
byte space[8] = {0,0,0,0,0,0,0,0};
byte A[8] = {0x0,0x3C,0x66,0x66,0x7E,0x66,0x66,0x0};
byte H[8] = {0x0,0x66,0x66,0x7E,0x7E,0x66,0x66,0x0};
byte N[8] = {0x0,0x66,0x66,0x76,0x7E,0x6E,0x66,0x0};
byte T[8] = {0x0,0x7E,0x7E,0x18,0x18,0x18,0x18,0x0};


void setup() {
  // put your setup code here, to run once:
  lc.shutdown(0,false);       
  lc.setIntensity(0,1);       
  lc.clearDisplay(0);
  pinMode(sndSrc, OUTPUT);
  digitalWrite(sndSrc, HIGH);
  counter = 0;
  printByte(smile); 
}

void loop() {
  // put your main code here, to run repeatedly:
  switchState = digitalRead(switchPin);
  if (switchState != lastSwitchState)
  {
    if(switchState) counter++;
    lastSwitchState = switchState;
  };

  switch (counter%15)
  {
    case 0:
      printByte(smile);
      break;
    case 1:
      printByte(sad);
      break;
    case 2:
      printByte(neutral);
      break;
    case 3:
      printByte(heart);
      break;
    case 4:
      printByte(tree);
      break;
    case 5: 
      scroll_horizontal(right,RIGHT,100);
      break;
    case 6:
      scroll_horizontal(left,LEFT,100);
      break;
    case 7:
      scroll_vertical(up,UP,100);
      break;
    case 8:
      scroll_vertical(down,DOWN,100);
      break;
    case 9:
      scroll_text("THANH HA ", 200);
      break;
    default:
      lc.clearDisplay(0);
      counter = -1;
      break;
  };  
};

// Helper functions
void printByte(byte symbol [])
{
  int i = 0;
  for(i=0;i<8;i++)
  {
    lc.setRow(0,i,symbol[i]);
  }
};

void scroll_horizontal(byte symbol[], int direction, int delay_time) {
  boolean isInterrupted = false;
  while(1)
  {
    printByte(symbol);    
    for (int i = 0; i < 8; i++) {
      byte tmp = symbol[i];
      symbol[i] = (direction == RIGHT) ? symbol[i]>>1 : symbol[i]<<1;
      if(direction == RIGHT && tmp%2) symbol[i] += 128;
      if(direction == LEFT && tmp>=128) symbol[i] += 1;            
    };
    isInterrupted = detectInterrupt(delay_time);
    if (isInterrupted) break;
  }
};

void scroll_vertical(byte symbol[], int direction, int delay_time) {
  boolean isInterrupted = false;
  while(1)
  {
    printByte(symbol);
    byte tmp = (direction == UP) ? symbol[0] : symbol[7];
    for (int i = 0; i < 7; i++) {
      if(direction == UP) symbol[i] = symbol[i+1];
      if(direction == DOWN) symbol[7-i] = symbol[6-i];  
    }
    if(direction == UP) symbol[7] = tmp;
    if(direction == DOWN) symbol[0] = tmp;
    isInterrupted = detectInterrupt(delay_time);
    if (isInterrupted) break;  
  }
};

void scroll_text(const char* text, int delay_time) {
  int len = strlen(text);
  byte arr[len][8];
  for (int i = 0; i < len; i++) {
    switch (text[i]) {
      case 'A':
        memcpy(arr[i],A,8);
        break;
      case 'H':
        memcpy(arr[i],H,8);
        break;
      case 'N':
        memcpy(arr[i],N,8);
        break;
      case 'T':
        memcpy(arr[i],T,8);
        break;
      default:
        memcpy(arr[i],space,8);
        break;
    };    
  }; 

  boolean isInterrupted;
  int counter = 0;
  int current = 0;
  int next = 1;
  byte left[8];
  byte right[8];
  byte screen[8];

  while(1)
  {
    for (int i = 0; i < 8; i++){
      left[i] = arr[current][i] << counter;
      right[i] = arr[next][i] >> (8-counter);
    };
    for (int i = 0; i < 8; i++) {
      screen[i] = left[i] | right[i];      
    };
    printByte(screen);
    if (counter == 7) {
      current = (current == len-1) ? 0 : current + 1;
      next = (next == len-1) ? 0 : next + 1;    
    };
    counter = (counter == 7) ? 0 : counter + 1;
    isInterrupted = detectInterrupt(delay_time);
    if (isInterrupted) break;      
  };
  
};


// devide delay time into small delays to catch switch changes
boolean detectInterrupt(int delay_time) {
  boolean isInterupted = false;
  int n = delay_time/5;

  for (int i = 0; i < n; i++) {
    switchState = digitalRead(switchPin);
    if (switchState != lastSwitchState) {
      isInterupted = true;
      if (switchState) {
        counter++;
      }
      lastSwitchState = switchState;
    };
    delay(5); 
  };  

  return isInterupted; 
};

 
