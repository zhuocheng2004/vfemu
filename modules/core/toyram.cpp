
#include <cstring>
#include <modules/pin.h>
#include <modules/toyram.h>


namespace vfemu {

namespace toyram {


Status ToyRAM8x8Module::rw_receive(Module* receiver, u64 data) {
	auto module = (ToyRAM8x8Module*) receiver;
	if (data & 0x1) { // read
		module->sendToPort(IDX_DATA, module->mem[module->addr]);
	} else { // write
		module->mem[module->addr] = module->data;
	}
	return SUCCESS;
}

Status ToyRAM8x8Module::addr_receive(Module* receiver, u64 data) {
	auto module = (ToyRAM8x8Module*) receiver;
	module->addr = data & 0xff;
	return SUCCESS;
}

Status ToyRAM8x8Module::data_receive(Module* receiver, u64 data) {
	auto module = (ToyRAM8x8Module*) receiver;
	module->data = data & 0xff;
	return SUCCESS;
}


Status ToyRAM8x8Module::init() {
	mem = new u8[size];
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


Status ToyRAM8x16Module::rw_receive(Module* receiver, u64 data) {
	auto module = (ToyRAM8x16Module*) receiver;
	if (data & 0x1) { // read
		module->sendToPort(IDX_DATA, module->mem[module->addr]);
	} else { // write
		module->mem[module->addr] = module->data;
	}
	return SUCCESS;
}

Status ToyRAM8x16Module::addr_receive(Module* receiver, u64 data) {
	auto module = (ToyRAM8x16Module*) receiver;
	module->addr = data & 0xff;
	return SUCCESS;
}

Status ToyRAM8x16Module::data_receive(Module* receiver, u64 data) {
	auto module = (ToyRAM8x16Module*) receiver;
	module->data = data & 0xff;
	return SUCCESS;
}


Status ToyRAM8x16Module::init() {
	mem = new u8[size];
	if (!mem)
		return Status::ERR_NOMEM;
	if (initial_mem && initial_mem_size > 0)
		memcpy(mem, initial_mem, initial_mem_size);
	return Status::SUCCESS;
}

Status ToyRAM8x16Module::exit() {
	if (mem)
		delete mem;
	return Status::SUCCESS;
}


} // namespace toyram

} // namespace vfemu

