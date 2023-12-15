
#include <iostream>
#include <nes/nesct01.h>

namespace vfemu {

namespace nes {


Status NESCT01Module::clock_receive(Module* receiver, u64 data) {
	auto module = (NESCT01Module*) receiver;
	if (module->running && (data & 0x1))
		module->action();
	return Status::SUCCESS;
}

Status NESCT01Module::reset_receive(Module* receiver, u64 data) {
	auto module = (NESCT01Module*) receiver;
	if (data & 0x1)
		module->reset();
	return Status::SUCCESS;
}

Status NESCT01Module::nmi_receive(Module* receiver, u64 data) {
	return Status::SUCCESS;
}

Status NESCT01Module::data_receive(Module* receiver, u64 data) {
	auto module = (NESCT01Module*) receiver;
	module->data = data & 0xff;
	return Status::SUCCESS;
}


void NESCT01Module::reset() {
	running = false;
	// 0xfffc-0xfffd contains the entry point of reset routine
	u8 resetPosLow = loadData(0xfffc), resetPosHigh = loadData(0xfffd);
	pc = (resetPosHigh << 8) + resetPosLow;
	a = x = y = 0;
	sp = 0xfd;
	data = 0;
	running = true;
}

void NESCT01Module::action() {
	u8 instr = loadData(pc++);
	printf("  %02X\n", instr);
}

} // namespace nes

} // namespace vfemu

