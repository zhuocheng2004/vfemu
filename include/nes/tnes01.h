
/*
 * Test NES MainBoard Module
 *
 * Ports:
 * 	rst:		pin1	# machine reset (low for reset state, high to run)
 * 	cart:		nes:nesconn	# port to connect to cartridge
 */

#ifndef VFEMU_NES_TNES01_H
#define VFEMU_NES_TNES01_H

#include <vfemu/Module.h>
#include <modules/pin.h>
#include <modules/node.h>
#include <modules/toyram.h>
#include <modules/pulsegen.h>
#include <nes/nespin.h>
#include <nes/nescm.h>
#include <nes/nesct01.h>


namespace vfemu {

namespace nes {

using namespace node;
using namespace toyram;
using namespace pulsegen;


class TNES01Module : public Module {
public:
	TNES01Module();

	Status init();
	Status exit();

private:
	NESCM01Module*		controller;
	NESCT01Module*		cpu;
	ToyRAM8x16Module*	ram;
	PulseGenModule*		clock;
	Node3u16Module*		node_addr;
	Node3u8Module*		node_data;
	Node3u1Module*		node_clock;
};



} // namespace nes

} // namespace vfemu


#endif /* VFEMU_NES_TNES01_H */
