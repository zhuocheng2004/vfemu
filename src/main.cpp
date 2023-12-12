
#include <chrono>
#include <iostream>
#include <thread>
#include <vfemu/Module.h>

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

	gen::U8Controller *controller;

	auto gen = gen8.create(nullptr);
	auto out = char1out.create(nullptr);
	gen->init(&controller);
	out->init();
	pin2pin.connect(&gen->getPort(0), &out->getPort(0));

	do {
		controller->send('-');
		std::this_thread::sleep_for(150ms);
		controller->send('#');
		std::this_thread::sleep_for(500ms);
	} while (true);

	return 0;
}
