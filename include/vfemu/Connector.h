
#ifndef VFEMU_CONNECTOR_H
#define VFEMU_CONNECTOR_H


#include <vector>
#include <vfemu/types.h>
#include <vfemu/Registry.h>
#include <vfemu/Port.h>


namespace vfemu {


/**
 * A connector defines how two ports should be connected
 */
class Connector {
public:
	inline virtual Status send(void* data) {
		return Status::SUCCESS;
	}

	inline virtual Status send(unsigned long data) {
		return send((void*) data);
	}
};


class Module;


class ConnectorType {
public:
	/**
	 * name of the connector type
	 */
	const char*		name;

	static Registry<ConnectorType>	registry;

	inline ConnectorType(const char* name) : name(name) { }

	/**
	 * method to [dis]connect two ports
	 */
	virtual Status connect(Port* port1, Port* port2);
	virtual Status disconnect(Port* port1, Port* port2);

	virtual Status connect(Module* module1, const char* id1, Module* module2, const char* id2);
	virtual Status disconnect(Module* module1, const char* id1, Module* module2, const char* id2);
};


typedef struct {
	const char*	connectorType;
	Module*		module1;
	const char*	id1;
	Module*		module2;
	const char*	id2;
} ConnectionInfo;

Status connectPorts(std::vector<ConnectionInfo> infos);


} // namespace vfemu


#endif /* VFEMU_CONNECTOR_H */
