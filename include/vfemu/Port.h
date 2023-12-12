
#ifndef VFEMU_PORT_H
#define VFEMU_PORT_H

#include <vfemu/types.h>
#include <vfemu/Registry.h>

namespace vfemu {


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


class Connector;


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
	Status		(*receive)(void* data);

	/**
	 * the connector instance that this port connects to
	 */
	Connector*	connector;

	/**
	 * constructor
	 */
	inline Port(const char* id, const char* type, 
		Status (*receive)(void*) = nullptr) 
		: id(id), type(type), receive(receive), connector(nullptr) { }
};


} // namespace vfemu


#endif /* VFEMU_PORT_H */
