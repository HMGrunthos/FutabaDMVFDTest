#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "MN12832L.h"

#define NSHIFTREGBITS 240

static const uint_fast8_t NGATEGROUPS = 43;
static const uint_fast8_t GATESBITOFFSET = 192;

static uint_fast8_t gateGroup = 0;
static uint_fast8_t pIdx = 0;
static uint_fast8_t shiftItr = 0;

static uint_least8_t driverShiftOut[NSHIFTREGBITS/8];

bool UpdateMN12832L(bool enableDisplay)
{
	uint_fast8_t gateA, gateB;

	gateA = gateGroup;
	gateB = gateA + 1;
	//printf("Gate group :%i: gateA:%i: gateB:%i:\n", gateGroup, gateA + 1, gateB + 1);

	uint_fast8_t gateAbyte, gateAbit;
	uint_fast8_t gateBbyte, gateBbit;

	gateAbyte = (gateA >> 3) + GATESBITOFFSET/8;
	gateAbit = gateA & 0x7;
	gateBbyte = (gateB >> 3) + GATESBITOFFSET/8;
	gateBbit = gateB & 0x7;
	//printf("gateAbyte:%i: gateAbit:%i: gateBbyte:%i: gateBbit:%i:\n", gateAbyte, gateAbit, gateBbyte, gateBbit);
	//printf("gateAR:%i: gateBR:%i:\n", gateAbyte*8 + gateAbit + 1, gateBbyte*8 + gateBbit + 1);

	memset(driverShiftOut, 0, sizeof(driverShiftOut));
	driverShiftOut[gateAbyte] = 1 << gateAbit;
	driverShiftOut[gateBbyte] |= 1 << gateBbit;

	enum AnodeNames {
		AnodeA,
		AnodeB,
		AnodeC,
		AnodeD,
		AnodeE,
		AnodeF,
		NANODES
	};
	int_fast8_t anodes[NANODES];
	memset(anodes, 0xFF, sizeof(anodes));

	// #define FUNC(a) (31 * ((a) % 37) / 36)
	// #define FUNC(a) (15.5 + 15.5*sinf((a)*2*M_PI/50) + 0.5)
	#define FUNC(a) (15.5 + 15.5*sinf((a)*2*M_PI/(32 + (shiftItr%60))) + 0.5)

	if(gateGroup == (NGATEGROUPS-1)) { // Just anodes (a, b)
		anodes[AnodeA] = FUNC(pIdx);
		pIdx += 1;
		anodes[AnodeB] = FUNC(pIdx);
		pIdx += 1;
	} else if((gateGroup & 0x1) == 0) { // Just anodes (a, b, c)
		anodes[AnodeA] = FUNC(pIdx);
		pIdx += 1;
		anodes[AnodeB] = FUNC(pIdx);
		pIdx += 1;
		anodes[AnodeC] = FUNC(pIdx);
		pIdx += 1;
	} else { // Just anodes (d, e, f)
		anodes[AnodeD] = FUNC(pIdx);
		pIdx += 1;
		anodes[AnodeE] = FUNC(pIdx);
		pIdx += 1;
		anodes[AnodeF] = FUNC(pIdx);
		pIdx += 1;
	}

	for(uint_fast8_t anodeNum = 0; anodeNum < NANODES; anodeNum++) {
		if(anodes[anodeNum] < 0) {
			continue;
		}

		uint_fast8_t anodeBitPos;
		if(anodeNum < 3) {
			anodeBitPos = anodeNum * 2; // a(0) = 0(1), b(1) = 2(3), c(2) = 4(5)
		} else {
			anodeBitPos = (5 - anodeNum)*2 + 1; // d(3) = 5(6), e(4) = 3(4), f(5) = 1(2)
		}

		uint_fast8_t rowGroup = anodes[anodeNum] / 4;
		uint_fast8_t groupIdx = anodes[anodeNum] % 4;
		// uint_fast8_t bitOffset = (anodeBitPos + 6*groupIdx) + ((rowGroup & 0x1)*8));
		uint_fast8_t bitOffset = anodeBitPos + 6*groupIdx;
		//printf("anodeBitPos:%hu: anodes[anodeNum]:%hd: rowGroup:%hu: groupIdx:%hu: bitOffset:%hu:\n", anodeBitPos, anodes[anodeNum], rowGroup, groupIdx, bitOffset);
		uint_least32_t protoMask = ((uint_least32_t)1) << bitOffset;
		//printf("protoMask:%lx:\n", protoMask);
		*(uint_least32_t*)(driverShiftOut + 3*rowGroup) |= protoMask;
	}
/*
	for(uint_fast32_t byIdx = 0; byIdx < sizeof(driverShiftOut); byIdx++) {
		for(uint_fast32_t biIdx = 0; biIdx < 8; biIdx++) {
			printf("%i,", (driverShiftOut[byIdx] >> biIdx) & 0x1);
		}
	}
	puts("\n");
*/
	extern SPI_HandleTypeDef hspi1;
    HAL_SPI_Transmit(&hspi1, driverShiftOut, sizeof(driverShiftOut), HAL_MAX_DELAY);

    HAL_GPIO_WritePin(VFD_BLK_GPIO_Port, VFD_BLK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(VFD_BLK_GPIO_Port, VFD_LAT_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(VFD_BLK_GPIO_Port, VFD_LAT_Pin, GPIO_PIN_RESET);
    if(enableDisplay) {
    	HAL_GPIO_WritePin(VFD_BLK_GPIO_Port, VFD_BLK_Pin, GPIO_PIN_RESET);
    }

	gateGroup = (gateGroup + 1) % NGATEGROUPS;

	if(gateGroup == 0) {
		pIdx = ++shiftItr >> 4;
	}

	return gateGroup == 0;
}
