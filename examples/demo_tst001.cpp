
#include <chrono>
#include <thread>
#include <vfemu/Connector.h>
#include <modules/pin.h>
#include <modules/gen.h>
#include <modules/char1.h>
#include <modules/pulsegen.h>
#include <modules/toyram.h>
#include <modules/tst001.h>

using namespace std::chrono_literals;
using namespace vfemu;

int main() {
	/*
	 * register "pin2pin" connector type 
	 */
	auto pin2pin = pin::Pin2pin();

	/*
	 * Initial Memory 
	 */
	const u8 MEM[] = {
		tst001::SETA, 0x30,	// 00
		tst001::LDB, 0x10,	// 02
		tst001::ADD,		// 04
		tst001::OUT,		// 05
		tst001::SETA, 0x40,	// 06
		tst001::LDB, 0x11,	// 08
		tst001::SUB,		// 0A
		tst001::OUT,		// 0B
		tst001::STOP,		// 0C
		0x00, 0x00, 0x00, 	// 0D
		0x01, 0x07,		// 10
	};

	/**
	 * generate module instances of different types
	 */
	auto cpu = new tst001::TST001Module();				// CPU
	auto ram = new toyram::ToyRAM8x8Module(MEM, sizeof(MEM));	// RAM
	auto clock = new pulsegen::PulseGenModule(5ms);			// clock source with period
	auto out = new char1::Char1OutModule();				// output
	auto reset_ctrl = new gen::Gen1Module();			// signal to reset and start CPU

	initModules({
		cpu, ram, clock, out, reset_ctrl
	});

	/**
	 * Here we define how ports are connected.
	 * 
	 * The entry { "pin2pin", reset_ctrl, "out", cpu, "reset" }
	 * means connecting port "out" of reset_ctrl with port "reset" of cpu, 
	 * using connector type "pin2pin".
	 */
	connectPorts({
		{ &pin2pin, reset_ctrl, "out", cpu, "reset" },
		{ &pin2pin, clock, "out", cpu, "clock" },
		{ &pin2pin, cpu, "rw", ram, "rw" },
		{ &pin2pin, cpu, "addr", ram, "addr" },
		{ &pin2pin, cpu, "data", ram, "data" },
		{ &pin2pin, cpu, "io", out, "in" },
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