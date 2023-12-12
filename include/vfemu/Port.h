
#ifndef VFEMU_PORT_H
#define VFEMU_PORT_H

#include <vfemu/types.h>
#include <vfemu/Registry.h>

namespace vfemu {


struct vfemu_module;


/**
 * A Port Type specifies how data will be dealt and transferred
 * between two modules.
 */
class PortType {
public:
	/**
	 * name of this port type
	 */
	const char*			name;

	static Registry<PortType>	registry;

	inline PortType(const char *name) : name(name) { }
};

class Module;

/**
 * A port instance of a module 
 */
class Port {
public:
	/**
	 * unique identifier of the port
	 */
	const char*		id;

	/**
	 * type of this port
	 */
	const char*		type;

	/**
	 * Another module will call this method
	 * to send data to this module
	 */
	Status		(*receive)(Module& from, Module& to, void* data);

	/**
	 * This method needs to be set to NULL 
	 * during init. It will be filled during the
	 * matching process.
	 */
	Status		(*send)(Module& from, Module& to, void* data);

	/**
	 * Parent module instance the port belongs to
	 * It will be set during module instance init process.
	 */
	Module*		module;

	/**
	 * Destination module instance of the port.
	 * It will be set during matching process.
	 */
	Module*		dest;

	/**
	 * constructor
	 */
	inline Port(const char* id, const char* type, 
		Status (*receive)(Module&, Module&, void*) = nullptr) 
		: id(id), type(type), receive(receive) { }
};


} // namespace vfemu


#endif /* VFEMU_PORT_H */
