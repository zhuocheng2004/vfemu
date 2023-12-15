
/**
 * NES Cartridge Simulation Modules
 *
 * CartMapper0Module: implementation of mapper0
 * Ports:
 * 	cart:		nes:nesconn	# port to connect to main board
 */

#ifndef VFEMU_NES_CARTRIDGE_H
#define VFEMU_NES_CARTRIDGE_H

#include <vfemu/Module.h>
#include <nes/nes.h>
#include <nes/nespin.h>


namespace vfemu {

namespace nes {


class CartMapper0Module : public Module {
public:
	CartMapper0Module(const INESStruct&);

	static const int IDX_CART = 0;

	Status 	init();
	Status 	exit();

private:
	/** NES ROM information */
	INESStruct	info;

	u16		addr;

	static Status cart_receive(Module* receiver, u64 data);
};



} // namespace nes

} // namespace vfemu


#endif /* VFEMU_NES_CARTRIDGE_H */
