/* Menu
      Adafruit         |  Inicjalizacja bilbioteki od ledow
      Inicjalizacja    |  Bindowanie przyciskow pod piny i zmienne
      Setup            |  Uruchomienie wszystkiego
      Lighting         |  Funcja od zmiany kolorow  light(r ,g ,b ,speed)
      Program Glowny   |  Switch z wyborem trybów
      Paleta patternow |  Switch z wyborem patternow
      UPDATE           |  Funkcja sprawdzajaca interacje uzytkownika
      Odczyt z PC      |  Funkcja odczytu z kodu pythonowego
      Patterny         |  Funkcje z patternami
*/


/* Dzialanie
      Po wpieciu usb (arduino) do pc (standardowo port COM3) ORAZ wpieciu drugiego usb od zasilania ledami 
      otrzymujemy dzialajacy program, nic nie powinno sie swiecic. Po kliknieciu przycisku skrajnego(pierwszego) 
      otrzymujemy kolor z wartosc 4 potencjometrow, 1-RED, 2-BLUE, 3-GREEN 4-BRIGHTNESS/SPEED możemy kontrolować live
      Drugie klikniecie - Kolor czerwony (buffor pomiedzy trybami/pusty tryb mozna zmienić)
      Trzecie klikniecie - Kolor Zielony (buffor pomiedzy trybami/pusty tryb mozna zmienić)
      Czwarte klikniecie - Kolor Niebieski (Tryb ustawienia patternow/animacji ledow, drugim przyciskiem zmieniamy patterny)
           0.C - Niebieski
           1.C - poscig 
           2.C - Tecza
           3.C - Tecza v2
           4.C - przebijany
           5.C - przebijany + tecza
           6.C - Powrot do 0
      Piate klikniecie - Wylaczone ledy
      Szoste klikniecie - Powrot na poczatek, utrzymuje stan z poprzedniego klikniecia takze wylaczone ledy, tryb odczytu z pc
      W tym trybie (rowniez zawsze gdy tylko odpala sie serial) można przejść do przekazywania wartość ze zlacza Serial
      po odpaleniu kodu pythona adruinoports.py mozemy wybrac tryby podobnie jak z przyciskow + dodatkowe opcje
          Tryb.0 - Wylacz ledy
          Tryb.1 - Kolor z kursora (kolor pojedynczego pixela na ktorym znajduje sie kursor)
          Tryb.2 - Kolor ze średniej z całego ekranu
          Tryb.3 - Statyczny kolor, wybor r,g,b 
          Tryb.4 - Wybor patternow, wybor trybu oraz paternu       
*/


/* ToDo
  problem z szumem (zmiania potencjonometrow na 10k) [nadal chuj]
  optymalizacja wywolania funkcji Update
  optymalizacja odczytu danych z pc (pierdolony print)
  uzycie shielda esp8266ex
  funkcja odczytu/zapisu zdalnego (WiFi/Bluetooth)
  funkcja kontrolowania/zapisu stanu (Http/POST/GET/IoT/FireBase/Sql etc)
  Dodanie wiekszej ilosc patternow
  Analiza danych (zapis i sprawdzanie sredniej kolorow z godziny etc)
  Aplikacja Pythonowa (Tkinter, PyQt5, Kivy etc)
*/

//*******************************************************************************
//************************************** Adafruit *******************************
//*******************************************************************************

#include <Adafruit_NeoPixel.h>
#define LED_PIN         7
#define NUMPIXELS      70 //Standardowa wartosc ledow
int header = 0xff; 
int len;
int rgb[4];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);


//*******************************************************************************
//************************************ Inicjalizacja ****************************
//*******************************************************************************


//Zmienne do przyciskow
const int buttonPin = 2;  
const int buttonPin2 = 3;  
int modeState, patternState, mode = 0, pattern = 0;

//Zmienne do potencjometrow
int potR = A0;
int potG = A1;
int potB = A2;
int potA = A3;
int potR_val, potG_val, potB_val, potA_val;


//*******************************************************************************
//***************************************** Setup *******************************
//*******************************************************************************


void setup() {
  Serial.begin(9600);
  pixels.begin(); //start bilbio od ledow
  pinMode(buttonPin, INPUT); //inicjowanie przycisku na pinie 2
  pinMode(buttonPin2, INPUT);//inicjowanie przycisku na pinie 3
}


//*******************************************************************************
//************************************* Lighting ********************************
//*******************************************************************************


void light(int r, int g, int b, int a = 255){ //ustawianie paska ledowego
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setBrightness(a);
      pixels.setPixelColor(i, pixels.Color(r,g,b));
      pixels.show();
    }
}


//*******************************************************************************
//********************************** Program Glowny *****************************
//*******************************************************************************


void loop() {
  Update();
  switch(mode){
    
  //Z powody budowy Arduino Uno Rev3 istnieje opcja automatycznego
  //resetowania ustawień przy odpalaniu Seriala więc odczyt z pc to opcja domyślna 
    
  case 0: ReadingFromPc();                            break; //Tryb odczytu kodu pythonowego
  case 1: light(potR_val,potG_val,potB_val,potA_val); break;  //Ustawianie reczne z potencjonometrow
  case 2: light(30,0,0);                              break;
  case 3: light(0,30,0);                              break;
  case 4: patternPicker();                            break; //Patterny
  case 5: light(0,0,0);                               break; //Ledy off

  }//endswitch
}//loop 


//*******************************************************************************
//******************************** Paleta patternow *****************************
//*******************************************************************************


void patternPicker(){ //funkcja wyboru patternow (drugi przycisk)
  Update();
  switch(pattern){
  
  case 0: light(0,0,20);                 break; //niebieski
  case 1: colorWipe(potA_val);           break; //poscig 
  case 2: rainbow(potA_val);             break; //tecza  
  case 3: rainbowCycle(potA_val);        break; //tecza v2   
  case 4: theaterChase(potA_val);        break; //przebijany
  case 5: theaterChaseRainbow(potA_val); break; //przebijany + tecza
   
  }//eof switch
}//eof patternPicker()


//*******************************************************************************
//************************************* UPDATE **********************************
//*******************************************************************************


//Funkcja sprawdzająca czy doszło do zmian przez użytkownika
void Update(){ //debouncing buttonów (mode,pattern) + pobieranie wartosc przyciski/analog
  potR_val = analogRead(potR)/4;
  potG_val = analogRead(potG)/4;
  potB_val = analogRead(potB)/4;
  potA_val = analogRead(potA)/4;
  
  int modeButton = digitalRead(buttonPin);
  if (modeButton != modeState) {
    modeState = modeButton;
    if (modeState == HIGH) {
      mode += 1;
      if(mode>5) mode = 0;
    }
  }
  
  int patternButton = digitalRead(buttonPin2); 
  if (patternButton != patternState) {
    patternState = patternButton;
    if (patternState == HIGH) {
      pattern += 1;
      if(pattern>=6) pattern = 0;
    }
  } 
} //eofUpdate()


//*******************************************************************************
//************************************* Odczyt z PC *****************************
//*******************************************************************************


void ReadingFromPc(){                       //do zmiany jeszcze (albo nie, print ratuje) [napisac magisterke dlaczego print ratuje]
    while (Serial.available() > 0) {        //ciagle oczekiwanie na dane
      Update();                             //Funkcja sprawdzajaca czy doszlo do zmiany przez uzytkownika (guziki itp)
      int dane = Serial.read();             //czytanie kazdego bitu
      if (dane == header){                  //jezeli pojawi sie naglowek (0xff) rozpocznij zbieranie danych
        delay(10);                          //(wazne) chwila na przejecie headera
        len = Serial.read();                //pobierz drugi bit czyli ilosc zmiennych w ciagu

  
        if(len == 2){                       //jezeli podano dokladnie 2 zmienna: mode,pattern
            mode = Serial.read();
            pattern = Serial.read();
        }//len2            

        
        if(len == 3){                       //jezeli podano dokladnie 3 zmienne: r,g,b
          for (int i=0 ; i<len; i++){
            rgb[i] = Serial.read();
          }
          for(int i=0;i<NUMPIXELS;i++){
            pixels.setPixelColor(i, pixels.Color(rgb[0],rgb[1],rgb[2]));
            pixels.show();
          }
        }//len3

  
        if(len == 4){                        //jezeli podano dokladnie 4 zmienne: r,g,b oraz ile diod
          for (int i=0 ; i<len; i++){
            //delay(10);
            rgb[i] = Serial.read();
            Serial.print(rgb[i]);
          }
          for(int i=0;i<rgb[3];i++){
            pixels.setPixelColor(i, pixels.Color(rgb[0],rgb[1],rgb[2]));
            pixels.show();
          }
        }//len4

   
      }//if dane => header
   }//while Serial.available() 
}//eof ReadingFromPc()


//*******************************************************************************
//****************************************Patterny*******************************
//*******************************************************************************
//*************************************Ten kod to jebac**************************
//*************************************Nie czytac dalej**************************
//*******************************************************************************
//*******************************************************************************


void colorWipe(uint8_t wait) { //1
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
      if(mode!=4 or pattern!=1)break;
      Update();
      wait = potA_val;
      pixels.setPixelColor(i, potR_val,potG_val,potB_val);
      pixels.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) { //2
  uint16_t i, j;
  for(j=0; j<256; j++) {
    if(mode!=4 or pattern!=2)break;
    Update();
    wait = potA_val;
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) { //3
  uint16_t i, j;
  for(j=0; j<256*5; j++) { 
    if(mode!=4 or pattern!=3)break;
    for(i=0; i< pixels.numPixels(); i++) {
      if(mode!=4 or pattern!=3)break;
      Update();
      wait = potA_val;
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void theaterChase(uint8_t wait) { //4
  for (int j=0; j<10; j++) {
    if(mode!=4 or pattern!=4)break;
    for (int q=0; q < 3; q++) {
      if(mode!=4 or pattern!=4)break;
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        if(mode!=4 or pattern!=4)break;
        Update();
        wait = potA_val;
        pixels.setPixelColor(i+q, potR_val,potG_val,potB_val);  
      }
      pixels.show();
      
      delay(wait);
      
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        if(mode!=4 or pattern!=4)break;
        Update();
        wait = potA_val;
        pixels.setPixelColor(i+q, 0);       
      }
    }
  }
}

void theaterChaseRainbow(uint8_t wait) { //5
  for (int j=0; j < 256; j++) {    
    if(mode!=4 or pattern!=5)break;
    
    for (int q=0; q < 3; q++) {
      Update();
      wait = potA_val;
      if(mode!=4 or pattern!=5)break;
      
      for (uint16_t i=0; i < NUMPIXELS; i=i+3) {
        if(mode!=4 or pattern!=5)break;
        pixels.setPixelColor(i+q, Wheel( (i+j) % 255));   
      }
      pixels.show(); 
      delay(wait);
      for (uint16_t i=0; i < NUMPIXELS; i=i+3) {
        if(mode!=4 or pattern!=5)break;
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
