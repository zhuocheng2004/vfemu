
#include <nes/nespin.h>


namespace vfemu {

namespace nes {


Status NesConnector::send(u64 data) {
	if (dest && dest->receive && dest->module) {
		dest->receive(dest->module, data);
	}
	return Status::SUCCESS;
}


Status NesConn::connect(Port* port1, Port* port2) {
	if (!port1 || !port2)
		return Status::ERR_NULL;

	if (port1->connector || port2->connector)
		return Status::ERR_EXIST;

	if (port1->type != "nes:nesconn" || port2->type != "nes:nesconn")
		return Status::ERR_INVALID;

	auto data = new NesConnector::Data();
	if (!data)
		return Status::ERR_NOMEM;


	port1->connector = new NesConnector(port2);
	port2->connector = new NesConnector(port1);
	port1->data = data;
	port2->data = data;

	return Status::SUCCESS;
}

Status NesConn::disconnect(Port* port1, Port* port2) {
	if (!port1 || !port2)
		return Status::ERR_NULL;

	if (port1->connector->getDest() != port2 ||
		port2->connector->getDest() != port1)
		return Status::ERR_INVALID;

	if (port1->data != port2->data)
		return Status::ERR_INVALID;

	delete port1->connector;
	port1->connector = nullptr;
	delete port2->connector;
	port2->connector = nullptr;

	if (port1->data) {
		delete (NesConnector::Data*) port1->data;
		port1->data = nullptr;
		port2->data = nullptr;
	}

	return Status::SUCCESS;
}


} // namespace pin

} // namespace vfemu

