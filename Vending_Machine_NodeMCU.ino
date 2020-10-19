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
 *Versão 1.0: (22/08/2020) - (05/09/2020)
 *Objetivo: Trazer a programação feita no arduino uno para 
 *nodeMCU com correção e melhorias da mesma.
 *
 *Versão 2.0: (07/09/2020) - (13/09/2020)
 *Objetijo: Fazer a comunicação com WiFi, integração de teste de conexão ao servidor do banco de dados MySQL; Envio de dados para servidor e/ou correção e melhoria do código base
 *
 *Versão 3.0: (13/09/2020) - (20/09/2020)
 *Objetivo: Correção de erros do código, ortograficos e alinhamento dos textos do display LCD
 *
 *Dificuldades: Transferencia de programação da plataforma arduno uno para nodeMCU; conexão com banco de dados e envio de sinal para o arduino UNO
 */
 
/*
 * Ligação do Leitor RFID - RC522
 * 
 * Leitor RF ---- ESP8266
 *  3,3V -------- 3,3V
 *  GND --------- GND
 *  RST --------- GPIO 0 --- D3
 *  MISO -------- GPIO 12 -- D6
 *  MOSI -------- GPIO 13 -- D7
 *  SCK --------- GPIO 14 -- D5
 *  SDA --------- GPIO 15 -- D8 
 *  
 * Ligação do módulo IC2 do Display LCD
 *  IC2 ---- ESP8266
 *   3,3V -------- 3,3V
 *   GND --------- GND
 *   SDA --------- GPIO 4 -- D2
 *   SCL --------- GPIO 5 -- D1
 *   
 * Ligação dos botões de navegação
 *  Botões --------- ESP8266
 *  Seleção -------- GPIO 10 -- D0
 *  Confirmação ---- GPIO 02 -- 10
 *  
 *  Saída de acionamento
 *  Sinal 1 ------------ D4
 *  Sinal 2 ------------ RX
 */
 
// Inclusão de bibliotecas
#include <ESP8266WiFi.h>
#include <MFRC522.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

// Definição dos pinos RST e SS do MFRC522
#define RST_PIN 0
#define SS_PIN  15

const int advance = D0;                   // Define o pino como avança opção no display
const int selection = 10;                 // Define o pino como seleciona a opção no display
const int Log1 = D4;                      // Define o pino de envio de sinal ao arduino uno 
const int Log2 = 3;                       // Define o pino de envio de sinal ao arduino uno 

int selecao = 0;                          // Variáveis que sera responsavel pelo inclemento dos case dos caracteres que vai ser mostrado ao úsuario
int chave;                                // Variáveis que pelo bloqueio da void pagamento
const char* sabor;                        // Variáveis que guardara o valor do sabor selecionado
char TAG[20];                             // Variáveis que guardara o valor da TAG que vai ser enviada para DB4Free

WiFiClient client;                        // Instancia do cliente WiFi
LiquidCrystal_I2C lcd(0x27,16,2);         // Objeto baseado na biblioteca do LiquidCrystal_I2C
MFRC522 cartao(SS_PIN, RST_PIN);          // Objeto baseado na biblioteca do MFRC522

// Inicio da conexão com o banco de dados baseado no client wifi
MySQL_Connection  BDconsumos(&client);

// Void de conexão com WiFi e de acesso ao banco de dados
void conexao(){

  IPAddress server_addr;
  
  const char* ssid     = "DESKTOP-8C7R7P7 5683";
  const char* password = "M75g66/3";

  WiFi.begin(ssid,password);
  
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("");
    Serial.println("Iniciando conexão");
    Serial.println("");
    lcd.clear();
    lcd.setCursor(3,0);                                               
    lcd.print("Iniciando");
    lcd.setCursor(4,1);                                               
    lcd.print("conexao");
  }
  Serial.println("conexão feita com suscesso");
   lcd.clear();
   lcd.setCursor(0,0);                                               
   lcd.print("conectado");
  
  //A conexão não aceita nomes, então esse método busca o IP no site
  WiFi.hostByName("db4free.net",server_addr);
  
  Serial.println("IP do banco de dados: ");
  Serial.println(server_addr);

  if(BDconsumos.connect(server_addr,3306,"vmachine","vmachine")){  
    Serial.println("Conectado no banco de dados");
    lcd.clear();
    lcd.setCursor(1,0);                                               
    lcd.print("DB conectado");
    delay(1000);
  }
  else{
    Serial.println("Erro na conexão");
    lcd.clear();
    lcd.setCursor(3,0);                                               
    lcd.print("Erro  DB");
    delay(1000);
  }
}

// Void de comando dos botões de seleção e avança
void inicio(){
  
// SE o botão avança for prescionado entra nestá função
  if(digitalRead(advance) == false){
    Serial.println();
    Serial.println("Botão avança acionado");
    delay(500);
    selecao++;           
    screen();             // Chama a Void screen que é responsável pela mudança de informações no display LCD
  }

// SE o botão seleção for prescionado e "selecao" diferente de 0 entra nestá função
  if(digitalRead(selection) == false and selecao != 0){
   Serial.println();
   Serial.println("Botão seleção acionado");
   delay(500);
   chave = 1;            // Atribui valor de chave = 1
   while(chave){
     pagamento();        // Aciona void pagamento
   } 
  }
}

// Só entra nesta função se for presscionado o botão de avança opção
void screen(){
// Caso "selecao" seja maior que 3 retorna "selecao" a 0
  if(selecao > 3){
    selecao = 0;
  }

// O case e resposável por demonstrar as informações de seleção no display e/ou atravez da serial para máxima comunicação com usuario
// No final de cada case e chamando a void inicio, para uma leitura mais rápida do botão  
  switch(selecao){
      case 0:
      delay(10);
      Serial.println("Escolha sabor:");
      lcd.clear();
      lcd.setCursor(0,0);                                               
      lcd.print("Escolha sabor:");
      delay(200);
      inicio();                
      break;

      case 1:
      delay(10);
      Serial.println("Club Social Sabor Tradicional"); 
      lcd.clear();
      lcd.setCursor(0,0);                                               
      lcd.print("Escolha sabor:");
      lcd.setCursor(0,1);                                               
      lcd.print("Tradicional");
      delay(200);
      sabor = "Tradicional";
      inicio();
      break;

      case 2:
      delay(10);
      Serial.println("Club Social Sabor Queijo");
      lcd.clear();
      lcd.setCursor(0,0);                                              
      lcd.print("Escolha sabor:");
      lcd.setCursor(0,1);                                               
      lcd.print("Queijo");
      delay(200);
      sabor = "Queijo";
      inicio();
      break;
      
      case 3:
      delay(10);
      Serial.println("Club Social Sabor Pizza");
      lcd.clear();
      lcd.setCursor(0,0);                                               
      lcd.print("Escolha sabor:");
      lcd.setCursor(0,1);                                               
      lcd.print("Pizza");
      delay(200);
      sabor = "Pizza";
      inicio();
      break;
    }
}

// Só entra nesta função se o botão de seleção for prescionado
void pagamento(){
  Serial.println();
  Serial.println("Produto selecionado, efetue o pagamento:");
  Serial.println("Inseria seu cartão");
  lcd.clear();
  lcd.setCursor(1,0);                                               
  lcd.print("Inseria cartao");
  delay(1000);

  while( ! cartao.PICC_IsNewCardPresent() || ! cartao.PICC_ReadCardSerial()){  // Aguarda cartão e busca novo cartão
    return ;                                                                   // retorno
  }

  String conteudo= "";                                                          // "Cria uma variável do tipo string nome "conteudo"
  byte letra;                                                                   // "Cria uma variável do tipo byte nome "letra"
  for (byte i = 0; i < cartao.uid.size; i++){                                   // Leitura da UID
   conteudo.concat(String(cartao.uid.uidByte[i] < 0x10 ? " 0" : " "));          // Ler valor da TAG
   conteudo.concat(String(cartao.uid.uidByte[i], HEX));                         // Converte para hexadecimal 
  }                                                                                                                                 
                                                                                
  conteudo.toUpperCase();                                                       // Modifica a String no lugar 
  conteudo.toCharArray(TAG,20);                                                 // Coverte a String "conteudo"  para Char[20] "TAG" 
                                                                                //
  Serial.print("UID da TAG lida: ");                                            // Imprime "UID da TAG lida: " no Serial
  Serial.println(conteudo);                                                     // Imprime o valor da variável "conteudo" no Serial 
           
  if (conteudo.substring(1) == "97 11 33 40" || "AB DD 75 22"){                 // Se a variavel "conteudo" for verdade, entramos dentro do IF
    Serial.println(F("Cartao aprovado = Compra aprovada"));                     // Imprime "Cartao aprovado = Compra aprovada" no Serial
    lcd.clear();                                                                // Limpa Display
    lcd.setCursor(4,0);                                                         // Define a posição do cursor: (5,0)
    lcd.print("Compra");                                                        // Imprime uma mensagem no LCD: "Compra"
    lcd.setCursor(3,1);                                                         // Define a posição do cursor: (2,1)
    lcd.print("Aprovada");                                                      // Imprime uma mensagem no LCD: "Aprovada" 
    delay(1000);                                                                // Interrompe por 1s 
    engine();                                                                   // Chama a void "engine"                                           
  }                                                                               
  else {                                                                         // Se a variavel "conteudo" for false, entramos dentro do IF
    Serial.println(F("Cartao invalido = Compra reprovada"));                     // Imprime "Cartao invalido = Compra reprovada" no Serial
    lcd.clear();                                                                 // Limpa Display
    lcd.setCursor(4,0);                                                          // Define a posição do cursor: (0,0)
    lcd.print("Cartao");                                                         // Imprime uma mensagem no LCD: "Cartao" 
    lcd.setCursor(2,1);                                                          // Define a posição do cursor: (0,1)
    lcd.print("Invalido");                                                       // Imprime uma mensagem no LCD: "Invalido"
    delay(1000);                                                                 // Espera 1s
    
    Serial.println("Club Social Sabor Queijo");
    lcd.clear();
    lcd.setCursor(0,0);                                             
    lcd.print("Escolha sabor:");
    selecao = 0;                                                                 // Atribui o valor 0 a selecao
    chave = 0;                                                                   // Atribui o valor 0 a chave
    return inicio();                                                             // Retorna a função inicio                       
  }                    
}

// Está void e responsável pelo envio do sinal em binario para entradas A0 e A1 Arduino uno
void engine(){
  Serial.println();
  Serial.println("Aguade o acionamento dos motores de passo");
  lcd.clear();
  lcd.setCursor(0,0);                                               
  lcd.print("....Aguarde.....");
  delay(1000); // Espera 1s

  // Este if é reponsável de enviar  1 para entrada A1
  if(sabor == "Tradicional"){
    Serial.println("Acionamento do Motor 1 : Club Social Tradicional");
    digitalWrite(Log1,HIGH);
    delay(800); // Espera 800ms
    digitalWrite(Log1,LOW);
  }

  // Este else if é reponsável de enviar  1 para entrada A0
  else if(sabor == "Queijo"){
    Serial.println("Acionamento do Motor 2 : Club Social Queijo"); 
    digitalWrite(Log2,HIGH);
    delay(800); // Espera 800ms
    digitalWrite(Log2,LOW);
  } 

  // Este else if é reponsável de enviar  1 para entrada A0 e A1
  else if(sabor == "Pizza"){
    Serial.println("Acionamento do Motor 4 : Club Social Pizza");
    digitalWrite(Log1,HIGH);
    digitalWrite(Log2,HIGH);
    delay(800); // Espera 800ms
    digitalWrite(Log1,LOW);
    digitalWrite(Log2,LOW);
  }      

  envioDPB(); // Chama a void "envioDPB"

  delay(3000);
  lcd.clear();
  lcd.setCursor(1,0);                                               
  lcd.print("Retire Produto");
  delay(5000); // Espera 3s
  Serial.println("Escolha sabor:");
  lcd.clear();
  lcd.setCursor(0,0);                                             
  lcd.print("Escolha sabor:");
  selecao = 0;
  chave = 0;
}

// Void responsável pelo envio de dados ao DB4free
void envioDPB(){

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&BDconsumos);

  char INSERT_DATA[]   = "INSERT INTO dbmachine.consumo (TAG, Produto) VALUES ('%s','%s')"; 
  
  char query[128];
  
  sprintf(query,INSERT_DATA, TAG, sabor);
  
  cur_mem->execute(query);

  delete cur_mem;
  delay(2000); // Espera 2s
}

void setup() {
  Serial.begin(115200);
  SPI.begin(); 

  lcd.init();
  lcd.backlight();

  conexao();                                                                                         // Chama a void "conexao"
  
  pinMode(advance, INPUT_PULLUP);                                                                    // Define o pino advance como entrada
  pinMode(selection, INPUT_PULLUP);                                                                  // Define o pino selection como entrada
  pinMode(Log1, OUTPUT);                                                                             // Define o pino PWMotor como saída
  pinMode(Log2, OUTPUT); 
                                                                                                     
  delay(1000);                                                                                       // Espera 1s
                                                                                                     
  Serial.println("Iniciando Setup....");                                                             
  Serial.println("Versão do software: JLR 1.0");                                                     
  Serial.println("Autores: ");                                                                       
  Serial.println("\nJeferson Damasceno \nLeandro Barbosano \nLucas Conceição \nRodrigo de Souza");   

  lcd.clear();
  lcd.setCursor(1,0);  
  lcd.print("Inicio Setup");
  delay(500);                                                                                        // Espera 500ms

  cartao.PCD_Init();
  
  Serial.println();
  Serial.println("........Setup iniciado........ ");
  Serial.println();
  lcd.clear();                                                     
  lcd.setCursor(1,0);                                              
  lcd.print("Setup Pronto");
  delay(500);                                            // Espera 500ms
  lcd.clear();
  lcd.setCursor(0,0);                                                                                        
  lcd.print("Escolha sabor:");
  Serial.println("Escolha sabor:");
  delay(500);                                            // Espera 500ms
}

void loop() {
    inicio(); // Chama a void "inicio"
}
