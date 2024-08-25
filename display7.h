#ifndef __DISPLAY7_H__
#define __DISPLAY7_H__

#include <Arduino.h>

extern uint8_t pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinP;

//Función para la configuración de Display de 7 segmentos 
void configurarDisplay(uint8_t segA, uint8_t segB, uint8_t segC, uint8_t segD, uint8_t segE, uint8_t segF, uint8_t segG, uint8_t segP);

//Función para desplegar el número en el display
void desplegarDisplay(uint8_t numero);

//Función para desplegar el punto decimal
void desplegarPunto(uint8_t punto);

#endif // __DISPLAY7_H__
