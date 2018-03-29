#ifndef MAIN_H
# define MAIN_H


//Adresser i EEPROM dit olika data skrivs
#define MAX_SPEED				   0x20
#define GOAL_ANGLE				   0x1e
#define TORQUE					   0x22
#define RETURN_LEVEL			   0x10 //När servot ska skicka tillbaka data
#define LOWER_ANGLE_LIMIT    	   0x06 //Nedre vinkelgräns
#define HIGHER_ANGLE_LIMIT         0x08 //Övre vinkelgräns

//Startvinklar då roboten står upp
#define STAND_DEG_00				150
#define STAND_DEG_03				150
#define STAND_DEG_06				150
#define STAND_DEG_09				150
#define STAND_DEG_12				150
#define STAND_DEG_15				150

#define STAND_DEG_01				 90
#define STAND_DEG_04				 90
#define STAND_DEG_07				 90
#define STAND_DEG_10				210
#define STAND_DEG_13				210
#define STAND_DEG_16				210

#define STAND_DEG_02				230
#define STAND_DEG_05				230
#define STAND_DEG_08				230
#define STAND_DEG_11				 70
#define STAND_DEG_14				 70
#define STAND_DEG_17				 70


#endif