#include <stdio.h>
#include <stdint.h>
#include "utils.h"

inline uint8_t getFlag(uint8_t value, uint8_t mask){
	return (value & mask);
}

inline uint8_t setFlag(uint8_t value, uint8_t mask, uint8_t flagValue){
	return (value & ~(mask)) | (flagValue & mask);
}

