
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


#define TOYRAM8X8 "toyram8x8"


class ToyRAM8x8Module : public Module {
public:
	inline ToyRAM8x8Module(const std::vector<Port> ports,
		const char* initial_mem, 
		const unsigned int initial_mem_size)
		: Module(ports), initial_mem(initial_mem), initial_mem_size(initial_mem_size) { }

	Status init();

	Status exit();

	static Status load_receive(Module* receiver, void* data);
	static Status store_receive(Module* receiver, void* data);
	static Status addr_receive(Module* receiver, void* data);
	static Status data_receive(Module* receiver, void* data);
private:
	u8*	mem;
	u8	addr;
	u8	data;

	const char*		initial_mem;
	const unsigned int 	initial_mem_size;
};


class ToyRAM8x8ModuleType : public ModuleType {
public:
	static std::vector<Port> toyram8x8_ports;

	inline ToyRAM8x8ModuleType() : ModuleType(TOYRAM8X8, toyram8x8_ports) { }

	inline ToyRAM8x8Module* create(const char* initial_mem, const unsigned int initial_mem_size) {
		return new ToyRAM8x8Module(ports, initial_mem, initial_mem_size);
	}

	inline ToyRAM8x8Module* create() {
		return new ToyRAM8x8Module(ports, nullptr, 0);
	}
};



} // namespace toyram
	
} // namespace vfemu


#endif /* VFEMU_MODULES_TOYRAM8X8_H */