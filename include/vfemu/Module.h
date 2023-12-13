
#ifndef VFEMU_MODULE_H
#define VFEMU_MODULE_H

#include <vector>
#include <vfemu/constants.h>
#include <vfemu/types.h>
#include <vfemu/Registry.h>
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
};


Status initModules(std::vector<Module*> modules);


} // namespace vfemu


#endif /* VFEMU_MODULE_H */
