// =============================================
// Etapa 2: Controle por Botão e Velocidade
// =============================================

// Definição dos pinos
const int LED_RED    = 10;
const int LED_YELLOW = 9;
const int LED_GREEN  = 8;
const int BOTAO_PIN  = 7;   // Botão no pino D7
const int POT_PIN    = A0;

// Estado de pausa
bool pausado = false;

// Debounce
unsigned long ultimoDebounce = 0;
const unsigned long DEBOUNCE_MS = 300;

// =============================================
// Configuração
// =============================================
void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BOTAO_PIN, INPUT); // Pull-down externo de 10kΩ

  Serial.begin(9600);
  Serial.println("=== Semaforo com Controle Iniciado ===");
}

// =============================================
// Função: Apaga todos os LEDs
// =============================================
void apagarTodos() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
}

// =============================================
// Função: Lê o potenciômetro e converte para ms
// =============================================
int lerIntervalo() {
  int valorPot = analogRead(POT_PIN);
  return map(valorPot, 0, 1023, 200, 2000);
}

// =============================================
// Função: Verifica o botão com debounce
// =============================================
void verificarBotao() {
  if (digitalRead(BOTAO_PIN) == HIGH) {
    unsigned long agora = millis();
    if (agora - ultimoDebounce > DEBOUNCE_MS) {
      ultimoDebounce = agora;
      pausado = !pausado;

      if (pausado) {
        apagarTodos();
        Serial.println(">>> PAUSADO <<<");
      } else {
        Serial.println(">>> RETOMADO <<<");
      }

      // Espera soltar o botão
      while (digitalRead(BOTAO_PIN) == HIGH) {
        delay(10);
      }
    }
  }
}

// =============================================
// Função: Aguarda o intervalo verificando o botão
// =============================================
void aguardarComVerificacao(int intervalo) {
  unsigned long inicio = millis();

  while (millis() - inicio < (unsigned long)intervalo) {
    verificarBotao();

    // Se pausou, trava até despausar
    while (pausado) {
      verificarBotao();
      delay(50);
    }

    delay(10);
  }
}

// =============================================
// Função: Acende LED e imprime info na serial
// =============================================
void acenderSemaforo(int pino, const char* cor) {
  apagarTodos();
  digitalWrite(pino, HIGH);

  int intervalo = lerIntervalo();

  Serial.print("Cor ativa: ");
  Serial.print(cor);
  Serial.print(" | Intervalo: ");
  Serial.print(intervalo);
  Serial.print(" ms");
  Serial.print(" | Pausado: ");
  Serial.println(pausado ? "SIM" : "NAO");

  aguardarComVerificacao(intervalo);
}

// =============================================
// Loop principal
// =============================================
void loop() {
  verificarBotao();

  while (pausado) {
    verificarBotao();
    delay(50);
  }

  acenderSemaforo(LED_GREEN,  "VERDE");
  acenderSemaforo(LED_YELLOW, "AMARELO");
  acenderSemaforo(LED_RED,    "VERMELHO");
}
