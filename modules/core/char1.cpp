
#include <iostream>
#include <vfemu/constants.h>
#include <vfemu/Module.h>
#include <modules/pin.h>
#include <modules/char1.h>

using namespace std::chrono_literals;
using namespace vfemu;


VFEMUStatus char1_out_receive(VFEMUModule* from, VFEMUModule* to, void* data) {
	u8 received = ((unsigned long) data & 0xff);
	std::cout << received << std::flush;
	return SUCCESS;
}


static VFEMUPort char1_out_ports[] = {
	{
		.id =		"in",
		.type =		"pin8",
		.receive =	char1_out_receive,
	}
};


static VFEMUStatus char1_out_init(VFEMUModule* module, void* config) {
	module->data = (void*) 0;
	module->ports[0].module = module;
	return SUCCESS;
}


static VFEMUStatus char1_out_exit(VFEMUModule* module) {
	module->ports[0].module = nullptr;
	return SUCCESS;
}


VFEMUModule char1_out_module = {
	.min_sdk_version =	VFEMU_VERSION,
	.target_sdk_version =	VFEMU_VERSION,
	.name =			"char1",

	.init =			char1_out_init,
	.exit =			char1_out_exit,
	.copy =			simple_copy,

	.num_ports =		1,
	.ports =		char1_out_ports,

	.data =			(void*) 0,
};


/*
 * one-byte periodic generator
 */

typedef struct {
	char1_gen_config*	config;
	std::thread*		thread;
} char1_gen_data;

static VFEMUPort char1_gen_ports[] = {
	{
		.id =		"out",
		.type =		"pin8",
	}
};


static void char1_gen_thread(VFEMUModule* module) {
	VFEMUPort port = module->ports[0];
	auto data = (char1_gen_data*) module->data;
	while (true)
	{
		std::this_thread::sleep_for(data->config->interval);
		if (port.send) {
			port.send(module, port.dest, (void *) (unsigned long) data->config->ch);
		}
	}
}

static VFEMUStatus char1_gen_init(VFEMUModule* module, void* config) {
	module->ports[0].module = module;
	auto thread = new std::thread(char1_gen_thread, module);
	auto data = new char1_gen_data();

	data->config = (char1_gen_config*) config;
	data->thread = thread;
	module->data = data;

	return SUCCESS;
}


static VFEMUStatus char1_gen_exit(VFEMUModule* module) {
	module->ports[0].module = nullptr;
	auto data = (char1_gen_data*) module->data;
	delete data;
	return SUCCESS;
}


VFEMUModule char1_gen_module = {
	.min_sdk_version =	VFEMU_VERSION,
	.target_sdk_version =	VFEMU_VERSION,
	.name =			"char1",

	.init =			char1_gen_init,
	.exit =			char1_gen_exit,
	.copy =			simple_copy,

	.num_ports =		1,
	.ports =		char1_gen_ports,
};
