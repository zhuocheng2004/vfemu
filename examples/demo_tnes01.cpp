
#include <chrono>
#include <thread>
#include <vfemu/Connector.h>
#include <modules/pin.h>
#include <modules/gen.h>
#include <modules/pulsegen.h>

using namespace std::chrono_literals;
using namespace vfemu;

int main() {
	/*
	 * register "pin2pin" connector type 
	 */
	auto pin2pin = pin::Pin2pin();

	auto reset_ctrl = new gen::Gen1Module();			// signal to reset and start CPU
	auto clock = new pulsegen::PulseGenModule(5ms);			// clock source with period

	initModules({
		reset_ctrl, clock
	});

	/**
	 * Here we define how ports are connected.
	 * 
	 * The entry { "pin2pin", reset_ctrl, "out", cpu, "reset" }
	 * means connecting port "out" of reset_ctrl with port "reset" of cpu, 
	 * using connector type "pin2pin".
	 */
	connectPorts({
		{ &pin2pin, reset_ctrl, "out", clock, "reset" },
	});

	// enable clock source
	clock->enable();
	// reset cpu
	reset_ctrl->send(1);

	do {
		std::this_thread::sleep_for(1000ms);
	} while (1);

	return 0;
}