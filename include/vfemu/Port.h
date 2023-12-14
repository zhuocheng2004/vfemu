
#ifndef VFEMU_PORT_H
#define VFEMU_PORT_H

#include <string>
#include <vfemu/types.h>
#include <vfemu/Registry.h>

namespace vfemu {


class Connector;
class Module;


class Port {
public:
	/**
	 * type name of this port, e.g. "pin8"
	 */
	const std::string		type;

	/**
	 * Another module will call this method (if not null)
	 * to send data to this module
	 */
	Status			(*receive)(Module* receiver, u64 data);

	/**
	 * the connector instance this port connects to
	 */
	Connector*		connector = nullptr;

	/**
	 * the module instance that this port is on
	 */
	Module*			module = nullptr;

	/**
	 * additional information
	 */
	void*			data = nullptr;

	/**
	 * constructor
	 */
	inline Port(const std::string& type, 
		Status (*receive)(Module*, u64) = nullptr) 
		: type(type), receive(receive) { }
};


} // namespace vfemu


#endif /* VFEMU_PORT_H */
