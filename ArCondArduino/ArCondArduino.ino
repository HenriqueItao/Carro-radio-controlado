#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <string.h>
#include <VirtualWire.h>
 
#define TAMANHOMAXSENHA 9
#define SENHAVALIDA "01234"
#define MINPRESSURE 100
#define MAXPRESSURE 2000
#define PRETO   0x0000
#define VERMELHO     0xF800
#define VERDE   0x07E0
#define BRANCO  0xFFFF
#define AZUL    0x001F
#define CIANO   0x07FF
#define MAGENTA 0xF81F
#define AMARELO 0xFFE0
 
#define XP 7
#define XM A1
#define YP A2
#define YM 6
 
#define TS_LEFT 921
#define TS_RT 200
#define TS_TOP 207
#define TS_BOT  889

 
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button botoes[4];
MCUFRIEND_kbv tft;
 
bool pressionado = false;
int posX;
int posY;
String senha = "";
int temp =17; 
int Botao;
const int pinoSensor = 30; //PINO DIGITAL UTILIZADO PELO SENSOR
const int pinRF = 28;
int Porta =0;
char Valor_CharMsg[10];
char codigo[8];
char Valor_estado[2];
 
bool obterToque(void){
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE);
    if (pressed) {
        posX = map(p.y, TS_LEFT, TS_RT, 0, 320); 
        posY = map(p.x, TS_TOP, TS_BOT , 0, 240);
        return true;
    }
    return false; 
}

void setup() {
  Serial.begin(9600);
  pinMode(pinoSensor, INPUT_PULLUP); //DEFINE O PINO COMO ENTRADA / "_PULLUP" É PARA ATIVAR O RESISTOR INTERNO
  Serial1.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  telaInicial();
  vw_set_tx_pin(pinRF); //Pino ligado ao pino DATA do transmissor RF
  vw_setup(2000);   //Velocidade de comunicacao (bits por segundo)
  Serial.println("Transmissao modulo RF - Movimente - se");  
}

void telaInicial(){
  senha = "";
  tft.fillScreen(PRETO);

  botoes[0].initButton(&tft, 50, 65, 70, 110, BRANCO, CIANO, PRETO, "+", 6);
  botoes[1].initButton(&tft, 50, 180, 70, 110, BRANCO, CIANO, PRETO, "-", 6);
  botoes[2].initButton(&tft, 145, 200, 105, 65, BRANCO, VERDE, PRETO, "On/Off", 2);
  botoes[3].initButton(&tft, 265, 200, 105, 65, BRANCO, VERDE, PRETO, "Modo", 2);
 
  for (int i=0; i<4; i++){
    botoes[i].drawButton(false);
  }
  escreveLCD();
  
}

void TeclaPressionada(bool teclaTocada){
  for (int i=0; i<4; i++){
    botoes[i].press(teclaTocada && botoes[i].contains(posX,posY));
  }
  checkPressedButton();
 
}

void checkPressedButton(){
  for (int i=0; i<4; i++){
    if (botoes[i].justPressed()){
      checkPressed(i);
    }
  }
}

void esperaSoltar(){
  while (obterToque()){
    delayMicroseconds(100);
  }
}

void escreveLCD(){ 
  tft.fillRect(95, 10, 220, 140, AMARELO);  
  tft.setCursor(100,20); 
  tft.setTextColor(PRETO); 
  tft.setTextSize(3);
  tft.print("Temperatura");
  tft.setCursor(130,60);
  tft.setTextColor(PRETO); 
  tft.setTextSize(5);
  tft.print(temp); 
  tft.setCursor(190,60);
  tft.setTextColor(PRETO); 
  tft.setTextSize(2);
  tft.print("o");
  tft.setCursor(205,60);
  tft.setTextColor(PRETO); 
  tft.setTextSize(5);
  tft.print("C");
  if(Porta==1){
    tft.setCursor(100,100);
    tft.setTextColor(PRETO); 
    tft.setTextSize(3);
    tft.print("Porta Aberta");
  }
  else{
    tft.setCursor(100,100);
    tft.setTextColor(PRETO); 
    tft.setTextSize(3);
    tft.print("Porta");
    tft.setCursor(100,125);
    tft.setTextColor(PRETO); 
    tft.setTextSize(3);
    tft.print("Fechada");
  }
}

void clear(char* var){
  for (int k=0; k < strlen(var); k++){
    var[k] = 0;
  }
}

void checkPressed(int button){
    botoes[button].drawButton(true);
    esperaSoltar();
    botoes[button].drawButton();
    if(senha.length()<TAMANHOMAXSENHA || button == 10 || button == 11){
    switch (button){
        case 0:
          Botao = 221;
        break;
        case 1:
          Botao = 222;
        break;
        case 2:
          Botao = 223;
        break;
        case 3:
 //         Botao = 3;
          Botao = 221;        
        break;
    }
    Serial.println(Botao);
    Serial1.write(Botao);
    delay(1000);
    }
}

void RF() {
  strcpy(codigo, "mod02:");

  //Converte o valor para envio
  itoa(Porta, Valor_estado, 10);

  strcat(cox  digo, Valor_estado);
  strcpy(Valor_CharMsg, codigo);

  //Envio dos dados
  for(int i=0; i<5;i++){
    vw_send((uint8_t*)Valor_CharMsg, strlen(Valor_CharMsg));

    //Aguarda o envio de dados
    vw_wait_tx();
    Serial.println(Valor_CharMsg);
  }
}

void loop() {
  // Lê da porta 1, envia para porta 0:
  if (Serial1.available()) {
    temp = Serial1.read();
    if(temp!=0 && temp<=100){
      Serial.print(temp, DEC);      
      escreveLCD();
    }
  }
  bool toque = obterToque();
  TeclaPressionada(toque);
  if (digitalRead(pinoSensor) == HIGH){ //SE A LEITURA DO PINO FOR IGUAL A HIGH, FAZ
    
    if(Porta==1){
      Porta=0; 
      RF();   
      Serial.println("Porta Fechou");
    }
  }
  else{ 
    if(Porta==0){
      Porta=1;
      RF();
      Serial.println("Porta Abriu");
    }
  }
}
