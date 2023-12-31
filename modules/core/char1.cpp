
#include <iostream>
#include <vfemu/Module.h>
#include <modules/char1.h>
#include <modules/pin.h>

using namespace std::chrono_literals;


namespace vfemu {

namespace char1 {


Status Char1OutModule::out_receive(Module* receiver, u64 data) {
	u8 received = data & 0xff;
	std::cout << (char) received << std::flush;
	return SUCCESS;
}


Status CChar1OutModule::out_receive(Module* receiver, u64 data) {
	auto module = (CChar1OutModule*) receiver;
	module->data = data & 0xff;
	return SUCCESS;
}

Status CChar1OutModule::ctrl_receive(Module* receiver, u64 data) {
	auto module = (CChar1OutModule*) receiver;
	if (data & 0x1) {
		std::cout << (char) module->data << std::flush;
	}
	return SUCCESS;
}


void Char1GenModule::char1_gen_thread(Char1GenModule* module) {
	auto ch = module->ch;
	auto interval = module->interval;
	auto port = module->ports[0].second;
	do {
		std::this_thread::sleep_for(interval);
		if (port->connector) {
			port->connector->send(ch);
		}
	} while(module->running);
}

Status Char1GenModule::init() {
	running = true;
	thread = new std::thread(char1_gen_thread, this);
	return SUCCESS;
}

Status Char1GenModule::exit() {
	running = false;
	thread->join();
	return Status::SUCCESS;
}


} // namespace char1

} // namespace vfemu
