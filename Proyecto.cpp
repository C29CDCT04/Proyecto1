/* Universidad Del Valle de Guatemala
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

// Importación de bibliotecas necesarias
#include <Arduino.h>
#include "display7.h"  // Biblioteca para manejar el display de 7 segmentos
#include "config.h"    // Configuración de Adafruit IO

// Definición de pines y variables globales
const int lm35Pin = 34;  // Pin ADC donde está conectado el LM35
const int buttonPin = 23; // Pin donde está conectado el botón
int buttonState = HIGH;   // Estado del botón
int lastButtonState = HIGH;  // Último estado del botón
float temperaturaC = 0.0; // Variable para almacenar la temperatura

// Configuración de feeds para Adafruit IO
AdafruitIO_Feed *tempC = io.feed("temperatura");  // Feed para la temperatura
AdafruitIO_Feed *servoC = io.feed("servo");       // Feed para la posición del servo

// Pines para los LEDs individuales
const int ledr = 2;   // LED rojo
const int ledv = 5;   // LED verde
const int leda = 4;   // LED amarillo

// Configuración del Servo
#define servoPin 15
#define canalPWM 9
#define freqPWM 50  // Frecuencia del PWM (50 Hz)
#define resPWM 10   // Resolución del PWM (10 bits)

// Pines para los displays de 7 segmentos
#define DIS1 21
#define DIS2 19
#define DIS3 18

// Pines para los segmentos del display de 7 segmentos
#define segA 25
#define segB 32
#define segC 27
#define segD 12
#define segE 13
#define segF 33
#define segG 26
#define segP 14

// Función para inicializar el PWM del servo
void initPWM(void);
bool reachedEnd = false;  // Bandera para indicar si el servo ha alcanzado el final de su movimiento

// Función para multiplexar el display de 7 segmentos
void multiplexarDisplay(void);
int digito1, digito2, digito3;  // Variables globales para los dígitos que se mostrarán en el display

unsigned long previousMillis = 0;  // Temporizador para Adafruit IO
const long interval = 3000;           // Intervalo de envío de datos (5 ms)

void setup() {
  Serial.begin(115200);  // Inicialización de la comunicación serial a 115200 baudios

  // Esperar a que se abra el monitor serial
  while(!Serial);

  Serial.print("Connecting to Adafruit IO");

  // Conectar a Adafruit IO
  io.connect();

  // Esperar la conexión a Adafruit IO
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Conexión establecida
  Serial.println();
  Serial.println(io.statusText());

  // Configurar los pines de entrada y salida
  pinMode(lm35Pin, INPUT);  // Pin del sensor LM35 como entrada
  pinMode(buttonPin, INPUT_PULLUP);  // Pin del botón con resistencia interna pull-up

  // Configurar los pines de salida para los LEDs
  pinMode(ledr, OUTPUT);
  pinMode(ledv, OUTPUT);
  pinMode(leda, OUTPUT);
  
  // Inicializar los LEDs apagados
  digitalWrite(ledr, LOW);
  digitalWrite(ledv, LOW);
  digitalWrite(leda, LOW); 

  // Inicializar el PWM del servo
  initPWM();

  // Configurar los pines de los displays de 7 segmentos
  configurarDisplay(segA, segB, segC, segD, segE, segF, segG, segP);
  pinMode(DIS1, OUTPUT);
  pinMode(DIS2, OUTPUT);
  pinMode(DIS3, OUTPUT);

  // Inicializar los pines de los displays apagados
  digitalWrite(DIS1, LOW);
  digitalWrite(DIS2, LOW);
  digitalWrite(DIS3, LOW);
}

void loop() {

  unsigned long currentMillis = millis();  // Obtener el tiempo actual en milisegundos
  // Multiplexar los displays continuamente para mantener la actualización de los dígitos
  multiplexarDisplay();
  delay(5);  // Pequeño retardo para suavizar el multiplexado
 
  buttonState = digitalRead(buttonPin);  // Leer el estado actual del botón

  // Detectar si hubo un cambio en el estado del botón (presionado)
  if (buttonState == LOW && lastButtonState == HIGH) {
    io.run();  // Ejecutar la conexión a Adafruit IO

    // Leer el valor analógico del sensor LM35
    int analogValue = analogRead(lm35Pin);
    
    // Convertir el valor analógico a temperatura en grados Celsius
    float voltaje = analogValue * (3.3 / 4095.0);  // Convertir el valor ADC a voltaje
    temperaturaC = (voltaje * 100.0) + 23.3;  // Calcular la temperatura en grados Celsius
    
    Serial.print("Temperatura: ");
    Serial.print(temperaturaC);
    Serial.println(" ºC");

    // Convertir la temperatura a dígitos para mostrar en el display
    int tempEntera = (int)(temperaturaC * 10);  // Multiplicar por 10 para manejar el decimal
    digito1 = tempEntera / 100;                 // Primer dígito (centenas)
    digito2 = (tempEntera / 10) % 10;           // Segundo dígito (decenas)
    digito3 = tempEntera % 10;                  // Tercer dígito (unidades)

    // Control de LEDs y movimiento del servo según la temperatura
    if (temperaturaC < 37.0) {
      digitalWrite(ledr, LOW);
      digitalWrite(ledv, HIGH);
      digitalWrite(leda, LOW);

      int dutyCycle = 100;  // Ciclo de trabajo inicial para el servo
      if (!reachedEnd) {  // Si el servo no ha alcanzado el final de su movimiento
        ledcWrite(canalPWM, dutyCycle);  // Mover el servo
        dutyCycle++;  // Incrementar el ciclo de trabajo para mover el servo gradualmente
        delay(10);  // Pequeño retardo para suavizar el movimiento del servo

        // Si se ha alcanzado el final del movimiento del servo
        if (dutyCycle >= 125) {
          reachedEnd = true;  // Marcar que se ha alcanzado el final del movimiento
        }
      Serial.print("sending -> ");
      Serial.println(45);
      servoC->save(45);  // Guardar la posición del servo en Adafruit IO
      }

    } else if (temperaturaC >= 37.0 && temperaturaC <= 37.5) {
      digitalWrite(ledr, LOW);
      digitalWrite(ledv, LOW);
      digitalWrite(leda, HIGH);

      int dutyCycle = 75;  // Ciclo de trabajo inicial para el servo
      if (!reachedEnd) {  // Si el servo no ha alcanzado el final de su movimiento
        ledcWrite(canalPWM, dutyCycle);  // Mover el servo
        dutyCycle++;  // Incrementar el ciclo de trabajo para mover el servo gradualmente
        delay(10);  // Pequeño retardo para suavizar el movimiento del servo

        // Si se ha alcanzado el final del movimiento del servo
        if (dutyCycle >= 85) { 
          reachedEnd = true;  // Marcar que se ha alcanzado el final del movimiento
        }

      Serial.print("sending -> ");
      Serial.println(90);
      servoC->save(90);  // Guardar la posición del servo en Adafruit IO
      }
      
    } else if (temperaturaC > 37.5) {
      digitalWrite(ledr, HIGH);
      digitalWrite(ledv, LOW);
      digitalWrite(leda, LOW);

      int dutyCycle = 50;  // Ciclo de trabajo inicial para el servo
      if (!reachedEnd) {  // Si el servo no ha alcanzado el final de su movimiento
        ledcWrite(canalPWM, dutyCycle);  // Mover el servo
        dutyCycle++;  // Incrementar el ciclo de trabajo para mover el servo gradualmente
        delay(10);  // Pequeño retardo para suavizar el movimiento del servo

        // Si se ha alcanzado el final del movimiento del servo
        if (dutyCycle >= 70) {
          reachedEnd = true;  // Marcar que se ha alcanzado el final del movimiento
        }

      Serial.print("sending -> ");
      Serial.println(135);
      servoC->save(135);  // Guardar la posición del servo en Adafruit IO
      }
    }

    // Actualizar Adafruit IO solo si ha pasado el intervalo
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      // Enviar la temperatura a Adafruit IO
      Serial.print("sending -> ");
      Serial.println(temperaturaC);
      tempC->save(temperaturaC);
    }

  }

  lastButtonState = buttonState;  // Actualizar el estado del botón

}

// Función para inicializar el PWM del servo
void initPWM(void) {
  ledcSetup(canalPWM, freqPWM, resPWM);  // Configurar el canal PWM, la frecuencia y la resolución
  ledcAttachPin(servoPin, canalPWM);  // Vincular el pin del servo al canal PWM
  ledcWrite(canalPWM, 0);  // Inicializar el ciclo de trabajo del PWM en 0
}

// Función para multiplexar los displays y mostrar los dígitos actuales
void multiplexarDisplay() {
  
  // Mostrar el primer dígito
  digitalWrite(DIS1, HIGH);  // Encender el display 1
  desplegarDisplay(digito1);  // Mostrar el primer dígito
  desplegarPunto(0);  // No activar el punto decimal en el primer display
  delay(5);  // Pequeño retardo para mantener el multiplexado
  digitalWrite(DIS1, LOW);  // Apagar el display 1 después de mostrar el dígito

  // Mostrar el segundo dígito
  digitalWrite(DIS2, HIGH);  // Encender el display 2
  desplegarDisplay(digito2);  // Mostrar el segundo dígito
  desplegarPunto(1);  // Activar el punto decimal en el segundo display
  delay(5);  // Pequeño retardo para mantener el multiplexado
  digitalWrite(DIS2, LOW);  // Apagar el display 2 después de mostrar el dígito

  // Mostrar el tercer dígito
  digitalWrite(DIS3, HIGH);  // Encender el display 3
  desplegarDisplay(digito3);  // Mostrar el tercer dígito
  desplegarPunto(0);  // No mostrar el punto decimal en el tercer display
  delay(5);  // Pequeño retardo para mantener el multiplexado
  digitalWrite(DIS3, LOW);  // Apagar el display 3 después de mostrar el dígito
}
