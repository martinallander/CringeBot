/*
//Panasonic Grid-EYE Communication for Atmel Software Framework
//Jonathan Richardson
//
//Provides communication to the Panasonic Grid-EYE using the
//Atmel Software Framework. 
//
//Requires the use of ASF Two-wire Master Interface Driver.
*/

#include "twim.h"
#include "sysclk.h"
#include "delay.h"

#ifndef GRIDEYE_H_
#define GRIDEYE_H_

//static status_code_t init_test(void);


#define BUFFERC				5
#define GE_CALIBRATION_TIMES		BUFFERC
#define filterWidth				3
#define filterHeight				3
#define imageWidth				8
#define imageHeight				8

typedef struct {
	int16_t data[64];
} GridEyeImage;

typedef struct {
	uint8_t address;
	Twim *twim_loc;
	uint8_t read_buffer[300];
	twi_package_t packet;
	GridEyeImage image[BUFFERC];
	GridEyeImage difference;
	int imagepointer;
	int initialframes;
	int16_t temperature;
} GridEye;

uint8_t		ge_init				(GridEye *this,	Twim *twim,	uint8_t grideye_address);

/*Secondary Functions*/
bool		ge_readData			(GridEye *ge);
bool		ge_readAverage			(GridEye *ge);

/*Direct Access*/
bool		ge_writePacket			(GridEye *ge, uint8_t addr, uint8_t datalength, void* data);
bool		ge_readPacket			(GridEye *ge, uint8_t addr, uint8_t datalength, void* data);
void		ge_setupPacket			(GridEye *ge, uint8_t addr, uint8_t datalength, void* data);
bool		ge_setFPS			(GridEye *ge, bool fps_set_10);
bool		ge_readAverage			(GridEye *ge);
bool		ge_initialReset			(GridEye *ge);
bool		ge_setInterrupt			(GridEye *ge, bool absolute, uint16_t upper, uint16_t lower, uint16_t hysteresis);
bool		ge_readInterruptFlag			(GridEye *ge, bool clearFlag);
bool		ge_clearInterruptFlag			(GridEye *ge);
uint8_t		ge_readFPS			(GridEye *ge);
bool		ge_readTemp			(GridEye *ge);


/*GridEye Image Manipulation*/
void		geimage_copy			(GridEyeImage *from, GridEyeImage *to);
void		geimage_sum			(GridEyeImage *from, GridEyeImage *to);
void		geimage_subtract			(GridEyeImage *source, GridEyeImage *minus);
void		geimage_abs			(GridEyeImage *image);

#endif /* GRIDEYE_H_ */