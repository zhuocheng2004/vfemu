
#ifndef VFEMU_CONNECTOR_H
#define VFEMU_CONNECTOR_H

#include <string>
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
	inline Connector(Port* dest) : dest(dest) { }

	inline virtual Status send(u64 data) {
		return Status::SUCCESS;
	}

	inline Port* getDest() {
		return dest;
	}

protected:
	Port*	dest;
};


class Module;


class ConnectorType {
public:

	inline ConnectorType() { }

	/**
	 * method to [dis]connect two ports
	 */
	virtual Status connect(Port* port1, Port* port2);
	virtual Status disconnect(Port* port1, Port* port2);

	virtual Status connect(Module* module1, const std::string& id1, Module* module2, const std::string& id2);
	virtual Status disconnect(Module* module1, const std::string& id1, Module* module2, const std::string& id2);
};


typedef struct {
	ConnectorType*		connectorType;
	Module*			module1;
	const std::string	id1;
	Module*			module2;
	const std::string	id2;
} ConnectionInfo;

Status connectPorts(std::vector<ConnectionInfo> infos);
Status disconnectPorts(std::vector<ConnectionInfo> infos);


} // namespace vfemu


#endif /* VFEMU_CONNECTOR_H */
