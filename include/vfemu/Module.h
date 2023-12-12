
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
	/* ================================
	 * Module method
	 */
	Module(const std::vector<Port> ports);

	/**
	 * This will be called when an instance
	 * of this module is plugged in.
	 */
	virtual Status			init(void);

	/**
	 * This will be called when an instance 
	 * of this module disconnects.
	 */
	virtual Status			exit(void);
	
	inline Port& getPort(int index) {
		return ports[index];
	}

	Port* getPort(const char* id);

protected:
	/* ================================
	 * Module port info
	 */

	/**
	 * the list of ports
	 */
	std::vector<Port>		ports;
};


/**
 * This struct contains the meta information 
 * of a type of module.
 */
class ModuleType {
	friend Module;
public:
	/* ================================
	 * Header
	 * Memory layout of this will never change in the future.
	 */

	/**
	 * minimum vfemu version to load this module
	 * Registration will fail if not compatible.
	 */
	const u32			min_vfemu_version;

	/**
	 * vfemu version the module targets to
	 * You can just set it to the VFEMU_VERSION constant.
	 */
	const u32			target_vfemu_version;

	/**
	 * name of the module
	 */
	const char*			name;

	/*
	 * Header end
	 */

	/* ================================
	 * Module port info
	 */

	/**
	 * the list of port definitions
	 */
	std::vector<Port>		ports;

	static Registry<ModuleType>	registry;


	/* ================================
	 * Module Type method
	 */
	inline ModuleType(const char* name, const int num_ports, const std::vector<Port> ports) 
		: min_vfemu_version(vfemu::VERSION), target_vfemu_version(vfemu::VERSION),
		 name(name), ports(ports) { }

	/**
	 * method to create module instance
	 */
	inline virtual Module*		create() {
		return nullptr;
	}
}; // ModuleType


} // namespace vfemu


#endif /* VFEMU_MODULE_H */
