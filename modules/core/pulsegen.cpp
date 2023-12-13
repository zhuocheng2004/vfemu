
#include <vfemu/Connector.h>
#include <modules/pulsegen.h>

using namespace std::chrono_literals;


namespace vfemu {

namespace pulsegen {


Status PulseGenModule::init() {
	running = true;
	thread = new std::thread(pulse_gen_thread, this);
	return SUCCESS;
}

Status PulseGenModule::exit() {
	running = false;
	thread->join();
	return Status::SUCCESS;
}


void PulseGenModule::pulse_gen_thread(PulseGenModule* module) {
	auto port = &module->ports[0];
	while (module->running) {
		auto time = std::chrono::steady_clock::now();
		auto expected = time + module->period;
		if (module->enabled && port->connector) {
			port->connector->send(1);
		}
		if (module->period > 0ms)
			std::this_thread::sleep_until(expected);
	}
}

Status PulseGenModule::enable_receive(Module* receiver, void* data) {
	auto module = (PulseGenModule*) receiver;
	if ((unsigned long) data & 0x1)
		module->enabled = true;
	else
		module->enabled = false;
	return SUCCESS;
}


} // namespace pulsegen

} // namespace vfemu

