
#include <cstring>
#include <modules/pin.h>
#include <modules/toyram8x8.h>


namespace vfemu {

namespace toyram {


Status ToyRAM8x8Module::load_receive(Module* receiver, void* data) {
	if ((unsigned long) data & 0x1) {
		ToyRAM8x8Module* module = (ToyRAM8x8Module*) receiver;
		Port& dataPort = module->ports[3];
		if (dataPort.connector)
			dataPort.connector->send(module->mem[module->addr]);
	}
	return SUCCESS;
}

Status ToyRAM8x8Module::store_receive(Module* receiver, void* data) {
	if ((unsigned long) data & 0x1) {
		ToyRAM8x8Module* module = (ToyRAM8x8Module*) receiver;
		module->mem[module->addr] = module->data;
	}
	return SUCCESS;
}

Status ToyRAM8x8Module::addr_receive(Module* receiver, void* data) {
	ToyRAM8x8Module* module = (ToyRAM8x8Module*) receiver;
	module->addr = (unsigned long) data & 0xff;
	return SUCCESS;
}

Status ToyRAM8x8Module::data_receive(Module* receiver, void* data) {
	ToyRAM8x8Module* module = (ToyRAM8x8Module*) receiver;
	module->data = (unsigned long) data & 0xff;
	return SUCCESS;
}


Status ToyRAM8x8Module::init() {
	mem = new u8[1 << 8];
	if (!mem)
		return Status::ERR_NOMEM;
	if (initial_mem && initial_mem_size > 0)
		memcpy(mem, initial_mem, initial_mem_size);
	return Status::SUCCESS;
}

Status ToyRAM8x8Module::exit() {
	if (mem)
		delete mem;
	return Status::SUCCESS;
}


} // namespace toyram

} // namespace vfemu

