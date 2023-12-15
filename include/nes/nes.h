
#ifndef VFEMU_NES_NES_H
#define VFEMU_NES_NES_H

#include <string>
#include <vfemu/types.h>


namespace vfemu {

namespace nes {


/** maigc */
static const u8 INES_MAG0 = 'N', INES_MAG1 = 'E', INES_MAG2 = 'S', INES_MAG3 = 0x1A;

/** 
 * mirroring
 * 0: horizontal, 1: vertcal
 */
static const u8 INES_HV = 0x01;

/**
 * battery-backed PRG RAM
 */
static const u8 INES_BB = 0x02;

/**
 * trainer
 */
static const u8 INES_TN = 0x04;

/**
 * ignore mirroring, provide four-screen VRAM 
 */
static const u8 INES_IM = 0x08;

/**
 * lower nybble of mapper number
 */
static const u8 INES_MPL = 0xf0;


class INESStruct {
public:
	/** is reading success */
	bool	success = false;
	std::string	errorMsg;

	u8	rpgSize;	// in 16k units
	u8	chrSize;	// in 8k units
	u8	flag;

	u8*	rpg = nullptr;
	u8*	chr = nullptr;

	/* uncommon fields */
	u8*	trainer = nullptr;

	INESStruct(const std::string& filename);
	~INESStruct();
};


} // namespace nes

} // namespace vfemu


#endif /* VFEMU_NES_NES_H */
