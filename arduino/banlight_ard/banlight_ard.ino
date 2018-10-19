/*
 * BanLight - Banshee 2018
 * 
 * Controlador sobre Arduino Pro Micro (Leonardo)
 * para control de BanLigth
 * 
 */


#include <Adafruit_NeoPixel.h>

#define BANLIGHT_VERSION              0.1

// Led Ring Config
#define DEFAULT_LED_ARRAY_PIN         14
#define DEFAULT_LED_ARRAY_SIZE        16
#define LED1_DEFAULT_BRIGHTNESS       100

// Serial Config
#define SERIAL_SPEED                  9600


// ASCII commands
#define ASCII_CMD_VERSION             "VERSION"
#define ASCII_CMD_ON                  "ON"
#define ASCII_CMD_OFF                 "OFF"
#define ASCII_CMD_BRIGTH              "BRIGHT="



// Variables Initialization
Adafruit_NeoPixel leds(DEFAULT_LED_ARRAY_SIZE, DEFAULT_LED_ARRAY_PIN);
 
 
// Array para la lectura de comandos ASCII
char    cmd[16];
uint8_t cmdlen = 0;
uint8_t echo   = 1;
 
void setup(){
  int i;
  
  // Generic set pin mode. Pins 0 y 1 reserved to USB-Serial port.
  for(i= 2; i<=16; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH); // Set pull up resistor
  }

  // Solo por estar en la protoboard un pin del BT. Quitar de version final
  pinMode(10, OUTPUT);
  digitalWrite(13, LOW);

  // Serial Initialization  
  Serial1.begin(SERIAL_SPEED);       // Inicializamos el puerto serie BT (Para Modo AT 2)
  Serial.begin(SERIAL_SPEED);        // Inicializamos  el puerto serie  

  
  //  Inicializacion de leds
  leds.setPin(DEFAULT_LED_ARRAY_PIN);
  leds.setNumPixels(DEFAULT_LED_ARRAY_SIZE);
  leds.setBrightness(LED1_DEFAULT_BRIGHTNESS);
  for(int i = 0;i < DEFAULT_LED_ARRAY_SIZE; i++){
    leds.setPixelColor(i, '\x00', '\x00', '\xff');
  }
  leds.show();
}
 
void loop() {  
  if(readUSBASCIICommand()) {
    executeASCIICommand();
  }  
 
  if(readBTASCIICommand()) {
    executeASCIICommand();
  }   
}


boolean readUSBASCIICommand() {
  char c;
  while(Serial.available() > 0) {
    c = Serial.read();
    if(echo) {
      Serial.write(c);
      if(c == '\r') Serial.write('\n');
    }
    
    if(c != '\r' && c != '\n') {
      cmd[cmdlen] = c;
      cmdlen++;
    }
    
    if(c == '\n') return true;
  }
  
  return false;
}

boolean readBTASCIICommand() {
  char c;
  while(Serial1.available() > 0) {
    c = Serial1.read();
    if(echo) {
      Serial1.write(c);
      if(c == '\r') Serial1.write('\n');
    }
    
    if(c != '\r' && c != '\n') {
      cmd[cmdlen] = c;
      cmdlen++;
    }
    
    if(c == '\n') return true;
  }
  
  return false;
}


void executeASCIICommand() {
  uint8_t error;
  cmd[cmdlen] = '\0';
  
  if(echo){
    Serial.write("Execute command : \"");
    Serial.write(cmd);
    Serial.write("\"\n");
    
    Serial1.write("Execute command : \"");
    Serial1.write(cmd);
    Serial1.write("\"\n");
  }
  
  
  if(strcmp(cmd,ASCII_CMD_VERSION) == 0) {
    error = cmdVersion();
  //} else if(strncmp(cmd,"SET ", 4) == 0) {
  //  error = cmdSet(cmd+4);
  } else if(strcmp(cmd,ASCII_CMD_ON) == 0) {
    error = cmdTurnOn();
  } else if(strcmp(cmd,ASCII_CMD_OFF) == 0) {
    error = cmdTurnOff();
  } else if(strncmp(cmd,ASCII_CMD_BRIGTH, String(ASCII_CMD_BRIGTH).length()) == 0) {
     error = cmdSetBright(atoi(cmd+String(ASCII_CMD_BRIGTH).length()));
  //} else if(strcmp(cmd,"HELP") == 0) {
  //  error = cmdHelp();
  } else {
    error = 1;
  }
  
  
  if(echo) {
    if(error == 0) {
      Serial.println("OK");
      Serial1.println("OK");
    } else {
      Serial.println("ERROR");
      Serial1.println("ERROR");
    }  
  }
  cmdlen = 0;
  
  Serial.flush();
  Serial1.flush();
}


uint8_t cmdTurnOn() {
  for(int i = 0;i < DEFAULT_LED_ARRAY_SIZE; i++){
    leds.setPixelColor(i, '\xff', '\xff', '\xff');
  }
  leds.show();
  return 0;
}

uint8_t cmdTurnOff() {
  for(int i = 0;i < DEFAULT_LED_ARRAY_SIZE; i++){
    leds.setPixelColor(i, '\x00', '\x00', '\x00');
  }
  leds.show();
  return 0;
}

uint8_t cmdSetBright(uint8_t val) {
  leds.setBrightness(val);
  leds.show();
  return 0;
}



uint8_t cmdVersion() {
  Serial1.print("\nBanLight - Version : ");
  Serial1.println(BANLIGHT_VERSION);
  return 0;
}

