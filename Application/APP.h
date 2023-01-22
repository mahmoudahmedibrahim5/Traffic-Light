#ifndef APP_H_
#define APP_H_

#include "../ECUAL/LED/LED.h"
#include "../ECUAL/Button/BUTTON.h"

void initialize(void);
void NormalMode(void);
void PedestrianMode(void);

void CarsPeriod(void);
void FromCarsToPedestrian(void);
void PestrianPeriod(void);
void FromPedestrianToCars(void);

#endif /* APP_H_ */