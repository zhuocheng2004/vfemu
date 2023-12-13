
#include <iostream>
#include <vfemu/Module.h>
#include <modules/char1.h>
#include <modules/pin.h>

using namespace std::chrono_literals;


namespace vfemu {

namespace char1 {



std::vector<Port> Char1OutModuleType::char1_out_ports = {
	Port("in", "pin8", Char1OutModule::out_receive)
};


Status Char1OutModule::out_receive(Module* receiver, void* data) {
	u8 received = ((unsigned long) data & 0xff);
	std::cout << (char) received << std::flush;
	return SUCCESS;
}


std::vector<Port> CChar1OutModuleType::cchar1_out_ports = {
	Port("in", "pin8", CChar1OutModule::out_receive),
	Port("ctrl", "pin1", CChar1OutModule::ctrl_receive),
};


Status CChar1OutModule::out_receive(Module* receiver, void* data) {
	auto module = (CChar1OutModule*) receiver;
	module->data = ((unsigned long) data & 0xff);
	return SUCCESS;
}

Status CChar1OutModule::ctrl_receive(Module* receiver, void* data) {
	auto module = (CChar1OutModule*) receiver;
	if ((unsigned long) data & 0x1) {
		std::cout << (char) module->data << std::flush;
	}
	return SUCCESS;
}


std::vector<Port> Char1GenModuleType::char1_gen_ports =  {
	Port("out", "pin8")
};

void Char1GenModule::char1_gen_thread(Char1GenModule* module) {
	auto ch = module->ch;
	auto interval = module->interval;
	auto port = &module->ports[0];
	do {
		std::this_thread::sleep_for(interval);
		if (port->connector) {
			port->connector->send(ch);
		}
	} while(module->running);
}

Status Char1GenModule::init() {
	running = true;
	thread = new std::thread(Char1GenModule::char1_gen_thread, this);
	return SUCCESS;
}

Status Char1GenModule::exit() {
	running = false;
	thread->join();
	return Status::SUCCESS;
}


} // namespace char1

} // namespace vfemu
