/*
//Panasonic Grid-EYE Communication for Atmel Software Framework
//Jonathan Richardson
//
//Provides communication to the Panasonic Grid-EYE using the
//Atmel Software Framework. 
//
//Requires the use of ASF Two-wire Master Interface Driver.
*/
#include "grideye.h"

uint8_t ge_init (GridEye *ge, Twim *twim, uint8_t address_number)
{
	//set the I2C address of the Grid Eye instance
	ge->address = address_number == 0 ? 0b1101000 : 0b1101001;
	
	//TWIM port to use
	ge->twim_loc = twim;
	
	//Set default structure variables
	ge->imagepointer = 0;
	ge->temperature = 0;

	for(int i = 0; i < BUFFERC; i++)
		for(int j = 0; j < 64; j++)
			ge->image[i].data[j] = 0;
			
	for(int j = 0; j < 64; j++)
	{
		ge->difference.data[j] = 0;
	}

	//Setup the TWIM module with the proper settings for the Grid Eye
	uint32_t cpu_speed = sysclk_get_peripheral_bus_hz(ge->twim_loc);
	struct twim_config opts = {
		.twim_clk = cpu_speed,
		.speed = TWI_FAST_MODE_SPEED,
		.hsmode_speed = 0,
		.data_setup_cycles = 0,
		.hsmode_data_setup_cycles = 0,
		.smbus = false,
		.clock_slew_limit = 0,
		.clock_drive_strength_low = 0,
		.data_slew_limit = 0,
		.data_drive_strength_low = 0,
		.hs_clock_slew_limit = 0,
		.hs_clock_drive_strength_high = 0,
		.hs_clock_drive_strength_low = 0,
		.hs_data_slew_limit = 0,
		.hs_data_drive_strength_low = 0
	};

	//setup callback
	twim_set_callback(ge->twim_loc, 0, twim_default_callback, 77);

	//finally, write config options
	twim_set_config(ge->twim_loc, &opts);
	
	return ge_initialReset(ge);
}

bool ge_readData (GridEye *ge)
{
	//Read full 128 bytes of data from Grid Eye
	if(!ge_readPacket(ge, 0x80, 128, &ge->read_buffer))
		return false;

	//update current image buffer pointer
	ge->imagepointer = (ge->imagepointer + 1) % BUFFERC;

	//Convert read buffer to actual image
	for(int i = 0; i < 64; i++)
		ge->image[ge->imagepointer].data[i] = (ge->read_buffer[2*i] | ((ge->read_buffer[2*i+1] & 0x7) << 8));

	return true;
}

bool ge_readAverage(GridEye *ge)
{
	//reset the difference array
	for(int i = 0; i < 64; i++)
		ge->difference.data[i] = 0;

	//fill the read buffer while storing the sum of the buffer in difference
	for(int i = 0; i < BUFFERC; i++)
	{
		if(!ge_readData(ge))
			return false;
		delay_ms(105);
		geimage_sum(ge->image[ge->imagepointer].data, ge->difference.data);
	}

	//finish taking take the average of the difference array
	for(int i = 0; i < 64; i++)
		ge->difference.data[i] /= BUFFERC;

	return true;
}

void ge_setupPacket(GridEye *ge, uint8_t addr, uint8_t datalength, void* data)
{
	//create the structure of a new packet
	ge->packet.chip = ge->address;
	ge->packet.addr[0] = addr;
	ge->packet.addr_length = 1;
	ge->packet.high_speed = 0;
	ge->packet.high_speed_code = 0;
	ge->packet.buffer = data;
	ge->packet.length = datalength;
	ge->packet.ten_bit = false;
}

bool ge_writePacket(GridEye *ge, uint8_t addr, uint8_t datalength, void* data)
{
	//setup write packet and send
	ge_setupPacket(ge, addr, datalength, data);
	return twi_master_write(ge->twim_loc, &ge->packet) == STATUS_OK;
}

bool ge_readPacket(GridEye *ge, uint8_t addr, uint8_t datalength, void* data)
{
	//setup request packet and send
	ge_setupPacket(ge, addr, datalength, data);
	return twi_master_read(ge->twim_loc, &ge->packet) == STATUS_OK;
}

bool ge_setFPS (GridEye *ge, bool fps_set_10)
{
	//setup packet and write to Grid Eye
	uint8_t databuffer = !fps_set_10;
	return ge_writePacket(ge, 0x02, 1, &databuffer);
}

uint8_t ge_readFPS (GridEye *ge)
{
	//Read Grid Eye setting
	uint8_t databuffer = 0;
	if(!ge_readPacket(ge, 0x02, 1, &databuffer))
		return 0xFF;
	return (databuffer&1==0?10:1);
}

bool ge_readTemp (GridEye *ge)
{
	int16_t rawdata;

	//get packet from Grid Eye
	if(!ge_readPacket(ge, 0x0E, 2, &rawdata))
		return false;

	//handle negative numbers, the sign bit is stored in bit 11
	//when read directly from the Grid Eye
	if((rawdata >> 11) & 1)
		rawdata |= 1 << 15;

	//mask out all the bits that are not needed
	rawdata &= 0x87FF;

	ge->temperature = rawdata * 0.25;
	
	return true;
}

bool ge_initialReset (GridEye *ge)
{
	uint8_t packet = 0x3F;
	return ge_writePacket(ge, 0x01, 1, &packet);
}

bool ge_setInterrupt (GridEye *ge, bool absolute, uint16_t upper, uint16_t lower, uint16_t hysteresis)
{
	//mask variables to 12-bit and create the full packet to send
	uint16_t fullpacket[3] = {upper & 0xFFF, lower & 0xFFF, hysteresis & 0xFFF};
	uint8_t setuppacket = 0x01 | (absolute?0x02:0x00);

	//send packet to Grid Eye
	return ge_writePacket(ge, 0x08, 6, &fullpacket) && ge_writePacket(ge, 0x03, 1, &setuppacket);
}

bool ge_readInterruptFlag (GridEye *ge, bool clearFlag)
{
	uint8_t databuffer;
	bool status = false;

	//read interrupt flag from register
	if(!ge_readPacket(ge, 0x04, 1, &databuffer))
		return false;

	//buffer to boolean
	status = (databuffer >> 1) & 1;
	
	//if chosen, clear the interrupt flag
	if(clearFlag && status)
		ge_clearInterruptFlag(ge);

	return status;
}

bool ge_clearInterruptFlag (GridEye *ge)
{
	//create and send clear packet to flag register
	uint8_t clearPacket = 0x02;
	return ge_writePacket(ge, 0x05, 1, &clearPacket);
}

void geimage_copy(GridEyeImage *from, GridEyeImage *to)
{
	for(int i = 0; i < 64; i++)
		to->data[i] = from->data[i];
}

void geimage_sum(GridEyeImage *from, GridEyeImage *to)
{
	for(int i = 0; i < 64; i++)
		to->data[i] += from->data[i];
}

void geimage_subtract(GridEyeImage *source, GridEyeImage *minus)
{
	for(int i = 0; i < 64; i++)
		source->data[i] -= minus->data[i];
}

void geimage_abs(GridEyeImage *image)
{
	for(int i = 0; i < 64; i++)
		image->data[i] = abs(image->data[i]);
}