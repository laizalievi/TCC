/*==========================================================================================================================================
Projeto: Desenvolvimento de uma Interface MODBUS RTU entre CLP e Microcontrolador 
         para Automação de Sistemas de Ensaio de Aerogeradores
Autor 1: Jean Evangelista Mendes
Autor 2: Laiz Magali Alievi
Orientador: CARLOS EDUARDO VIANA
Modelo micro controlador Arduino UNO: ATMEGA328-PU
IDE: Arduino IDE 2.3.3
ID do escravo = 1
baud = 9600
============================================================================================================================================*/


//==========================================================================================================================================
// DECLARAÇÃO DAS BIBLIOTECAS                                                                        
//==========================================================================================================================================
#include <Arduino.h>                    // Biblioteca Nativa 
#include <ArduinoRS485.h>               // Biblioteca para comunicação RS-485
#include <ArduinoModbus.h>              // Biblioteca para Modbus
//==========================================================================================================================================

//=========================================================================================================================================
// DECLARAÇÃO DAS VARIÁVEIS GLOBAIS                                                                     
//=========================================================================================================================================
const float pi = 3.14159265;
int periodo = 5000;                 
float RaioAnemometro = 7.5;          // em cm
float vel_mont = 0;
float vel_jus = 0;
unsigned int contador1 = 0;
unsigned int contador2 = 0;
unsigned int RPM_jusante = 0;
unsigned int RPM_montante = 0;
//==========================================================================================================================================

//=========================================================================================================================================
// FUNÇÃO SETUP -> void setup                                                                       
//=========================================================================================================================================
void setup() {
  // Inicializa comunicação serial
  Serial.begin(9600);
  while (!Serial);

  // Configuração do Modbus
  if (!ModbusRTUServer.begin(1, 9600)) { 
    Serial.println("Falha ao iniciar Modbus RTU Server!");
    while (1);
  }
  ModbusRTUServer.configureHoldingRegisters(0x00, 10); // Configura 10 registradores de 16 bits
  Serial.println("Servidor Modbus RTU iniciado com sucesso.");

  // Configuração dos pinos e interrupções
  pinMode(3, INPUT);  // Pino para montante
  pinMode(4, INPUT);  // Pino para jusante
  attachInterrupt(digitalPinToInterrupt(3), interrupcao1, RISING); // Interrupção no pino 3
  attachInterrupt(digitalPinToInterrupt(4), interrupcao2, RISING); // Interrupção no pino 4
}
//==========================================================================================================================================

//=======================================================================================================================================
// FUNÇÃO-> void anemometro                                                              
//=======================================================================================================================================
void anemometro() {
  if ((millis() - periodo) >= 5000) { // Avalia o intervalo de 5 segundos
    RPM_jusante = (contador1 * 60) / 5;                   // Calcula rotações por minuto
    vel_jus = ((2 * pi * RaioAnemometro * RPM_jusante)) / 100; // Velocidade em m/s

    RPM_montante = (contador2 * 60) / 5;
    vel_mont = ((2 * pi * RaioAnemometro * RPM_montante)) / 100;

    // Reset dos contadores e do tempo
    contador1 = 0;
    contador2 = 0;
    periodo = millis();
  }
}
//==========================================================================================================================================

//======================================================================================================================================
// FUNÇÃO CONTADOR VELOCIDADE JUSANTE -> void interrupcao1                                                                  
//======================================================================================================================================
void interrupcao1() {
  contador1++;
}
//==========================================================================================================================================

//======================================================================================================================================
// FUNÇÃO CONTADOR VELOCIDADE JUSANTE -> void interrupcao2                                                                 
//======================================================================================================================================
void interrupcao2() {
  contador2++;
}
//==========================================================================================================================================

//======================================================================================================================================
// FUNÇÃO -> void loop                                                                  
//======================================================================================================================================
void loop() {
  ModbusRTUServer.poll(); // Atualiza o servidor Modbus
  anemometro(); 

  // Atualizar os registradores com valores
  // Multiplica vel_jus por 100 para manter precisão de 2 casas decimais
  uint16_t vel_jus_escalado = (uint16_t)(vel_jus * 100);
  ModbusRTUServer.holdingRegisterWrite(0, vel_jus_escalado); //3 pino

  // Atualizar os registradores com valores
  // Multiplica vel_mont por 100 para manter precisão de 2 casas decimais
  uint16_t vel_mont_escalado = (uint16_t)(vel_mont * 100);
  ModbusRTUServer.holdingRegisterWrite(2, vel_mont_escalado);

  // Enviar valores de debug pela Serial
  Serial.println("Velocidade Jusante: " + String(vel_jus, 2) + " m/s");
  //Serial.println("Velocidade Jusante Escalada (Modbus): " + String(vel_jus_escalado));

    // Enviar valores de debug pela Serial
  Serial.println("Velocidade Montante: " + String(vel_mont, 2) + " m/s");
  //Serial.println("Velocidade Montante Escalada (Modbus): " + String(vel_mont_escalado));

  delay(150);
}
//==========================================================================================================================================






