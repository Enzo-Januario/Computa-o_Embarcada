// =============================================
// Etapa 3: Modo Noturno e Contagem de Ciclos
// =============================================

// Definição dos pinos
const int LED_RED    = 10;
const int LED_YELLOW = 9;
const int LED_GREEN  = 8;
const int BOTAO_PIN  = 7;
const int POT_PIN    = A0;

// Estados do sistema
enum Estado {
  MODO_SEMAFORO,
  MODO_NOTURNO
};

Estado estadoAtual = MODO_SEMAFORO;

// Contagem de ciclos
int contadorCiclos = 0;

// Debounce e detecção de pressionamento longo
unsigned long botaoPressionadoEm = 0;
bool botaoAnterior = false;
const unsigned long TEMPO_LONGO = 2000; // 2 segundos

// =============================================
// Configuração
// =============================================
void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BOTAO_PIN, INPUT);

  Serial.begin(9600);
  Serial.println("=== Semaforo Etapa 3 Iniciado ===");
  imprimirTimestamp("Sistema iniciado no MODO SEMAFORO");
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
// Função: Imprime mensagem com timestamp
// =============================================
void imprimirTimestamp(const char* mensagem) {
  unsigned long segundos = millis() / 1000;
  unsigned long minutos = segundos / 60;
  segundos = segundos % 60;

  Serial.print("[");
  if (minutos < 10) Serial.print("0");
  Serial.print(minutos);
  Serial.print(":");
  if (segundos < 10) Serial.print("0");
  Serial.print(segundos);
  Serial.print("] ");
  Serial.println(mensagem);
}

// =============================================
// Função: Lê o potenciômetro e converte para ms
// =============================================
int lerIntervalo() {
  int valorPot = analogRead(POT_PIN);
  return map(valorPot, 0, 1023, 200, 2000);
}

// =============================================
// Função: Verifica pressionamento longo do botão
// Retorna true se houve pressionamento >= 2s
// =============================================
bool verificarBotaoLongo() {
  bool botaoAtual = digitalRead(BOTAO_PIN) == HIGH;

  // Botão acabou de ser pressionado
  if (botaoAtual && !botaoAnterior) {
    botaoPressionadoEm = millis();
  }

  // Botão está sendo segurado — verifica se passou 2 segundos
  if (botaoAtual && botaoAnterior) {
    if (millis() - botaoPressionadoEm >= TEMPO_LONGO) {
      // Espera soltar o botão
      while (digitalRead(BOTAO_PIN) == HIGH) {
        delay(10);
      }
      botaoAnterior = false;
      return true; // Pressionamento longo detectado!
    }
  }

  // Botão foi solto (pressionamento curto — ignora)
  if (!botaoAtual && botaoAnterior) {
    // Nada acontece, era curto
  }

  botaoAnterior = botaoAtual;
  return false;
}

// =============================================
// Função: Aguarda intervalo verificando o botão
// Retorna true se houve mudança de modo
// =============================================
bool aguardarComVerificacao(int intervalo) {
  unsigned long inicio = millis();

  while (millis() - inicio < (unsigned long)intervalo) {
    if (verificarBotaoLongo()) {
      return true; // Mudou de modo!
    }
    delay(10);
  }
  return false; // Tempo esgotou normalmente
}

// =============================================
// Função: Executa um ciclo do semáforo
// Retorna true se houve mudança de modo
// =============================================
bool cicloSemaforo() {
  // ---------- VERDE ----------
  apagarTodos();
  digitalWrite(LED_GREEN, HIGH);
  int intervalo = lerIntervalo();

  Serial.print("  VERDE   | Intervalo: ");
  Serial.print(intervalo);
  Serial.println(" ms");

  if (aguardarComVerificacao(intervalo)) return true;

  // ---------- AMARELO ----------
  apagarTodos();
  digitalWrite(LED_YELLOW, HIGH);
  intervalo = lerIntervalo();

  Serial.print("  AMARELO | Intervalo: ");
  Serial.print(intervalo);
  Serial.println(" ms");

  if (aguardarComVerificacao(intervalo)) return true;

  // ---------- VERMELHO ----------
  apagarTodos();
  digitalWrite(LED_RED, HIGH);
  intervalo = lerIntervalo();

  Serial.print("  VERMELHO| Intervalo: ");
  Serial.print(intervalo);
  Serial.println(" ms");

  if (aguardarComVerificacao(intervalo)) return true;

  // Ciclo completo!
  contadorCiclos++;

  String msg = "Ciclo #" + String(contadorCiclos) + " completo";
  imprimirTimestamp(msg.c_str());

  // Alerta a cada múltiplo de 5
  if (contadorCiclos % 5 == 0) {
    Serial.println("============================================");
    Serial.print(">>> ALERTA: ");
    Serial.print(contadorCiclos);
    Serial.println(" ciclos completados! <<<");
    Serial.println("============================================");
  }

  return false;
}

// =============================================
// Função: Executa o modo noturno
// Retorna quando botão longo é detectado
// =============================================
void modoNoturno() {
  imprimirTimestamp("Entrou no MODO NOTURNO");

  while (true) {
    // Amarelo ON
    apagarTodos();
    digitalWrite(LED_YELLOW, HIGH);
    Serial.println("  MODO NOTURNO - Amarelo ON");

    if (aguardarComVerificacao(500)) return;

    // Amarelo OFF
    apagarTodos();
    Serial.println("  MODO NOTURNO - Amarelo OFF");

    if (aguardarComVerificacao(500)) return;
  }
}

// =============================================
// Loop principal
// =============================================
void loop() {
  if (estadoAtual == MODO_SEMAFORO) {
    bool mudou = cicloSemaforo();

    if (mudou) {
      estadoAtual = MODO_NOTURNO;
      apagarTodos();
      modoNoturno();
      // Ao sair do noturno, volta para semáforo
      estadoAtual = MODO_SEMAFORO;
      apagarTodos();
      imprimirTimestamp("Retornou ao MODO SEMAFORO");
    }
  }
}
