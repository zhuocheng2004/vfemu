
#ifndef VFEMU_PORT_H
#define VFEMU_PORT_H

#include <vfemu/types.h>
#include <vfemu/Registry.h>

namespace vfemu {


class Connector;
class Module;


class Port {
public:
	/**
	 * unique identifier of the port on a module
	 */
	const char*		id;

	/**
	 * type name of this port, e.g. "pin8"
	 */
	const char*		type;

	/**
	 * Another module will call this method (if not null)
	 * to send data to this module
	 */
	Status		(*receive)(Module* receiver, void* data);

	/**
	 * the connector instance this port connects to
	 */
	Connector*	connector = nullptr;

	/**
	 * the module instance that this port is on
	 */
	Module*		module = nullptr;

	/**
	 * constructor
	 */
	inline Port(const char* id, const char* type, 
		Status (*receive)(Module*, void*) = nullptr) 
		: id(id), type(type), receive(receive) { }
};


} // namespace vfemu


#endif /* VFEMU_PORT_H */
