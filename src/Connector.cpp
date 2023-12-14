
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


Status ConnectorType::connect(Module* module1, const std::string& id1, Module* module2, const std::string& id2) {
	auto port1 = module1->getPort(id1), port2 = module2->getPort(id2);
	if (!port1 || !port2) {
		return Status::ERR_NONEXIST;
	}
	return connect(port1, port2);
}

Status ConnectorType::disconnect(Module* module1, const std::string& id1, Module* module2, const std::string& id2) {
	auto port1 = module1->getPort(id1), port2 = module2->getPort(id2);
	if (!port1 || !port2) {
		return Status::ERR_NONEXIST;
	}
	return disconnect(port1, port2);
}


Status connectPorts(std::vector<ConnectionInfo> infos) {
	Status status = Status::SUCCESS;
	for (auto& info : infos) {
		ConnectorType* connectorType = info.connectorType;
		if (!connectorType) {
			status = Status::WARNING;
			continue;
		}

		if (connectorType->connect(info.module1, info.id1, info.module2, info.id2) != Status::SUCCESS) {
			status = Status::WARNING;
		}
	}
	return status;
}

Status disconnectPorts(std::vector<ConnectionInfo> infos) {
	Status status = Status::SUCCESS;
	for (auto& info : infos) {
		ConnectorType* connectorType = info.connectorType;
		if (!connectorType) {
			status = Status::WARNING;
			continue;
		}

		if (connectorType->disconnect(info.module1, info.id1, info.module2, info.id2) != Status::SUCCESS) {
			status = Status::WARNING;
		}
	}
	return status;
}


}
