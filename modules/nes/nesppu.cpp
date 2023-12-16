
#include <iostream>
#include <nes/nesppu.h>


namespace vfemu {

namespace nes {


Status NESPPUModule::init() {
	render_thread = new std::thread(render_thread_func, this);
	return Status::SUCCESS;
}

Status NESPPUModule::exit() {
	return Status::SUCCESS;
}


Status NESPPUModule::clock_receive(Module* receiver, u64 signal) {
	return Status::SUCCESS;
}

Status NESPPUModule::reset_receive(Module* receiver, u64 signal) {
	auto module = (NESPPUModule*) receiver;
	module->running = false;

	module->vblank = true;
	module->nmi = false;
	//module->inc = true;
	module->baseNameTable = 0x2000;
	module->bgPTable = 0x0000;

	module->running = true;
	return Status::SUCCESS;
}

Status NESPPUModule::rw_receive(Module* receiver, u64 signal) {
	auto module = (NESPPUModule*) receiver;
	u8 data;
	if (!module->running)
		return Status::SUCCESS;
	if (signal & 0x1) {	// read
		switch (module->addr & 0xF) {
			case 0x2:	// PPUSTATUS
				data = 0x0;
				if (module->vblank)
					data |= MSTATUS_V;
				module->sendToPort(IDX_DATA, data);
				break;
		}
	} else {	// write
		data = module->data;
		switch (module->addr & 0xF) {
			case 0x0:	// PPUCTRL
				switch (data & 0x3) {
					// choose base nametable addr
					case 0x0:
						module->baseNameTable = 0x2000;
						break;
					case 0x1:
						module->baseNameTable = 0x2400;
						break;
					case 0x2:
						module->baseNameTable = 0x2800;
						break;
					case 0x3:
						module->baseNameTable = 0x2C00;
						break;
				}
				module->spPTable = (data & MCTRL_S) ? 0x1000 : 0x0000;
				module->bgPTable = (data & MCTRL_B) ? 0x1000 : 0x0000;
				module->nmi = (data & MCTRL_V) ? true : false;
				break;
		}
	}
	return Status::SUCCESS;
}

Status NESPPUModule::addr_receive(Module* receiver, u64 addr) {
	auto module = (NESPPUModule*) receiver;
	module->addr = addr;
	return Status::SUCCESS;
}

Status NESPPUModule::data_receive(Module* receiver, u64 data) {
	auto module = (NESPPUModule*) receiver;
	module->data = data;
	return Status::SUCCESS;
}

Status NESPPUModule::pdata_receive(Module* receiver, u64 data) {
	return Status::SUCCESS;
}


void NESPPUModule::render_thread_func(NESPPUModule* module) {
	;
}


} // namespace nes

} // namespace vfemu
