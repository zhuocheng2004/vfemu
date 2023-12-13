
#include <vfemu/Connector.h>
#include <modules/node.h>


namespace vfemu {

namespace node {


std::vector<Port> Node3u8ModuleType::node3u8_ports = {
	Port("p0", "pin8", Node3u8Module::p1_receive),
	Port("p1", "pin8", Node3u8Module::p2_receive),
	Port("p2", "pin8", Node3u8Module::p3_receive),
};

Status Node3u8Module::generic_receive(Module* receiver, int index, u8 data) {
	auto module = (Node3u8Module*) receiver;
	for (int i = 0; i < 3; i++) {
		if (i == index)
			continue;
		Port& port = module->ports[i];
		if (port.connector)
			port.connector->send(data);
	}
	return Status::SUCCESS;
}

Status Node3u8Module::p1_receive(Module* receiver, void* data) {
	return generic_receive(receiver, 0, (u8) (unsigned long) data);
}

Status Node3u8Module::p2_receive(Module* receiver, void* data) {
	return generic_receive(receiver, 1, (u8) (unsigned long) data);
}

Status Node3u8Module::p3_receive(Module* receiver, void* data) {
	return generic_receive(receiver, 2, (u8) (unsigned long) data);
}

} // namespace node

} // namespace vfemu

