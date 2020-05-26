/* Autor: Paulo Borges
 * Data: 02 setembro de 2018
 * E-mail: pauloborge10@yandex.com
 */

// Rótulo das entradas/saídas físicas
#define VERDE 2
#define BOBINA 3
#define VERMELHO 4
#define ANALOGPILA 0
#define MONITORIA true

// Delay(milisegundos) para o julgamento de chaveamento.
#define TEMPO_ADICIONAL 1000

// Variáveis
unsigned long TEMPO_MARCADO = 0;
int entradaAnalogica;
float tensaoEntradaArduino = 0;
float tensaoSaidaBateria;
float tensaoValorAproximado;

const float tensaoAlimentacao = 4.6; /* Típica tensão de alimentação, sujeita a mudança dependente
do nível utilizado na shield. */
const float R1 = 7.5;
const float R2 = 30.0;
int aceito = 0, negado = 0;
bool resultado;

// Para definição dos limites, edite aqui c/ apenas uma casa decimal após a vírgula.
float limiteAlto = 14.0;
float limiteBaixo = 12.7;
float limiteMuitoBaixo = 1.0;

void setup() {
  // Iniciamos 0 monitor serie
#if MONITORIA
  Serial.begin(9600);
#endif
  // Los pinos de LED en modo salida
  pinMode(VERDE, OUTPUT);
  pinMode(BOBINA, OUTPUT);
  pinMode(VERMELHO, OUTPUT);
}

void loop() {
  // Leitura do valor de entrada analógica
  entradaAnalogica = analogRead(ANALOGPILA);
  /*Cálculo para a obtensão de valor que provêm do divesor de tensão(0~4,71Volts).
    Resultado com 2 casa decimais  */
  tensaoEntradaArduino = ((entradaAnalogica * tensaoAlimentacao) / 1024);
  /*Cálculo matemático envolvendo o valor do divisor de tensão p/ encontrar o valor da bateria.
    Resultado com 2 casa decimais  */
  tensaoSaidaBateria = tensaoEntradaArduino * ((R1 + R2) / R1);
  /*Cálculo matemático p/ equivalência do valor medido no arduino com valor registrado no display.
    Resultado com 1 casa decimal  */
  tensaoValorAproximado = tensaoSaidaBateria * 10;
  tensaoValorAproximado = round(tensaoValorAproximado);
  tensaoValorAproximado /= 10;

  /*Mostrando o resultado na saída serial p/ acompanhamento em tempo real.*/
#if false
  Serial.print("VOLT: ");
  Serial.println(tensaoValorAproximado);
#endif

  if ((millis() > (TEMPO_MARCADO + TEMPO_ADICIONAL)) || (millis() < TEMPO_MARCADO)) {

    /* Julgando a condição de medição por relação de medidas acima ou abaixo do limite analógico estabelecido.*/
    if (aceito > negado)
      resultado = true;
    else
      resultado = false;

    /*Mostrando na porta serial o resultado p/ acompanhamento em tempo real, sob as seguintes condições:
       Positivo - Quer dizer que a leitura analógica esteve em niveis "aceitáveis" por mais tempo;
       Negativo - Quer dizer que a leitura analógica esteve em niveis "inaceitáveis" por mais tempo.
    */
#if MONITORIA
    Serial.print("Resultado: ");
    Serial.print(((resultado == true) ? "Positivo " : "Negativo "));
    Serial.print(aceito);
    Serial.print("/");
    Serial.print(negado);
    Serial.print("\t");
    Serial.print(tensaoSaidaBateria);
    Serial.print("/");
    Serial.print(tensaoValorAproximado);
    Serial.println("Volt(s)");
#endif

    /*Limpeza dos marcadores e contadores p/ uma nova leitura durante o tempo determinado.*/
    TEMPO_MARCADO = millis();
    aceito = 0 ; negado = 0;
  }
  else {
    if (tensaoValorAproximado >= limiteBaixo && tensaoValorAproximado < limiteAlto) {
      aceito++;
    }
    else {
      negado++;
    }
  }

  /*Chaveamento das saídas físicas de acordo com o valor booleano presente na variável "resultado".*/
  if (resultado) {
    digitalWrite(VERDE, HIGH);
    digitalWrite(BOBINA, LOW);
    digitalWrite(VERMELHO, LOW);
  }
  else {
    digitalWrite(VERDE, LOW);
    digitalWrite(BOBINA, HIGH);
    digitalWrite(VERMELHO, HIGH);
  }

}
