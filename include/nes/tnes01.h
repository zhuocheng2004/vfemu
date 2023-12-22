
/*
 * Test NES MainBoard Module
 *
 * Ports:
 * 	rst:		pin1	# machine reset (low for reset state, high to run)
 *  exit:		pin1	# output exit signal when PPU exits
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
#include <nes/nesppu.h>


namespace vfemu {

namespace nes {

using namespace node;
using namespace toyram;
using namespace pulsegen;


class TNES01Module : public Module {
public:
	TNES01Module(bool vertical_mirroring = true);

	Status init();
	Status exit();

private:
	bool				vertical_mirroring = true;
	Port				exit_port;

	NESCM01Module*		controller;
	NESCT01Module*		cpu;
	NESPPUModule*		ppu;
	ToyRAM8x16Module*	ram;
	PulseGenModule*		clock;
	Node4u16Module*		node_addr;
	Node4u8Module*		node_data;
	Node4u1Module*		node_clock;
	Node3u1Module*		node_reset;

	static Status exit_receive(Module* receiver, u64 signal);
};



} // namespace nes

} // namespace vfemu


#endif /* VFEMU_NES_TNES01_H */
