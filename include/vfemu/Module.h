
#ifndef VFEMU_MODULE_H
#define VFEMU_MODULE_H

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
	Module(const std::vector<Port> ports);

	virtual Status			init(void);
	virtual Status			exit(void);
	
	inline Port& getPort(int index) {
		return ports[index];
	}

	Port* getPort(const char* id);

	Status initPorts();

protected:
	/**
	 * the list of ports
	 */
	std::vector<Port>		ports;

	inline Status sendToPort(int portIndex, void* data) {
		Port& port = ports[portIndex];
		if (port.connector)
			return port.connector->send(data);
		return Status::SUCCESS;
	}

	inline Status sendToPort(int portIndex, unsigned long data) {
		Port& port = ports[portIndex];
		if (port.connector)
			return port.connector->send(data);
		return Status::SUCCESS;
	}
};


Status initModules(std::vector<Module*> modules);


} // namespace vfemu


#endif /* VFEMU_MODULE_H */
