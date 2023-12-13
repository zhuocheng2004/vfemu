
#include <vfemu/Connector.h>
#include <modules/pulsegen.h>

using namespace std::chrono_literals;


namespace vfemu {

namespace pulsegen {


void PulseGenModule::pulse_gen_thread(PulseGenModule* module) {
	auto port = &module->ports[0];
	while (module->running) {
		auto time = std::chrono::steady_clock::now();
		auto expected = time + module->period;
		if (!module->masked && port->connector) {
			port->connector->send(1);
		}
		if (module->period > 0ms)
			std::this_thread::sleep_until(expected);
	}
}

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


} // namespace pulsegen

} // namespace vfemu

