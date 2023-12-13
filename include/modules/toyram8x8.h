
/*
 * Toy RAM module
 *
 * toyram8x8: 8bit data x 8bit address
 * Ports:
 * 	load:	pin1	# signal to send data at some address to port "data"
 * 	store:	pin1	# signal to save data to some address from port "data"
 * 	addr:	pin8	# set the address of operation
 * 	data:	pin8	# data in/out port
 */

#ifndef VFEMU_MODULES_TOYRAM8X8_H
#define VFEMU_MODULES_TOYRAM8X8_H

#include <vfemu/Module.h>


namespace vfemu {

namespace toyram {


class ToyRAM8x8Module : public Module {
public:
	inline ToyRAM8x8Module(const u8* initial_mem, const unsigned int initial_mem_size)
		: Module({
			Port("load",  "pin1", ToyRAM8x8Module::load_receive),
			Port("store", "pin1", ToyRAM8x8Module::store_receive),
			Port("addr",  "pin8", ToyRAM8x8Module::addr_receive),
			Port("data",  "pin8", ToyRAM8x8Module::data_receive),
		}), initial_mem(initial_mem), initial_mem_size(initial_mem_size) { }
	
	inline ToyRAM8x8Module() : ToyRAM8x8Module(nullptr, 0) { }

	Status init();

	Status exit();

private:
	u8*	mem = nullptr;
	u8	addr = 0;
	u8	data = 0;

	const u8*		initial_mem;
	const unsigned int 	initial_mem_size;

	static Status load_receive(Module* receiver, void* data);
	static Status store_receive(Module* receiver, void* data);
	static Status addr_receive(Module* receiver, void* data);
	static Status data_receive(Module* receiver, void* data);
};


} // namespace toyram
	
} // namespace vfemu


#endif /* VFEMU_MODULES_TOYRAM8X8_H */