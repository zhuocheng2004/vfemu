
#include <iostream>
#include <vfemu/Connector.h>
#include <modules/pin.h>
#include <modules/node.h>
#include <modules/gen.h>
#include <modules/char1.h>
#include <modules/toyram8x8.h>

using namespace std::chrono_literals;
using namespace vfemu;


int main() {
	const char DATA[] = "ABCDEFGH XYZW\n 01234567 !@#$%^&()[]{}\\| \r\n";


	auto pin2pin = pin::Pin2pin();
	ConnectorType::registry.add(&pin2pin);
	auto node3u8 = node::Node3u8ModuleType();
	auto gen1 = gen::Gen1ModuleType();
	auto gen8 = gen::Gen8ModuleType();
	auto cchar1out = char1::CChar1OutModuleType();
	auto toyram8x8 = toyram::ToyRAM8x8ModuleType();

	auto load_ctrl = gen1.create();
	auto store_ctrl = gen1.create();
	auto out_ctrl = gen1.create();
	auto addr_ctrl = gen8.create();
	auto input = gen8.create();
	auto out = cchar1out.create();
	auto ram = toyram8x8.create();
	auto node = node3u8.create();

	out->init();
	ram->init();
	node->init();

	vfemu::connectPorts({
		{ "pin2pin", load_ctrl, "out", ram, "load" },
		{ "pin2pin", store_ctrl, "out", ram, "store" },
		{ "pin2pin", addr_ctrl, "out", ram, "addr" },
		{ "pin2pin", out_ctrl, "out", out, "ctrl" },
		{ "pin2pin", ram, "data", node, "p0" },
		{ "pin2pin", input, "out", node, "p1" },
		{ "pin2pin", out, "in", node, "p2" },
	});


	for (int i = 0; i < sizeof(DATA); i++) {
		addr_ctrl->send(i);
		input->send(DATA[i]);
		store_ctrl->send(1);
	}

	for (int i = 0; i < sizeof(DATA); i++) {
		addr_ctrl->send(i);
		load_ctrl->send(1);
		out_ctrl->send(1);
	}

	return 0;
}
