
//Zmienne do ledow
#include <Adafruit_NeoPixel.h>
#define LED_PIN         7 
#define NUMPIXELS      70 //Standardowa wartosc ledow
int header = 0xff; 
int len;
int rgb[4];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

//Zmienne do przyciskow
const int buttonPin = 2;  
const int buttonPin2 = 3;  
int modeState, mode = 0;

//Zmienne do potencjometrow
int potR = A0;
int potG = A1;
int potB = A2;
int potA = A3;
int potR_val, potG_val, potB_val, potA_val;


void setup() {
  Serial.begin(9600);
  pixels.begin(); //start bilbio od ledow
  pinMode(buttonPin, INPUT); //inicjowanie przycisku na pinie 2
  pinMode(buttonPin2, INPUT);//inicjowanie przycisku na pinie 3
}

void light(int r, int g, int b, int a = 255){ //ustawianie paska ledowego
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setBrightness(a);
      pixels.setPixelColor(i, pixels.Color(r,g,b));
      pixels.show();
    }
}



void loop() {
  //Program glowny
  checkingPot(); //sprawdzanie wartosc dla potencjometrow
  checkingMode(); //sprawdzanie czy nacisnieto przycisk
  
  switch(mode){

    //Z powody budowy Arduino Uno R3 istnieje opcja automatycznego
    //resetowania ustawień przy odpalaniu Seriala więc jest to opcja domyślna    
  case 0: //Tryb odczytu kodu pythonowego (podstawowa)
    ReadingFromPc();
    break;

  case 1: //Ustawianie reczne z potencjonometrow
  //problem z szumem (zmiania potencjonometrow na 10k
    light(potR_val,potG_val,potB_val,potA_val);
    break;

  case 2: //patterny (1)
    light(30,0,0);
    break;

  case 3: //patterny (2)
    light(0,30,0);
    break;


  case 4: //patterny (3)
    theaterChaseRainbow(potA_val);
    break; 

  case 5: //led off
    light(0,0,0);
    break; 

  }//endswitch
}//loop 


void checkingPot(){
  potR_val = analogRead(potR)/4;
  potG_val = analogRead(potG)/4;
  potB_val = analogRead(potB)/4;
  potA_val = analogRead(potA)/4;
}

void checkingMode(){ //debouncing buttonów (mode)
  int modeButtonForward = digitalRead(buttonPin);
  int modeButtonBackward = digitalRead(buttonPin2);
  
  if (modeButtonForward != modeState) {
    modeState = modeButtonForward ;
    modeButtonBackward = modeState;
    if (modeState == HIGH) {
      mode += 1;
      if(mode>5) mode = 0;
        delay(20);
    }
  }

  if (modeButtonBackward != modeState) {
    modeState = modeButtonBackward ;
    modeButtonForward = modeState;
    if (modeState == HIGH) {
      mode -= 1;
      if(mode<0) mode = 5;
        delay(20);
    }
  }
}

void ReadingFromPc(){  //do zmiany jeszcze
    while (Serial.available() > 0) { //ciagle oczekiwanie na dane
      int dane = Serial.read(); //czytanie kazdego bitu
      if (dane == header){ //jezeli pojawi sie naglowek (0xff) rozpocznij zbieranie danych
        delay(10); //(wazne) chwila na przejecie headera
        len = Serial.read(); //pobierz drugi bit czyli ilosc zmiennych w tablicy
  
        if(len == 3){ //jezeli podano dokladnie 3 zmienne: r,g,b
          for (int i=0 ; i<len; i++){
            //delay(10);
            rgb[i] = Serial.read();
          }
          for(int i=0;i<NUMPIXELS;i++){
            pixels.setPixelColor(i, pixels.Color(rgb[0],rgb[1],rgb[2]));
            pixels.show();
          }
        }
  
        if(len == 4){ //jezeli podano dokladnie 4 zmienne: r,g,b oraz ktora dioda
          for (int i=0 ; i<len; i++){
            Update();
            delay(10);
            rgb[i] = Serial.read();
            Serial.print(rgb[i]);

          }
          for(int i=0;i<rgb[3];i++){
            pixels.setPixelColor(i, pixels.Color(rgb[0],rgb[1],rgb[2]));
            pixels.show();
          }
        }
      }//czyheader
   }//czydane>0   
}


//do wyjebania
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    if(mode!=4)break;
    
    for (int q=0; q < 3; q++) {
      checkingMode();
      checkingPot();
      wait = potA_val;
      if(mode!=4)break;
      
      for (uint16_t i=0; i < NUMPIXELS; i=i+3) {
        pixels.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      pixels.show(); 
      
      delay(wait);

      for (uint16_t i=0; i < NUMPIXELS; i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}


uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
