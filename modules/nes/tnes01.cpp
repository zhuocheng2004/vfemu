
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
	ppu = new NESPPUModule();
	ram = new ToyRAM8x16Module(0x800);
	clock = new PulseGenModule(10us);
	node_addr = new Node4u16Module();
	node_data = new Node4u8Module();
	node_clock = new Node4u1Module();
	node_reset = new Node3u1Module();

	ports.push_back(std::make_pair("rst", node_reset->getPort("p0")));
	ports.push_back(std::make_pair("cart", controller->getPort("cart")));
}


Status TNES01Module::init() {
	initModules({
		controller, cpu, ppu, ram, clock,
		node_addr, node_data, node_clock, node_reset,
	});
	auto pin2pin = pin::Pin2pin();
	connectPorts({
		{ &pin2pin, node_reset, "p1", cpu, "rst" },
		{ &pin2pin, node_reset, "p2", ppu, "rst" },

		{ &pin2pin, node_addr, "p0", controller, "addr" },
		{ &pin2pin, node_addr, "p1", cpu, "addr" },
		{ &pin2pin, node_addr, "p2", ppu, "addr" },
		{ &pin2pin, node_addr, "p3", ram, "addr" },

		{ &pin2pin, node_data, "p0", controller, "data" },
		{ &pin2pin, node_data, "p1", cpu, "data" },
		{ &pin2pin, node_data, "p2", ppu, "data" },
		{ &pin2pin, node_data, "p3", ram, "data" },

		{ &pin2pin, node_clock, "p0", clock, "out" },
		{ &pin2pin, node_clock, "p1", cpu, "clk" },
		{ &pin2pin, node_clock, "p2", ppu, "clk" },
		{ &pin2pin, node_clock, "p3", controller, "clk" },

		{ &pin2pin, cpu, "rw", controller, "rw" },
		{ &pin2pin, controller, "rw_ram", ram, "rw" },
		{ &pin2pin, controller, "rw_ppu", ppu, "rw" },

		{ &pin2pin, controller, "irq", cpu, "irq" },

		{ &pin2pin, ppu, "int", cpu, "nmi" },

		{ &pin2pin, controller, "prd", ppu, "prd" },
		{ &pin2pin, controller, "pwr", ppu, "pwr" },
		{ &pin2pin, controller, "paddr", ppu, "paddr" },
		{ &pin2pin, controller, "pdata", ppu, "pdata" },
	});

	clock->enable();

	return Status::SUCCESS;
}

Status TNES01Module::exit() {
	exitModules({
		controller, cpu, ppu, ram, 
		node_addr, node_data, node_clock, node_reset,
	});
	return Status::SUCCESS;
}


} // namespace nes

} // namespace vfemu
