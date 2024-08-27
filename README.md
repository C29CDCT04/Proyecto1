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
WiFi. El ESP32 fue capaz de enviar los valores obtenidos del sensor de temperatura y del reloj de temperatura.
