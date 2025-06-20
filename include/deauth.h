#ifndef DEAUTH_H
#define DEAUTH_H

#include <Arduino.h>

void start_deauth(int wifi_number, int attack_type, uint16_t reason);
void stop_deauth();
void deauth_all();  // Declare new function

extern int eliminated_stations;
extern int deauth_type;

#endif
