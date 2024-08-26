/*Universidad Del Valle de Guatemala
Facultad de Ingeniería
Departamento de Electrónica, Mecatrónica y Biomédica
Electrónica Digital 2

Carlos Daniel Camacho Tista - 22690

Proyecto No. 1 - Sensor de temperatura
Parte 1: Sensor de Temperatura. Se diseñó e implementó una rutina en la cual, 
mediante un botón, adquirió la señal de un sensor de temperatura LM35 utilizando un ADC.

Parte 2: Semáforo de Temperatura. Se diseñó e implementó tres señales PWM para controlar 
tres LEDs de color rojo, verde y azul, donde, dependiendo de la temperatura, colocó un color indicador.

Parte 3: Reloj del semáforo de temperatura. Se diseñó e implementó una rutina que, dependiendo 
del valor de la temperatura obtenida, movió la posición del eje de un servomotor, modelando un 
reloj de temperatura que iba de la mano con el semáforo de temperatura.

Parte 4: Despliegue de temperatura. Se diseñó e implementó una rutina para desplegar el valor de la 
temperatura en tres displays de 7 segmentos. Utilizó el punto del primer display para mostrar un 
decimal y empleó multiplexeo para poder desplegar la información en los tres displays con ayuda de transistores. 
Se Tomó en cuenta que el circuito podía cambiar dependiendo de si los displays eran de ánodo común o cátodo común.

Parte 5: Dashboard Adafruit IO. Se diseñó una interfaz en los servidores de Adafruit IO, donde mostró 
los resultados de los valores obtenidos del sensor de temperatura.

Parte 6: ESP32 WiFi. Se implementó la comunicación entre el ESP32 y los servidores de Adafruit IO utilizando 
WiFi. El ESP32 fue capaz de enviar los valores obtenidos del sensor de temperatura y del reloj de temperatura.*/

#include <Arduino.h>
#include "display7.h"
#include "config.h"

const int lm35Pin = 34;  // Pin ADC donde está conectado el LM35
const int buttonPin = 23; // Pin donde está conectado el botón
int buttonState = HIGH;
int lastButtonState = HIGH;
float temperaturaC = 0.0;

// set up the 'counter' feed
AdafruitIO_Feed *tempC = io.feed("temperatura");

// Pines para los LEDs individuales
const int ledr = 2;   // LED rojo
const int ledv = 5;   // LED verde
const int leda = 4;   // LED amarillo

// Configuración del Servo
#define servoPin 15
#define canalPWM 9
#define freqPWM 50 // Frecuencia Hz 
#define resPWM 10  // Resolución bits

// Pines para los displays de 7 segmentos
#define DIS1 21
#define DIS2 19
#define DIS3 18

// Pines para los segmentos del display
#define segA 25
#define segB 32
#define segC 27
#define segD 12
#define segE 13
#define segF 33
#define segG 26
#define segP 14

void initPWM(void);
bool reachedEnd = false;  // Bandera para verificar si se llegó al final

void multiplexarDisplay(void);
int digito1, digito2, digito3;  // Variables globales para los dígitos a mostrar

unsigned long previousMillis = 0;  // Temporizador para Adafruit IO
const long interval = 3000;        // Intervalo de envío de datos (3 segundos)

void setup() {
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  pinMode(lm35Pin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Activa resistencia interna de pull-up

  // Configurar pines de salida para los LEDs
  pinMode(ledr, OUTPUT);
  pinMode(ledv, OUTPUT);
  pinMode(leda, OUTPUT);
  
  digitalWrite(ledr, LOW);
  digitalWrite(ledv, LOW);
  digitalWrite(leda, LOW); 

  initPWM();

  // Configurar los pines de los displays
  configurarDisplay(segA, segB, segC, segD, segE, segF, segG, segP);
  pinMode(DIS1, OUTPUT);
  pinMode(DIS2, OUTPUT);
  pinMode(DIS3, OUTPUT);

  // Inicializar los pines de los displays
  digitalWrite(DIS1, LOW);
  digitalWrite(DIS2, LOW);
  digitalWrite(DIS3, LOW);
}

void loop() {
  io.run();

  unsigned long currentMillis = millis();
  // Multiplexar displays continuamente
  multiplexarDisplay();
  delay(5);  // Pequeño retardo para suavizar el multiplexado
 
  buttonState = digitalRead(buttonPin);

  // Detectar cambio en el estado del botón
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Si el botón fue presionado (cambio de HIGH a LOW)
    int analogValue = analogRead(lm35Pin);
    
    // Convertir la lectura analógica a temperatura en grados Celsius
    float voltaje = analogValue * (3.3 / 4095.0); // Conversión a voltaje (3.3V referencia, ADC de 12 bits)
    temperaturaC = (voltaje * 100.0) + 25;               // LM35 entrega 10mV por grado Celsius
    
    Serial.print("Temperatura: ");
    Serial.print(temperaturaC);
    Serial.println(" ºC");

    // Actualizar los dígitos a mostrar en los displays
    int tempEntera = (int)(temperaturaC * 10);  // Multiplicamos por 10 para manejar un decimal
    digito1 = tempEntera / 100;                 // Primer dígito (centenas)
    digito2 = (tempEntera / 10) % 10;           // Segundo dígito (decenas)
    digito3 = tempEntera % 10;                  // Tercer dígito (unidades)

    if (temperaturaC < 37.0) {
      digitalWrite(ledr, LOW);
      digitalWrite(ledv, HIGH);
      digitalWrite(leda, LOW);

      int dutyCycle = 100;
      if (!reachedEnd) {  // Si no hemos llegado al final del movimiento
        ledcWrite(canalPWM, dutyCycle);  // Ajustar el ciclo de trabajo actual
        dutyCycle++;  // Incrementar el ciclo de trabajo para mover el servo gradualmente
        delay(100);  // Retardo para hacer que el movimiento sea suave

        // Verificar si hemos llegado al final del movimiento
        if (dutyCycle >= 125) {
          reachedEnd = true;  // Se ha alcanzado el final, detener el incremento
        }
      }

    } else if (temperaturaC >= 37.0 && temperaturaC <= 37.5) {
      digitalWrite(ledr, LOW);
      digitalWrite(ledv, LOW);
      digitalWrite(leda, HIGH);

      int dutyCycle = 75;
      if (!reachedEnd) {  // Si no hemos llegado al final del movimiento
        ledcWrite(canalPWM, dutyCycle);  // Ajustar el ciclo de trabajo actual
        dutyCycle++;  // Incrementar el ciclo de trabajo para mover el servo gradualmente
        delay(100);  // Retardo para hacer que el movimiento sea suave

        // Verificar si hemos llegado al final del movimiento
        if (dutyCycle >= 85) { 
          reachedEnd = true;  // Se ha alcanzado el final, detener el incremento
        }
      }
      
    } else if (temperaturaC > 37.5) {
      digitalWrite(ledr, HIGH);
      digitalWrite(ledv, LOW);
      digitalWrite(leda, LOW);

      int dutyCycle = 50;
      if (!reachedEnd) {  // Si no hemos llegado al final del movimiento
        ledcWrite(canalPWM, dutyCycle);  // Ajustar el ciclo de trabajo actual
        dutyCycle++;  // Incrementar el ciclo de trabajo para mover el servo gradualmente
        delay(100);  // Retardo para hacer que el movimiento sea suave

        // Verificar si hemos llegado al final del movimiento
        if (dutyCycle >= 70) {
          reachedEnd = true;  // Se ha alcanzado el final, detener el incremento
        }
      }
    }

    // Actualizar Adafruit IO solo si ha pasado el intervalo
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      // save count to the 'counter' feed on Adafruit IO
      Serial.print("sending -> ");
      Serial.println(temperaturaC);
      tempC->save(temperaturaC);
    }

  }

  lastButtonState = buttonState;

}

// Función para el servo
void initPWM(void) {
  ledcSetup(canalPWM, freqPWM, resPWM);
  ledcAttachPin(servoPin, canalPWM);
  ledcWrite(canalPWM, 0);
}

// Función para multiplexar los displays y mostrar los dígitos actuales
void multiplexarDisplay() {
  
  // Dígito 1
  digitalWrite(DIS1, HIGH);
  desplegarDisplay(digito1);
  desplegarPunto(0);  // No activar el punto decimal en el primer display
  delay(10);  // Retardo para mantener el multiplexado
  digitalWrite(DIS1, LOW);  // Apagar el display después de mostrar el dígito

  // Dígito 2
  digitalWrite(DIS2, HIGH);
  desplegarDisplay(digito2);
  desplegarPunto(1);  // Activar el punto decimal en el segundo display
  delay(10);  // Retardo para mantener el multiplexado
  digitalWrite(DIS2, LOW);  // Apagar el display después de mostrar el dígito

  // Dígito 3
  digitalWrite(DIS3, HIGH);
  desplegarDisplay(digito3);
  desplegarPunto(0);  // No mostrar punto decimal en el tercer display
  delay(10);  // Retardo para mantener el multiplexado
  digitalWrite(DIS3, LOW);  // Apagar el display después de mostrar el dígito
}
