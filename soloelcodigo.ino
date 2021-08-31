// JHON EDISON VARGAS VARGAS jhevargasv@correo.udistrital.edu.co
// Reproductor musical haciendo uso de ADC.

// ***** librerias y pines ****
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

//Pin RESET conectado a Vcc (+3.3v)
//Pin LED conectado a +3.3V
#define CS 5 //CS0
#define DC 3 //A0
#define MOSI 23 //SDA
#define SCK 18 //CLK
TFT_ILI9163C tft=TFT_ILI9163C(CS,DC); //Se crea objeto de nombre tft
#define F0 17
#define F1 16
#define F2 19
#define F3 21
#define C0 13
#define C1 12
#define C2 14
#define C3 27 
#define DAC1 25


#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CIAN    0x07FF
#define MAGENTA 0xF81F
#define ORANGE  0xFD80


// **** Variables globales ****
volatile uint8_t t,bandera,key,key_f;
boolean flagplay = false;
volatile uint32_t rebote;
uint32_t t_actual,t_anterior,tiempos;
uint8_t scan,playpausa=0,menu;
uint8_t counter=0;
char tecla;   //yi=36
uint8_t xi=7, yi=46, diametro=5, alto=50,voltotal =100;
const uint32_t * misimagenes[4] = {ofchange,lapena,anotherlove1,arcad};
const unsigned char * miscanciones[4] = {rawData,lapena1,anotherlove,arcade};
// **** Interrupciones. ****
void IRAM_ATTR Int_F0(){
  switch(t){ 
    case 0: //0xEE
      key='1';
    break;
    case 1: //0xED
      key='2';
    break;
    case 2: //0xEB 
      key='3';
    break;
  }
  key_f=1; //Indica que se oprimio una tecla
  detachInterrupt(digitalPinToInterrupt(F0)); //Deshabilitar int. del pulsador
  bandera=1;
  rebote=millis();
}
void IRAM_ATTR Int_F1(){
  switch(t){
    case 0: //0xDE
      key='4';
    break;
    case 1: //0xDD
      key='5';
    break;
    case 2: //0xDB 
      key='6';
    break;
  }
  key_f=1; //Indica que se oprimio una tecla
  detachInterrupt(digitalPinToInterrupt(F1)); //Deshabilitar int. del pulsador
  bandera=1;
  if(playpausa==0){
    playpausa=1;
  }else{
    playpausa=0;
  }
  menu=0;
  rebote=millis();
}

void dibujarImagen(uint8_t xi,uint8_t yi,uint8_t size_x, uint8_t size_y, const uint32_t *bitmap,uint8_t tipo){
  uint16_t dato=0;
  
  if(tipo==0){ //Formato RGB565
    for(uint8_t y=yi;y<yi+size_y;y++){
      for(uint8_t x=xi;x<xi+size_x;x++){
        tft.drawPixel(x,y,bitmap[dato]);
        dato++;
      }
    }
  }else{ //Formato RGB
    for(uint8_t y=yi;y<yi+size_y;y++){
      for(uint8_t x=xi;x<xi+size_x;x++){
        tft.drawPixel(x,y,tft.Color24To565(bitmap[dato]));
        dato++;
      }
    }
  }
}

void dibujar(){
  //info
  tft.fillRect(0, 0, 128, 26, BLUE); //fila[x], columna[y], ancho, altura
  //piture
  tft.fillRect(19, 26, 90, 70, RED); //fila[x], columna[y], ancho, altura
  //control
  tft.fillRect(0, 96, 128, 32, BLUE); //fila[x], columna[y], ancho, altura
  //play
  //tft.fillCircle(64,112,10,BLACK);
  tft.fillTriangle(72,110,64,100,64,120,WHITE);
  //skipt left
  tft.fillTriangle(21,110,29,100,29,120,WHITE);
  tft.fillRect(19, 100, 4, 20, WHITE); 
  //skipt right
  tft.fillTriangle(107,110,99,100,99,120,WHITE);
  tft.fillRect(106, 100, 4, 20, WHITE); 
  //time
  //tft.fillRect(19, 90, 90, 5, WHITE); 
  tft.setTextSize(0.3);
  tft.setTextColor(WHITE,BLACK);
  tft.setCursor(2,110);
  tft.print(counter);  
  
}

void setup(){
  //Serial.begin(115200);
  randomSeed(140584);
  tft.begin();
  pinMode(C0,OUTPUT);
  pinMode(C1,OUTPUT);
  pinMode(C2,OUTPUT);
  pinMode(C3,OUTPUT);
  pinMode(F0,INPUT_PULLUP);
  pinMode(F1,INPUT_PULLUP);
  //pinMode(F2,INPUT_PULLUP);
  //pinMode(F3,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(F0),Int_F0,FALLING);
  attachInterrupt(digitalPinToInterrupt(F1),Int_F1,FALLING);
  //attachInterrupt(digitalPinToInterrupt(F2),Int_F2,FALLING);
  //attachInterrupt(digitalPinToInterrupt(F3),Int_F3,FALLING);
  digitalWrite(C0,0); digitalWrite(C1,1); digitalWrite(C2,1); digitalWrite(C3,1);
  t=0;
  t_anterior=0;
  key_f=0;
  bandera=0;
  flagplay=0;
  scan=60; //Barrido del teclado de 20ms  
  dibujar();
  tft.setTextSize(0.3);
}

void reproducir(uint8_t counter, char tecla){
    for(int i=0;i<661488;i++){           
        dacWrite(DAC1,(miscanciones[counter])[i]);
        delayMicroseconds(38);    //36
        //Se imprime unicamente en datos exactos para no sobrecargar el procesador
        if(i%22050 == 0){        
          tiempos=(i/22050);
          tft.setTextSize(0.3);
          tft.setTextColor(WHITE,BLACK);
          tft.setCursor(7,88);    
          tft.print(tiempos);    
          tft.print(" ");
          tft.fillRect(19, 90, i/7350, 5, WHITE); 
        }
      }
  } 
      


void volum(uint16_t vol,uint16_t tm){
  tft.setTextSize(0.3);
  tft.setTextColor(WHITE,BLACK);
  tft.setCursor(2,30);
  tm = vol*(voltotal/4095.0);
  tft.print(tm,0);
  tft.print(" ");
  tft.fillRect(7, yi+(voltotal/2-tm/2),5,tm/2,RED);  
  tft.fillRect(7, yi,5,(voltotal/2-tm/2),BLACK);
}

void next(uint8_t counter, char tecla){
  //Dibujar imagen correspondiente
  dibujarImagen(19,26,90,70,misimagenes[counter],0);  

  //Reproducir cancion.
  reproducir(counter,tecla);
} 




void play(char tecla){  
  switch(tecla){
    case '1':       //return
      tft.fillTriangle(107,110,99,100,99,120,BLACK);
      tft.fillRect(106, 100, 4, 20, BLACK); 
      delay(500);      
      tft.fillTriangle(107,110,99,100,99,120,WHITE);
      tft.fillRect(106, 100, 4, 20, WHITE); 
      delay(500); 

      //Aumentar el contador
      counter++;
      if(counter>3){
       counter=0;
      }
      tft.setTextSize(0.3);
      tft.setTextColor(WHITE,BLACK);
      tft.setCursor(2,110);
      tft.print(counter);
      tft.print(" ");
      

      next(counter,tecla);
    break;
    case '2':       //Play/Pause
      tft.fillTriangle(72,110,64,100,64,120,BLACK);
      delay(500);      
      tft.fillTriangle(72,110,64,100,64,120,WHITE);
      delay(500);
      tft.setTextSize(0.3);
      tft.setTextColor(WHITE,BLACK);
      tft.setCursor(111,88); 
      tft.print(676800/22050,0);
      dibujarImagen(19,26,90,70,misimagenes[counter],0);
      //piture      
      flagplay = !flagplay; // invertir el valor.
      reproducir(counter,tecla);       
    break;
    case '3':      //Skip
      tft.fillTriangle(21,110,29,100,29,120,BLACK);
      tft.fillRect(19, 100, 4, 20, BLACK); 
      delay(500);      
      tft.fillTriangle(21,110,29,100,29,120,WHITE);
      tft.fillRect(19, 100, 4, 20, WHITE); 
      delay(500);

      //Aumentar el contador
      counter--;
      if(counter==255){
        counter=3;
      }
      tft.setTextSize(0.3);
      tft.setTextColor(WHITE,BLACK);
      tft.setCursor(2,110);
      tft.print(counter);
      tft.print(" ");

      next(counter,tecla);        
    break;
    case '5':
      reproducir(counter,tecla);  
    break;



  }  
}


void loop() {
    t_actual=millis();
  if(t_actual-t_anterior>scan){
    t++;
    if(t==4){t=0;}
    t_anterior=t_actual;
    switch(t){
      case 0:
        digitalWrite(C0,0); digitalWrite(C1,1); digitalWrite(C2,1); digitalWrite(C3,1);
      break;
      case 1:
        digitalWrite(C0,1); digitalWrite(C1,0); digitalWrite(C2,1); digitalWrite(C3,1);
      break;
      case 2:
        digitalWrite(C0,1); digitalWrite(C1,1); digitalWrite(C2,0); digitalWrite(C3,1);
      break;
      case 3:
        digitalWrite(C0,1); digitalWrite(C1,1); digitalWrite(C2,1); digitalWrite(C3,0);
      break;
    }
    if(key_f==1){   
      tecla = char(key);
      key_f=0;
    }
    if(millis()-rebote>300&&bandera){
      tecla='0';
      attachInterrupt(digitalPinToInterrupt(F0),Int_F0,FALLING); //Habilita nuevamente int. de la F0
      attachInterrupt(digitalPinToInterrupt(F1),Int_F1,FALLING); //Habilita nuevamente int. de la F1
      //attachInterrupt(digitalPinToInterrupt(F2),Int_F2,FALLING); //Habilita nuevamente int. de la F2
      //attachInterrupt(digitalPinToInterrupt(F3),Int_F3,FALLING); //Habilita nuevamente int. de la F3
      bandera=0;
    }
  }
  
  uint16_t vol=analogRead(4);
  uint16_t tm=(vol,0,4095,0,100);
  
  volum(vol,tm);
  play(tecla);
  
}
