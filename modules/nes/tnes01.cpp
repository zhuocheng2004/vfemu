
#include <modules/pin.h>
#include <nes/tnes01.h>


namespace vfemu {

namespace nes {

using namespace std::chrono_literals;
using namespace node;
using namespace toyram;


TNES01Module::TNES01Module() {
	controller = new NESCM01Module();
	cpu = new NESCT01Module();
	ram = new ToyRAM8x16Module();
	clock = new PulseGenModule(10ms);
	node_addr = new Node3u16Module();
	node_data = new Node3u8Module();
	node_clock = new Node3u1Module();

	ports.push_back(std::make_pair("rst", cpu->getPort("rst")));
	ports.push_back(std::make_pair("cart", controller->getPort("cart")));
}


Status TNES01Module::init() {
	initModules({
		controller, cpu, ram, 
		node_addr, node_data, node_clock,
	});
	auto pin2pin = pin::Pin2pin();
	connectPorts({
		{ &pin2pin, node_addr, "p0", controller, "addr" },
		{ &pin2pin, node_addr, "p1", cpu, "addr" },
		{ &pin2pin, node_addr, "p2", ram, "addr" },

		{ &pin2pin, node_data, "p0", controller, "data" },
		{ &pin2pin, node_data, "p1", cpu, "data" },
		{ &pin2pin, node_data, "p2", ram, "data" },

		{ &pin2pin, clock, "out", node_clock, "p0" },
		{ &pin2pin, node_clock, "p1", cpu, "clk" },
		{ &pin2pin, node_clock, "p2", controller, "clk" },

		{ &pin2pin, cpu, "rw", controller, "rw" },
		{ &pin2pin, controller, "rw_ram", ram, "rw_ram" },
		{ &pin2pin, cpu, "rw", controller, "rw" },

		{ &pin2pin, controller, "irq", cpu, "irq" },
	});
	return Status::SUCCESS;
}

Status TNES01Module::exit() {
	exitModules({
		controller, cpu, ram, 
		node_addr, node_data, node_clock,
	});
	return Status::SUCCESS;
}


} // namespace nes

} // namespace vfemu
