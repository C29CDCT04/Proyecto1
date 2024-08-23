#include <Arduino.h>
#include <driver/ledc.h>

const int lm35Pin = 34;  // Pin ADC donde está conectado el LM35
const int buttonPin = 23; // Pin donde está conectado el botón
int buttonState = 0;
float temperaturaC = 0.0;

//Pines para los leds
const int ledr = 5; //rojo
const int ledv = 18; //verde
const int leda = 19; //azul

// Función para establecer el color de los LEDs
void setColor(int redValue, int greenValue, int blueValue) {
  ledcWrite(0, redValue);   // Canal 0 controla el LED rojo
  ledcWrite(1, greenValue); // Canal 1 controla el LED verde
  ledcWrite(2, blueValue);  // Canal 2 controla el LED azul
}


void setup() {
  Serial.begin(115200);
  pinMode(lm35Pin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Activa resistencia interna de pull-up

  pinMode(ledr, OUTPUT);
  pinMode(ledv, OUTPUT);
  pinMode(leda, OUTPUT);

  // Configurar PWM
  ledcSetup(0, 5000, 8);
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);

  // Asignar canales PWM
  ledcAttachPin(ledr, 0);
  ledcAttachPin(ledv, 1);
  ledcAttachPin(leda, 2);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {  // Si el botón está presionado
    int analogValue = analogRead(lm35Pin);
    
    // Convertir la lectura analógica a temperatura en grados Celsius
    float voltaje = analogValue * (3.3 / 4095.0); // Conversión a voltaje (3.3V referencia, ADC de 12 bits)
    float temperaturaC = (voltaje * 100.0);        // LM35 entrega 10mV por grado Celsius
    
    Serial.print("Temperatura: ");
    Serial.print(temperaturaC);
    Serial.print(" ºC \n");

    // Control de LEDs basado en la temperatura
    if (temperaturaC < 37.0) {
      setColor(0, 255, 0);  // Verde
    } else if (temperaturaC >= 37.0 && temperaturaC <= 37.5) {
      setColor(255, 3, 0);  // Amarillo (Rojo + Verde)
    } else if (temperaturaC > 37.5) {
      setColor(255, 0, 0);  // Rojo
    }

    delay(500);  // Retardo para evitar lecturas repetitivas si se mantiene presionado el botón
  }
}

