
#include <chrono>
#include <iostream>

#include <modules/pin.h>
#include <modules/gen.h>
#include <modules/char1.h>

using namespace std::chrono_literals;
using namespace vfemu;


int main() {
	auto pin8 = pin::Pin8();
	auto pin2pin = pin::Pin2pin();
	auto gen8 = gen::Gen8ModuleType();
	auto char1out = char1::Char1OutModuleType();
	auto char1gen = char1::Char1GenModuleType();

	auto gen = char1gen.create({
		.ch = 'A',
		.interval = 100ms,
	});
	auto out = char1out.create();
	gen->init();
	out->init();

	auto gen2 = char1gen.create({
		.ch = 'B',
		.interval = 500ms,
	});
	auto out2 = char1out.create();
	gen2->init();
	out2->init();

	pin2pin.ConnectorType::connect(gen, "out", out, "in");
	pin2pin.ConnectorType::connect(gen2, "out", out2, "in");

	do {
		std::this_thread::sleep_for(500ms);
	} while (true);

	return 0;
}
