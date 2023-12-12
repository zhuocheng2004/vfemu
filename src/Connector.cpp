
#include <list>
#include <vfemu/Connector.h>
#include <vfemu/Module.h>

namespace vfemu {


Status ConnectorType::connect(Port* port1, Port* port2) {
	return Status::SUCCESS;
}

Status ConnectorType::disconnect(Port* port1, Port* port2) {
	return Status::SUCCESS;
}


Status ConnectorType::connect(Module* module1, const char* id1, Module* module2, const char* id2) {
	auto port1 = module1->getPort(id1), port2 = module2->getPort(id2);
	if (!port1 || !port2) {
		return Status::ERR_NONEXIST;
	}
	return connect(port1, port2);
}

Status ConnectorType::disconnect(Module* module1, const char* id1, Module* module2, const char* id2) {
	auto port1 = module1->getPort(id1), port2 = module2->getPort(id2);
	if (!port1 || !port2) {
		return Status::ERR_NONEXIST;
	}
	return disconnect(port1, port2);
}


}
