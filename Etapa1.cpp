// =============================================
// Etapa 1: Semáforo Sequencial
// =============================================

// Definição dos pinos
const int LED_RED    = 10;
const int LED_YELLOW = 9;
const int LED_GREEN  = 8;

// Tempo de cada fase (1 segundo)
const unsigned long INTERVALO = 1000;

// =============================================
// Configuração: Declara os pinos como saída
// =============================================
void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Inicializa comunicação serial
  Serial.begin(9600);
  Serial.println("=== Semaforo Sequencial Iniciado ===");
}

// =============================================
// Modularidade: Função para encapsular o processo
// =============================================
void acenderSemaforo(int pino, const char* cor) {
  // Apaga todos os LEDs
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);

  // Acende apenas o LED da vez
  digitalWrite(pino, HIGH);

  // Comunicação: Imprime a cor ativa no Serial Monitor
  Serial.print("Cor ativa: ");
  Serial.println(cor);

  // Aguarda 1 segundo
  delay(INTERVALO);
}

// =============================================
// Loop principal: Sequência do semáforo
// =============================================
void loop() {
  acenderSemaforo(LED_GREEN,  "VERDE");
  acenderSemaforo(LED_YELLOW, "AMARELO");
  acenderSemaforo(LED_RED,    "VERMELHO");
}
