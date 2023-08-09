#include <SoftwareSerial.h>
#include <EEPROM.h>
//RDM6300 conf
int readed[10];
const int BUFFER_SIZE = 14; // RFID DATA FRAME FORMAT: 1byte head (value: 2), 10byte data (2byte version + 8byte tag), 2byte checksum, 1byte tail (value: 3)
const int DATA_SIZE = 10; // 10byte data (2byte version + 8byte tag)
const int DATA_VERSION_SIZE = 2; // 2byte version (actual meaning of these two bytes may vary)
const int DATA_TAG_SIZE = 8; // 8byte tag
const int CHECKSUM_SIZE = 2; // 2byte checksum
SoftwareSerial ssrfid = SoftwareSerial(6,8); 
int psu_rdm = 12;
uint8_t buffer[BUFFER_SIZE]; // used to store an incoming data frame 
int buffer_index = 0;
#include <ezButton.h>
//oled libs
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//oled conf
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ezButton button1(10);  // create ezButton object that attach to pin 6;
ezButton button2(11);  // create ezButton object that attach to pin 7;

int cY = 0;
int cX = 10;
unsigned long StartTime = millis();
int currentFrame = 0 ; //1-Main Frame, 2-Reader Frame, 3-Emulator frame, 4- Copier Frame
String incoming = "";
//emulator variables
int opto = 4;
int coil_pin = 9;
String load;
int header[9] = {1,1,1,1,1,1,1,1,1};
int payload[]= {
  0,0,0,0,0, //Segment 1
  0,0,0,0,0, //Segment 2
  0,0,0,0,0, //Segment 3
  0,0,0,0,0, //Segment 4
  0,0,0,0,0, //Segment 5
  0,0,0,0,0, //Segment 6
  0,0,0,0,0, //Segment 7
  0,0,0,0,0, //Segment 8
  0,0,0,0,0, //Segment 9
  0,0,0,0,0 //Segment 10
  };
int parity[5] = {0,0,0,0,0};

const uint8_t PROGMEM nfcat[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x01, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x02, 0x82, 0x20, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x04, 0x81, 0xa0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xf8, 0x80, 0x50, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x40, 0x50, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x30, 0xcf, 0xf0, 0xf0, 0x3c, 0x3f, 0xf0, 0x10, 0x00, 0x40, 0x90, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x30, 0xcf, 0xf0, 0xf0, 0x3c, 0x3f, 0xf0, 0x13, 0x06, 0x40, 0xa0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3c, 0xcc, 0x03, 0x0c, 0xc3, 0x03, 0x00, 0x10, 0x00, 0x7f, 0x20, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3c, 0xcc, 0x03, 0x0c, 0xc3, 0x03, 0x00, 0x10, 0xa8, 0x40, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x33, 0xcf, 0xc3, 0x00, 0xff, 0x03, 0x00, 0x08, 0x50, 0x80, 0x40, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x33, 0xcf, 0xc3, 0x00, 0xff, 0x03, 0x00, 0x06, 0x03, 0x00, 0x40, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x30, 0xcc, 0x03, 0x0c, 0xc3, 0x03, 0x00, 0x01, 0xfc, 0x00, 0x40, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x30, 0xcc, 0x03, 0x0c, 0xc3, 0x03, 0x00, 0x00, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x30, 0xcc, 0x00, 0xf0, 0xc3, 0x03, 0x00, 0x00, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x30, 0xcc, 0x00, 0xf0, 0xc3, 0x03, 0x00, 0x00, 0x40, 0x00, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c, 0xf5, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x8a, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
void setup() {
  pinMode(opto, OUTPUT);
  pinMode(coil_pin, OUTPUT);
  digitalWrite(coil_pin, LOW);
  digitalWrite(opto, LOW);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  //Serial.println(F("SSD1306 allocation failed"));
  for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.clearDisplay();
  display.setTextSize(1.5); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  
  //Serial.begin(9600);
  button1.setDebounceTime(50); // set debounce time to 50 milliseconds
  button2.setDebounceTime(50); // set debounce time to 50 milliseconds
  pinMode(psu_rdm, OUTPUT);
  display.drawBitmap(0,0, nfcat, 128, 32, WHITE);
  display.display();
  delay(1000);
    mainFrame(); 
    mainCursor(0);
}

void loop() {
  button1.loop(); // MUST call the loop() function first
  button2.loop(); // MUST call the loop() function first
  display.setCursor(0, 0);

  if(button1.isPressed()){
    StartTime = millis();
    }
    
  if(button1.isReleased()){
    unsigned long CurrentTime = millis();
    unsigned long ElapsedTime = CurrentTime - StartTime;    
    if (ElapsedTime < 1000){
    if (currentFrame == 6 || currentFrame == 3) {
      if (cX < 106){
      cX = cX + 24;
      memoryCursor(cX);
      }
      else {cX = 10;memoryCursor(cX);}
    }
    else {
    cY = cY+10;    
    if (cY >=30) { cY=0;}
    mainCursor(cY);}
    }
    if (ElapsedTime > 1000)  {enter();}
    
  }
      
  
  if(button2.isPressed()){ 
    StartTime = millis();

  }
  if(button2.isReleased()){
    unsigned long CurrentTime = millis();
    unsigned long ElapsedTime = CurrentTime - StartTime;
      if (ElapsedTime < 1000){ 
      cY = cY-10;
      if (cY <0) { cY=20;}
      mainCursor(cY);}
      if (ElapsedTime > 1000)  {select();}
    }
  }
void mainFrame(void) {
  display.clearDisplay();
  display.setCursor(20, 0);
  display.setTextSize(1);
  display.println("READ");
  display.setCursor(20, 11);
  display.println("EMULATE");
  display.setCursor(20, 21);
  display.println("CREDIT");
  display.display();
  currentFrame = 1;
}
void readerFrame(void) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("READ");
  display.display();
  currentFrame = 2;
  ssrfid.begin(9600);
  ssrfid.listen(); 
  reading();
}

void emuFrame(void) {
  currentFrame = 3;
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10,0);
  for(int m=1; m<6; m++) {
    display.print(m);
    display.print(" ");    
  }
  display.display();
  memoryCursor(10);
}


long mainCursor(int Y) {
  display.fillRect(0, 0, 18, 32,SSD1306_BLACK);
  display.setCursor(6,Y);
  display.println(">");
  display.display();
  
  }
long memoryCursor(int X) {
  display.fillRect(0, 20, 128, 32,SSD1306_BLACK);
  display.setCursor(X,25);
  display.println("^");
  display.display();
  }

void enter(){
  if (currentFrame == 1) {
  switch(cY){
    case 0:
      readerFrame();
      return;
    case 10:
      emuFrame();
      break;
    case 20:
      credit();
      break;    
      }
  }
  else if (currentFrame ==6) {
    display.setTextSize(1);
    switch(cX){
      case 10:
      display.clearDisplay();
      display.print("Saving to SLOT 1");
      display.display();
      code(incoming);
      for (int eep=0; eep<50; eep++) {
        EEPROM.write(eep,payload[eep]);
      }
      for (int eep=50; eep<55; eep++) {
        EEPROM.write(eep, parity[eep-50]); 
      }
      delay(1000);
      resetVars();
      mainFrame();
      break;
      case 34: //Buffer 2
      display.clearDisplay();
      display.print("Saving to SLOT 2");
      display.display();
      code(incoming);
      for (int eep=55; eep<105; eep++) {
        EEPROM.write(eep,payload[eep-55]);
      }
      for (int eep=105; eep<110; eep++) {
        EEPROM.write(eep, parity[eep-105]); 
      }
      delay(1000);
      resetVars();
      mainFrame();
      break;
      case 58: //Buffer 3
      display.clearDisplay();
      display.print("Saving to SLOT 3");
      display.display();
      code(incoming);
      for (int eep=110; eep<160; eep++) {
        EEPROM.write(eep,payload[eep-110]);
      }
      for (int eep=160; eep<165; eep++) {
        EEPROM.write(eep, parity[eep-160]); 
      }
      delay(1000);
      resetVars();
      mainFrame();
      break;
      case 82: //Buffer 4
      display.clearDisplay();
      display.print("Saving to SLOT 4");
      display.display();
      code(incoming);
      for (int eep=165; eep<215; eep++) {
        EEPROM.write(eep,payload[eep-165]);
      }
      for (int eep=215; eep<220; eep++) {
        EEPROM.write(eep, parity[eep-215]); 
      }
      delay(1000);
      resetVars();
      mainFrame();
      break;
      case 106: //Buffer 5
      display.clearDisplay();
      display.print("Saving to SLOT 5");
      display.display();
      code(incoming);
      for (int eep=220; eep<270; eep++) {
        EEPROM.write(eep,payload[eep-220]);
      }
      for (int eep=270; eep<275; eep++) {
        EEPROM.write(eep, parity[eep-270]); 
      }
      delay(1000);
      resetVars();
      mainFrame();
      break;                        
    }
  }
    else if (currentFrame ==3) {
    display.setTextSize(1);
    switch(cX){
      case 10:
        emulate_buffer1();
        break;
      case 34:
        emulate_buffer2();
        break;
      case 58:
        emulate_buffer3();
        break;
      case 82:
        emulate_buffer4();
        break;
      case 106:
        emulate_buffer5();
        break;                        
    }
  }
  
  else {display.clearDisplay();}
  if (currentFrame == 5) {
    display.clearDisplay();
    display.setTextSize(1);
    code(incoming);
    //display.print(load);

    display.display();
    while(true) {
    digitalWrite(opto, HIGH);
    emulate(header, payload, parity);
    digitalWrite(opto, LOW);
      }
    }
}
void select(){
    if (currentFrame ==1) {
      mainFrame();cY=0;mainCursor(cY);
    }
    if (currentFrame == 5) {
      memoryFrame();
      memoryCursor(cX);
    }
    

}
void memoryFrame() {
  currentFrame = 6;
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10,0);
  for(int m=1; m<6; m++) {
    display.print(m);
    display.print(" ");    
  }
  display.display();
}

void credit() {
display.clearDisplay();
display.print("made with <3 \n\nby crashlogs.com ");
display.display();
}

void resetVars() {
  String incoming = "";

  String load;
  int header[9] = {1,1,1,1,1,1,1,1,1};
  int payload[]= {
    0,0,0,0,0, //Segment 1
    0,0,0,0,0, //Segment 2
    0,0,0,0,0, //Segment 3
    0,0,0,0,0, //Segment 4
    0,0,0,0,0, //Segment 5
    0,0,0,0,0, //Segment 6
    0,0,0,0,0, //Segment 7
    0,0,0,0,0, //Segment 8
    0,0,0,0,0, //Segment 9
    0,0,0,0,0 //Segment 10
    };
  int parity[5] = {0,0,0,0,0};
}
