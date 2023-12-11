
#include <chrono>
#include <thread>
#include <vfemu/Module.h>

#include <modules/pin.h>
#include <modules/tst001.h>
#include <modules/char1.h>

using namespace std::chrono_literals;
using namespace vfemu;


int main() {
	registerPortType(&pin8);
	registerConnector(&pin2pin);
	registerModule(&char1_out_module);
	registerModule(&char1_gen_module);

	char1_gen_config config = {
		.ch =		'#',
		.interval =	250ms,
	};

	VFEMUModule gen = VFEMUModule();
	VFEMUModule out = VFEMUModule();
	char1_gen_module.copy(&gen, &char1_gen_module);
	char1_out_module.copy(&out, &char1_out_module);
	char1_gen_module.init(&gen, &config);
	char1_out_module.init(&out, nullptr);
	pin2pin.connect(&gen.ports[0], &out.ports[0]);

	do {
		std::this_thread::sleep_for(1000ms);
	} while (true);

	return 0;
}
