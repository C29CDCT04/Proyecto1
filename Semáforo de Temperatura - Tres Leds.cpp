#include <Arduino.h>

const int lm35Pin = 34;  // Pin ADC donde está conectado el LM35
const int buttonPin = 23; // Pin donde está conectado el botón
int buttonState = 0;
float temperaturaC = 0.0;

// Pines para los LEDs individuales
const int ledr = 5;   // LED rojo
const int ledv = 18;  // LED verde
const int leda = 19;  // LED azul (que representará amarillo junto con el rojo)

void setup() {
  Serial.begin(115200);
  pinMode(lm35Pin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Activa resistencia interna de pull-up

  // Configurar pines de salida para los LEDs
  pinMode(ledr, OUTPUT);
  pinMode(ledv, OUTPUT);
  pinMode(leda, OUTPUT);
  
  digitalWrite(ledr, LOW);
  digitalWrite(ledv, LOW);
  digitalWrite(leda, LOW); 
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {  // Si el botón está presionado
    int analogValue = analogRead(lm35Pin);
    
    // Convertir la lectura analógica a temperatura en grados Celsius
    float voltaje = analogValue * (3.3 / 4095.0); // Conversión a voltaje (3.3V referencia, ADC de 12 bits)
    temperaturaC = (voltaje * 100.0)+18;               // LM35 entrega 10mV por grado Celsius
    
    Serial.print("Temperatura: ");
    Serial.print(temperaturaC);
    Serial.println(" ºC");

    if (temperaturaC < 37.0) {
        digitalWrite(ledr, LOW);
        digitalWrite(ledv, HIGH);
        digitalWrite(leda, LOW);
    } else if (temperaturaC >= 37.0 && temperaturaC <= 37.5) {
        digitalWrite(ledr, LOW);
        digitalWrite(ledv, LOW);
        digitalWrite(leda, HIGH);
    } else if (temperaturaC > 37.5) {
        digitalWrite(ledr, HIGH);
        digitalWrite(ledv, LOW);
        digitalWrite(leda, LOW);
    }

    delay(500);  // Retardo para evitar lecturas repetitivas si se mantiene presionado el botón
  }
}
