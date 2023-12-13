
#include <vfemu/Connector.h>
#include <modules/node.h>


namespace vfemu {

namespace node {


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


} // namespace node

} // namespace vfemu

