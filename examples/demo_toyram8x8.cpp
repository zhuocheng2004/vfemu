
#include <vfemu/Connector.h>
#include <modules/pin.h>
#include <modules/node.h>
#include <modules/gen.h>
#include <modules/char1.h>
#include <modules/toyram8x8.h>

using namespace std::chrono_literals;
using namespace vfemu;


int main() {
	const u8 DATA[] = "ABCDEFGH 0123456789\nHello World!\n";


	auto pin2pin = pin::Pin2pin();
	ConnectorType::registry.add(&pin2pin);

	auto out = new char1::CChar1OutModule();
	auto ram = new toyram::ToyRAM8x8Module();
	auto node = new node::Node3u8Module();
	auto load_ctrl = new gen::Gen1Module();
	auto store_ctrl = new gen::Gen1Module();
	auto out_ctrl = new gen::Gen1Module();
	auto addr_ctrl = new gen::Gen8Module();
	auto input = new gen::Gen8Module();

	initModules({
		node, ram, out,
		input, load_ctrl, store_ctrl, out_ctrl, addr_ctrl,
	});

	connectPorts({
		{ "pin2pin", load_ctrl, "out", ram, "load" },
		{ "pin2pin", store_ctrl, "out", ram, "store" },
		{ "pin2pin", addr_ctrl, "out", ram, "addr" },
		{ "pin2pin", out_ctrl, "out", out, "ctrl" },
		{ "pin2pin", ram, "data", node, "p0" },
		{ "pin2pin", input, "out", node, "p1" },
		{ "pin2pin", out, "in", node, "p2" },
	});

	/**
	 * write data to toy ram
	 */
	for (int i = 0; i < sizeof(DATA); i++) {
		addr_ctrl->send(i);
		input->send(DATA[i]);
		store_ctrl->send(1);
	}

	/**
	 * print data from toy ram
	 */
	for (int i = 0; i < sizeof(DATA); i++) {
		addr_ctrl->send(i);
		load_ctrl->send(1);
		out_ctrl->send(1);
	}

	return 0;
}
