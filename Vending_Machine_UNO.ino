/* -------------------------------------------------------------------
 * Faculdade de Tecnologia SENAI Mariano Ferraz
 * Curso Superior de Tecnologia em Automação Industrial
 * São Paulo, 23 de stembro de 2020
 * Groupo: 20-TN2-5
 * Projeto: Vending Machine
 * 
 * Disciplina: Tecnologia da Informação 2
 * Prof. Caio Vinícius
 * 
 * Autores:
 *          Jeferson Damasceno de Moura
 *          Leandro Barbozano dos Santos
 *          Lucas Gonçalves da Conceição
 *          Rodrigo de Souza
 *          
 *Versão 1.0: (19/09/2020) - (19/09/2020)
 *Objetivo: Acionar os motores de passo que, serão responsável pela retirada do produto

 */
 
/*
 * Arduino ------------------------------ NodeMCU
 *   A0   -   pino de entrada de sinal  -  D4
 *   A1   -   pino de entrada de sinal  -  RX
 *   
 * Arduino Indicação de sinal para acionamento de motores de passo
 *  A2  - Indica que o primeiro motor foi acionado
 *  A3  - Indica que o segundo motor foi acionado
 *  A4  - Indica que o terceiro motor foi acionado
 */

#include <Stepper.h>                                                    // Inclusão da biblioteca "Stepper.h"
                                                                        
const int I1 = A0;                                                      // Define o pino que reebera o sinal do nodeMCU
const int I2 = A1;                                                      // Define o pino que reebera o sinal do nodeMCU 
const int Ind1 = A2;                                                    // Define o pino que acendera o LED Indicativo
const int Ind2 = A3;                                                    // Define o pino que acendera o LED Indicativo
const int Ind3 = A4;                                                    // Define o pino que acendera o LED Indicativo 
const int stepsPerRevolution = 200;                                     // Número de etapas por rotação do motor
                                                                        //
#define in1 13                                                          // Configuração entrada 1 do ULN2003 - UNIDADE1
#define in2 12                                                          // Configuração entrada 2 do ULN2003 - UNIDADE1
#define in3 11                                                          // Configuração entrada 3 do ULN2003 - UNIDADE1
#define in4 10                                                          // Configuração entrada 4 do ULN2003 - UNIDADE1
                                                                        //
#define in5 9                                                           // Configuração entrada 1 do ULN2003 - UNIDADE2
#define in6 8                                                           // Configuração entrada 2 do ULN2003 - UNIDADE2
#define in7 7                                                           // Configuração entrada 3 do ULN2003 - UNIDADE2
#define in8 6                                                           // Configuração entrada 4 do ULN2003 - UNIDADE2
                                                                        //
#define in9 5                                                           // Configuração entrada 1 do ULN2003 - UNIDADE3
#define in10 4                                                          // Configuração entrada 2 do ULN2003 - UNIDADE3
#define in11 3                                                          // Configuração entrada 3 do ULN2003 - UNIDADE3
#define in12 2                                                          // Configuração entrada 4 do ULN2003 - UNIDADE3
                                                                        // 
Stepper Engine1 (stepsPerRevolution, in1,in2,in3,in4);                  // Criação do objeto Engine1
Stepper Engine2 (stepsPerRevolution, in5,in6,in7,in8);                  // Criação do objeto Engine2
Stepper Engine3 (stepsPerRevolution, in9,in10,in11,in12);               // Criação do objeto Engine3
                                                                        //
void setup() {                                                          //
  Serial.begin(9600);                                                   // Inicialização da comunicação serial
                                                                        // 
  Engine1.setSpeed(120);                                                // Determina a velocidade do " PRIMEIRO " motor de passo
  Engine2.setSpeed(120);                                                // Determina a velocidade do " SEGUNDO " motor de passo
  Engine3.setSpeed(120);                                                // Determina a velocidade do " TERCEIRO " motor de passo
  pinMode(I1, INPUT);                                                   // Define o pino I1 como entrada
  pinMode(I2, INPUT);                                                   // Define o pino I2 como entrada
  pinMode(Ind1, OUTPUT);                                                // Define o pino Ind1 como saída
  pinMode(Ind2, OUTPUT);                                                // Define o pino Ind2 como saída
  pinMode(Ind3, OUTPUT);                                                // Define o pino Ind3 como saída
}                                 

void loop() { 
  
  // Comando de acionamento de motores de passo, que iram retirar os seus respectivos produtos das molas
   
  if(digitalRead(I1) == false and digitalRead(I2) == true){             // Se a entrada I1 for falso e entrada I2 for verdadeira executa esse if
    Serial.println("Slot 1: Acionado");                                 // Imprime "Slot 1: Acionado" no Serial                                                    
    Engine1.step(2040);                                                 // Quantidade de passo execultado pelo motor

    // Acende e desliga o LED indicativo
    digitalWrite(Ind1, HIGH); 
    delay(1000);                                                        // Responsável pelo tempo de funcionamento do motor de passo e acendimento do LED indicativo
    digitalWrite(Ind1,LOW);
  }
  
  else if(digitalRead(I1) == true and digitalRead(I2) == false){        // Se a entrada I1 for verdadeira e entrada I2 for falso executa esse if
    Serial.println("Slot 2: Acionado");                                 // Imprime "Slot 1: Acionado" no Serial                                                    
    Engine2.step(2040);                                                 // Quantidade de passo execultado pelo motor
    
    // Acende e desliga o LED indicativo
    digitalWrite(Ind2, HIGH); 
    delay(1000);                                                        // Responsável pelo tempo de funcionamento do motor de passo e acendimento do LED indicativo
    digitalWrite(Ind2,LOW);
  }
  
  else if(digitalRead(I1) == true and digitalRead(I2) == true){         // Se a entrada I1 for verdadeira e entrada I2 for verdadeira executa esse if
    Serial.println("Slot 3: Acionado");                                 // Imprime "Slot 1: Acionado" no Serial                                                    
    Engine3.step(2040);                                                 // Quantidade de passo execultado pelo motor
    
    // Acende e desliga o LED indicativo
    digitalWrite(Ind3, HIGH); 
    delay(1000);                                                        // Responsável pelo tempo de funcionamento do motor de passo e acendimento do LED indicativo
    digitalWrite(Ind3,LOW);
  }
}
