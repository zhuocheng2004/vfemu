
#ifndef VFEMU_MODULE_H
#define VFEMU_MODULE_H

#include <string>
#include <utility>
#include <vector>
#include <vfemu/types.h>
#include <vfemu/Connector.h>
#include <vfemu/Port.h>


namespace vfemu {


/**
 * A Module is a reusable component.
 */
class Module {
public:
	inline Module() { }

	Module(const std::vector<std::pair<const std::string, Port*>> ports);

	virtual Status			init(void);
	virtual Status			exit(void);
	
	inline Port* getPort(int index) {
		return ports[index].second;
	}

	Port* getPort(const std::string& id);

	Status initPorts();

protected:
	/**
	 * the list of ports
	 */
	std::vector<std::pair<const std::string, Port*>>		ports;

	inline Status sendToPort(int portIndex, u64 data) {
		Port* port = ports[portIndex].second;
		if (port->connector)
			return port->connector->send(data);
		return Status::SUCCESS;
	}
};


Status initModules(std::vector<Module*> modules);
Status exitModules(std::vector<Module*> modules);


} // namespace vfemu


#endif /* VFEMU_MODULE_H */
