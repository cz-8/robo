/*****************************************************************
SET22

Implementação do controle do minirobô protótipo para auxílio
nas aulas de Linguagem de Programação com foco no evento batalha
de robôs da Engenharia.

Programador: Prof. Carlos Jones
Ano/semestre: 2022/2
Curso: Engenharia de Controle e Automaçao
Última atualização: 08/09/2020

OBS.: uso de interrupçoes para controlar os giros para a direita
e para a esquerda


frente LY 0->127
trás   LY 128->255
esq    LX 0->127
dir    LX 128->255

*****************************************************************/

#include <Servo.h>
#include "PS2X_lib.h"

/******************************************************************
 * seta os pinos conectados ao controle do PS2
 *   - coluna 1: original 
 *   - coluna 2: pinos setados
 * substitua o numero pelo numero do pino que ira utilizar
 ******************************************************************/

 /* pinos para se comunicar com o receptor do controle (podem ser modificados)*/
#define PS2_DAT         8    
#define PS2_CMD         7 
#define PS2_SEL         6 
#define PS2_CLK         5 

/******************************************************************
 * seleçao do modo de controle PS2:
 *   - pressures = leitura analogica de botoes de pressao 
 *   - rumble    = motor rumbling
 * descomente 1 das linhas para cada modo de seleçao
 ******************************************************************/
#define pressures   true // habilita os botoes de direçao com valores de 0->255
//#define pressures   false  // desabilita os botoes de direçao fixando o valor em 0
#define rumble      true // percebi que nesse modo o botao X quando pressionado faz o controle vibrar
//#define rumble      false

Servo spinner; // criando variavel que usara funções da biblioteca Servo

/* definir pinos de entrada para controle do robo */
const int IN1 = 12; // entrada 1 da ponte H
const int IN2 = 11; // entrada 2 da ponte H
const int IN3 = 10; // entrada 3 da ponte H
const int IN4 = 9; // entrada 4 da ponte H in = input

const int rele=4;
int ligar=1;


PS2X ps2x; // cria um objeto da classe PS2
// Neste momento, a biblioteca NÃO suporta a conecçao de controles ​"a quente", o que significa
// você deve sempre reiniciar seu Arduino depois de conectar o controle,
// ou ligue config_gamepad (pinos) novamente depois de conectar o controle.

int error = 0;
byte type = 0;
byte vibrate = 0;
byte vel=30;

//int inverteControle=1;
int resetControle = 0;

void setup()
{
  // seta os pinos da ponte H
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(4, OUTPUT) // pino do rele da arma

  //Define pinos para o rele da arma como saida
  pinMode(rele, OUTPUT); 


 // spinner.attach(3); // define a porta 3 do pwm para o sinal do servomotor
 // spinner.write(0); // define o valor inicial de 0 para o motor brushless
  
  Serial.begin(9600);
  delay(500);  // atraso adicional para fornecer o módulo ps2 sem fio algum tempo para iniciar, antes de configurá-lo
  
 
 //================================================== Testes de conexao do controle ==========================================// 
 
  //Configurar pinos e outras configuraçoes: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.print("Controle encontrado, configurado com sucesso! ");
    
    Serial.print("pressures = ");
	if (pressures)
	  Serial.println("true ");
	else
	  Serial.println("false");
    
    Serial.print("rumble = ");
	if (rumble)
	  Serial.println("true)");
	else
	  Serial.println("false");
    
    Serial.println("Experimentar todos os botoes, X vibrara o controle, mais rapido quanto mais forte vc pressionar;");
    Serial.println("Segurar L1 ou R1 ira imprimir os valores analogicos dos botoes '1'.");
    Serial.println("Nota: Go to www.billporter.info for updates and to report bugs.");
  }  
  else if(error == 1)
    Serial.println("Controle nao encontrado, checar as conecçoes, olhe o arquivo readme.txt para habilitar o debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
    Serial.println("Controle encontrado mas nao aceita comandos. Olhe o arquivo readme.txt para habilitar o debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controle recusando entrar no modo Pressures, modo pode nao ser suportado. ");
  
//  Serial.print(ps2x.Analog(1), HEX);
  
  type = ps2x.readType(); 

   
   
   // -- Registradores de configuração do Timer2 --
  TCCR2A = 0x00;   //Timer operando em modo normal
  TCCR2B = 0x07;   //Prescaler 1:1024
  TCNT2  = 100;    //10 ms overflow again
  TIMSK2 = 0x01;   //Habilita interrupção do Timer2
}


//================================================== Fim testes de conexao do controle ==========================================//


void loop() {
  /* Voce deve ler o Gamepad para obter novos valores e setar os valores de vibraçao
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     Se voce nao habilitar o rumble, use o ps2x.read_gamepad(); sem valores
     Voce deveria chama-la pelo menos uma vez por segundo.
   */  
   
     while(resetControle==0)
     {
       error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
       resetControle++; 
     }
   
    // freio motor na ponte H
    digitalWrite(IN2, HIGH);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
     
  
    if(error == 1) //sai do loop se o controle nao for encontrado
    {
      return;
    }
    
    //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //faz o controle vibrar     //read controller and set large motor to spin at 'vibrate' speed
    
    
    
    
    
    /***************************************************** BOTOES START E SELECT DO CONTROLE *******************************************************/
    // Botao START do Controle 
    if(ps2x.Button(PSB_START))         //sera verdadeiro se o botao estiver pressionado
    {
      Serial.println("Botao Start esta sendo pressionado");
    } 
    
    // Botao SELECT do Controle 
    if(ps2x.Button(PSB_SELECT))
    {
      Serial.println("Botao Select esta sendo pressionado");      //held    
    }
    /***********************************************************************************************************************************************/
    
    
    
    
    /******************************************* BOTOES DE SETAS DIRECIONAIS: UP, DOWN, LEFT, RIGHT ************************************************/
    // Botao UP dos controles direcionais 
    if(ps2x.Button(PSB_PAD_UP))// sera verdadeiro se for pressionado por um bom tempo 
    {      
      Serial.print("Botao Up pressionado, valor: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);   
      
      if(ps2x.Analog(PSAB_PAD_UP)>=150 && ps2x.Analog(PSAB_PAD_UP)<=255)
      {}
    }
            
    // Botao DOWN dos controles direcionais
    if(ps2x.Button(PSB_PAD_DOWN)){
      Serial.print("Botao Down pressionado, valor: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    } 

    
    // Botao RIGHT dos controles direcionais
    if(ps2x.Button(PSB_PAD_RIGHT)){
      Serial.print("Botao Direito pressionado, valor: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }

    
    // Botao LEFT dos controles direcionais
    if(ps2x.Button(PSB_PAD_LEFT)){
      Serial.print("Botao Esquerdo pressionado, valor: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    /***********************************************************************************************************************************************/
 
 
 
  
    /****************************************** BOTOES L e R DO CONTROLE **************************************************************************/
    vibrate = ps2x.Analog(PSAB_CROSS);  //isso ira setar a velocidade de vibraçao do motor maior baseado no quao forte vc pressionar o botao X

    if (ps2x.NewButtonState())         //sera VERDADEIRO se qualquer botao mudar de estado(on to off, or off to on)
    {
      if(ps2x.Button(PSB_L3))
      {
        Serial.println("L3 pressionado"); 
      }
      
      if(ps2x.Button(PSB_L2))
      {
        Serial.println("L2 pressionado");
      }

      if(ps2x.Button(PSB_R2))
      {
        Serial.println("R2 pressionado");
      }
      
      if(ps2x.Button(PSB_TRIANGLE))
      {
        Serial.println("Triangulo pressionado");     
      }      
    }   
    
    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) 
    { //print stick values if either is TRUE
      
      // analogico esquerdo
      Serial.print("Stick Values:");
      Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
      Serial.print(",");
      Serial.println(ps2x.Analog(PSS_LX), DEC);
      // analogico direito
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_RY), DEC); 
      Serial.print(",");
      Serial.println(ps2x.Analog(PSS_RX), DEC);
    }
    /***********************************************************************************************************************************************/
   
   
   
    /************************************************* Botes Geometricos **********************************************************************************************/
   
    if(ps2x.ButtonPressed(PSB_TRIANGLE))
    {
      Serial.println("Triangulo pressionado");       //sera verdadeiro se o botao foi pressionado
    }
     
    // Botao CIRCULO
    if(ps2x.ButtonPressed(PSB_CIRCLE))               //sera verdadeiro se o botao foi pressionado
    {
     digitalWrite(4, LOW); // desliga arma
    }
    // Botao X
    if(ps2x.NewButtonState(PSB_CROSS))               //sera verdadeirO se o botao foi pressionado ou solto
    {
      spinner.write(170);
      Serial.println("X just changed");
    }
    
    // Botao QUADRADO 
    if(ps2x.ButtonReleased(PSB_SQUARE))              //sera verdadiro se o botao foi solto (liberado)
    {
      digitalWrite(4,HIGH);   // liga a arma
    }   

 
      
    
    //================================================= Movimentos do Robo =============================================//
    
      // andar para a frente
      if(ps2x.Analog(PSS_LY)>=0 && ps2x.Analog(PSS_LY)<=25)
      {
        Serial.println("Para frente");
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);        
      }
      // andar para tras
      else  if(ps2x.Analog(PSS_LY)>=252 && ps2x.Analog(PSS_LY)<=255)
      {
     //   Serial.print("LY ");
     //   Serial.println(ps2x.Analog(PSS_LY), DEC);
        Serial.println("Para trás");
        digitalWrite(IN2, HIGH);
        digitalWrite(IN1, LOW);
        digitalWrite(IN4, HIGH);
        digitalWrite(IN3, LOW);  
      }
      // freio motor
      else
      {
        digitalWrite(IN2, HIGH);
        digitalWrite(IN1, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
      }
   
    if(ps2x.Button(PSB_R1))
    {
 
    }
    else if(ps2x.Button(PSB_R2))
    {
        
    }


   // controle do spinner -> aumento de velocidade
 /*   if(ps2x.Button(PSB_L1))
    {
      if(vel<120)//125
      {
        vel+=30; //30
        Serial.println("+25");
      }
      else
      {
         vel=120; 
         Serial.println("125");
      }
      spinner.write(vel); 
      Serial.println(vel);
    }
    
    // controle do spinner -> diminuir a velocidade
    else if(ps2x.Button(PSB_L2))
    {
      if(vel>30)//25
      {
        vel-=30;//25
        Serial.println("-25");
      }
      else
      {
         vel=30; //25
         Serial.println("0");
      }
      spinner.write(vel); // coloca o motor na posição inicial 0 graus
      Serial.println(vel);
    }*/
    delay(100);
}

// --- Rotina de Interrupção ---
ISR(TIMER2_OVF_vect)
{
    TCNT2=100;          // Reinicializa o registrador do Timer2

      // girar para a direita
      if(ps2x.Analog(PSS_LX)>=147 && ps2x.Analog(PSS_LX)<=255)//148
      {
        //Serial.println("Direita");
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN4, HIGH);
        digitalWrite(IN3, LOW);
      }
      // girar para a esquerda
      else if(ps2x.Analog(PSS_LX)>=0 && ps2x.Analog(PSS_LX)<=107)//108
      {
        //Serial.println("Esquerda");
        digitalWrite(IN2, HIGH);
        digitalWrite(IN1, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      }
} 
