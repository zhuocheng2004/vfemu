
#include <iostream>
#include <nes/nespin.h>
#include <nes/nescm.h>

namespace vfemu {

namespace nes {


Status NESCM01Module::clock_receive(Module* receiver, u64 signal) {
	auto module = (NESCM01Module*) receiver;
	if (signal & 0x1)
		module->sendToPort(IDX_CART, NesConnector::toCartridgeData(1, 1, 0, 0));
	return Status::SUCCESS;
}

Status NESCM01Module::rw_receive(Module* receiver, u64 rw) {
	auto module = (NESCM01Module*) receiver;
	u16 addr = module->addr;

	bool inRAM = (addr & 0xe000) == 0;
	bool inPPU = (addr & 0xe000) == 0x2000;

	if (inRAM) {
		// in RAM range
		module->sendToPort(IDX_ADDR, addr);
		module->sendToPort(IDX_RW_RAM, rw);
	}
	
	if (inPPU) {
		// PPU register
		module->sendToPort(IDX_ADDR, addr & 0xF);
		module->sendToPort(IDX_RW_PPU, rw);
	}

	// always tell the cartridge
	module->sendToPort(IDX_CART, NesConnector::toCartridgeData(0, rw & 0x1, 0, 0));

	if (!inRAM && !inPPU) {
		// send cartridge data to CPU
		auto dat = (NesConnector::Data*) module->ports[IDX_CART].second->data;
		if (!dat)
			return Status::ERR_NULL;
		module->sendToPort(IDX_DATA, dat->data);
	}

	return Status::SUCCESS;
}

Status NESCM01Module::addr_receive(Module* receiver, u64 addr) {
	auto module = (NESCM01Module*) receiver;
	auto dat = (NesConnector::Data*) module->ports[IDX_CART].second->data;
	if (!dat)
		return Status::ERR_NULL;

	module->addr = addr;
	// not necessary. RAM is on the CPU addr bus.
	//module->sendToPort(IDX_ADDR, addr);
	dat->addr = addr;
	return Status::SUCCESS;
}

Status NESCM01Module::data_receive(Module* receiver, u64 data) {
	auto module = (NESCM01Module*) receiver;
	module->data = data & 0xff;
	return Status::SUCCESS;
}

Status NESCM01Module::cart_receive(Module* receiver, u64 data) {
	return Status::SUCCESS;
}


} // namespace nes

} // namespace vfemu

