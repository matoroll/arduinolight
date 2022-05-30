
//Zmienne do ledow
#include <Adafruit_NeoPixel.h>
#define LED_PIN         7 
#define NUMPIXELS      12 //Standardowa wartosc ledow
int header = 0xff; 
int len;
int rgb[4];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

//Zmienne do przyciskow
const int buttonPin = 2;  
const int buttonPin2 = 3;  
int modeState, patternState, mode = 0, pattern = 0;



void setup() {
  Serial.begin(9600);
  pixels.begin(); //start bilbio od ledow
  pinMode(buttonPin, INPUT); //inicjowanie przycisku na pinie 2
  pinMode(buttonPin2, INPUT);//inicjowanie przycisku na pinie 3
}


void loop() {

  //buttony
  int modeButton = digitalRead(buttonPin);
  int patternButton = digitalRead(buttonPin2); 

  if (modeButton != modeState) {
    modeState = modeButton;
    if (modeState == HIGH) {
      Serial.print(mode);
      mode += 1;
      if(mode>3) mode = 0;
      delay(20);
    }
  }
  
  if (patternButton != patternState) {
    patternState = patternButton;
    if (patternState == HIGH) {
      Serial.print(pattern);
      pattern += 1;
      if(pattern>5) pattern = 0;
      delay(20);
    }
  }   

  //Program glowny

  while (mode==0){ //Tryb wylaczonych ledow //bazowy
    //light(0,0,0);
  }
  
  while (mode==1){ //Tryb odczytu danych z pc (python script)
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
            //delay(10);
            rgb[i] = Serial.read();
          }
          for(int i=0;i<rgb[3];i++){
            pixels.setPixelColor(i, pixels.Color(rgb[0],rgb[1],rgb[2]));
            pixels.show();
          }
        }
      }//czyheader
    }//czydane>0   
  }
}//loop 


void light(int r, int g, int b){
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(r,g,b));
      pixels.show();
    }
}
