
/*
 * Toy RAM module
 *
 * toyram8x8: 8bit data x 8bit address
 * Ports:
 * 	rw:	pin1	# read/write signal (low for write, high for read)
 * 	addr:	pin8	# set the address of operation
 * 	data:	pin8	# data in/out port
 * 
 * toyram8x16: 8bit data x 16bit address
 * Ports:
 * 	rw:	pin1	# read/write signal (low for write, high for read)
 * 	addr:	pin16	# set the address of operation
 * 	data:	pin8	# data in/out port
 */

#ifndef VFEMU_MODULES_TOYRAM_H
#define VFEMU_MODULES_TOYRAM_H

#include <vfemu/Module.h>


namespace vfemu {

namespace toyram {


class ToyRAM8x8Module : public Module {
public:
	inline ToyRAM8x8Module(const u8* initial_mem, const unsigned int initial_mem_size, unsigned int size = 1 << 8)
		: Module({
			std::make_pair("rw", new Port("pin1", rw_receive)),
			std::make_pair("addr", new Port("pin8", addr_receive)),
			std::make_pair("data", new Port("pin8", data_receive)),
		}), initial_mem(initial_mem), initial_mem_size(initial_mem_size), size(size) { }

	static const int IDX_RW = 0, IDX_ADDR = 1, IDX_DATA = 2;
	
	inline ToyRAM8x8Module(unsigned int size = 1 << 8) : ToyRAM8x8Module(nullptr, 0, size) { }

	Status init();

	Status exit();

private:
	unsigned int 	size;
	u8*		mem = nullptr;
	u8		addr = 0;
	u8		data = 0;

	const u8*		initial_mem;
	const unsigned int 	initial_mem_size;

	static Status rw_receive(Module* receiver, u64 data);
	static Status addr_receive(Module* receiver, u64 data);
	static Status data_receive(Module* receiver, u64 data);
};


class ToyRAM8x16Module : public Module {
public:
	inline ToyRAM8x16Module(const u8* initial_mem, const unsigned int initial_mem_size, unsigned int size = 1 << 16)
		: Module({
			std::make_pair("rw", new Port("pin1", rw_receive)),
			std::make_pair("addr", new Port("pin16", addr_receive)),
			std::make_pair("data", new Port("pin8", data_receive)),
		}), initial_mem(initial_mem), initial_mem_size(initial_mem_size), size(size) { }

	static const int IDX_RW = 0, IDX_ADDR = 1, IDX_DATA = 2;
	
	inline ToyRAM8x16Module(unsigned int size = 1 << 16) : ToyRAM8x16Module(nullptr, 0, size) { }

	Status init();
	Status exit();

private:
	unsigned int 	size;
	u8*		mem = nullptr;
	u16		addr = 0;
	u8		data = 0;

	const u8*		initial_mem;
	const unsigned int 	initial_mem_size;

	static Status rw_receive(Module* receiver, u64 data);
	static Status addr_receive(Module* receiver, u64 data);
	static Status data_receive(Module* receiver, u64 data);
};


} // namespace toyram
	
} // namespace vfemu


#endif /* VFEMU_MODULES_TOYRAM8X8_H */