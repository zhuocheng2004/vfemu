
#include <iostream>
#include <vfemu/constants.h>
#include <vfemu/Module.h>
#include <modules/pin.h>
#include <modules/char1.h>

using namespace std::chrono_literals;


namespace vfemu {

namespace char1 {

const std::vector<Port> char1_out_ports =  {
	Port("in", "pin8", char1_out_receive)
};

Status char1_out_receive(void* data) {
	u8 received = ((unsigned long) data & 0xff);
	std::cout << received << std::flush;
	return SUCCESS;
}


const std::vector<Port> char1_gen_ports =  {
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

}

}
