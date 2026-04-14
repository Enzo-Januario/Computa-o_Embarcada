const int LED_RED    = 8;
const int LED_YELLOW = 9;
const int LED_GREEN  = 10;
const int BTN_PIN    = 7;
const int POT_PIN    = A0;

bool modoNoturno = false;
int  ciclos      = 0;

void apagaTodos() {
  digitalWrite(LED_RED,    LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN,  LOW);
}

void logSerial(String msg) {
  Serial.print("[");
  Serial.print(millis());
  Serial.print("ms] ");
  Serial.println(msg);
}

bool pressionamentoLongo() {
  if (digitalRead(BTN_PIN) == LOW) return false;

  unsigned long inicio = millis();
  while (digitalRead(BTN_PIN) == HIGH) {
    if (millis() - inicio >= 2000) {
      while (digitalRead(BTN_PIN) == HIGH) delay(10);
      return true;
    }
    delay(10);
  }
  return false;
}

void setup() {
  pinMode(LED_RED,    OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(BTN_PIN,    INPUT);
  Serial.begin(9600);
  logSerial("Sistema iniciado");
}

void loop() {
  // Verifica pressionamento longo (troca de modo)
  if (pressionamentoLongo()) {
    modoNoturno = !modoNoturno;
    apagaTodos();
    if (modoNoturno) {
      logSerial("Modo noturno ATIVADO");
    } else {
      logSerial("Modo semaforo RETOMADO");
    }
    return;
  }

  if (modoNoturno) {
    // Pisca amarelo verificando o botão a cada 50ms
    Serial.println("MODO NOTURNO");
    digitalWrite(LED_YELLOW, HIGH);
    for (int i = 0; i < 10; i++) {  // 10 x 50ms = 500ms
      if (pressionamentoLongo()) {
        modoNoturno = false;
        apagaTodos();
        logSerial("Modo semaforo RETOMADO");
        return;
      }
      delay(50);
    }
    digitalWrite(LED_YELLOW, LOW);
    for (int i = 0; i < 10; i++) {  // 10 x 50ms = 500ms
      if (pressionamentoLongo()) {
        modoNoturno = false;
        apagaTodos();
        logSerial("Modo semaforo RETOMADO");
        return;
      }
      delay(50);
    }
    return;
  }

  // Verifica pausa (pressionamento curto)
  if (digitalRead(BTN_PIN) == HIGH) {
    apagaTodos();
    logSerial("PAUSADO");
    while (digitalRead(BTN_PIN) == HIGH) delay(10);
    return;
  }

  // Lê potenciômetro
  int intervalo = map(analogRead(POT_PIN), 0, 1023, 200, 2000);

  // Ciclo semáforo: vermelho → amarelo → verde
  logSerial("Verde | Intervalo: " + String(intervalo) + "ms");
  apagaTodos();
  digitalWrite(LED_RED, HIGH);
  delay(intervalo);

  logSerial("Amarelo | Intervalo: " + String(intervalo) + "ms");
  apagaTodos();
  digitalWrite(LED_YELLOW, HIGH);
  delay(intervalo);

  logSerial("Vermelho | Intervalo: " + String(intervalo) + "ms");
  apagaTodos();
  digitalWrite(LED_GREEN, HIGH);
  delay(intervalo);

  // Conta ciclo completo
  ciclos++;
  logSerial("Ciclos completos: " + String(ciclos));
  if (ciclos % 5 == 0) {
    logSerial("*** ALERTA: " + String(ciclos) + " ciclos completados! ***");
  }
}
