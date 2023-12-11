
#ifndef VFEMU_PORT_H
#define VFEMU_PORT_H

#include <vfemu/types.h>

namespace vfemu {

struct vfemu_module;


/**
 * A Port Type specifies how data will be dealt and transferred
 * between two modules.
 */
typedef struct {
	/**
	 * name of this port type
	 */
	const char*		name;
} VFEMUPortType;


/**
 * retrieve a port type given its name
 */
extern VFEMUPortType* getPortType(const char* name);

/*
 * [un]register a port type 
 */
extern VFEMUStatus registerPortType(VFEMUPortType* portType);
extern VFEMUStatus unregisterPortType(VFEMUPortType* portType);


/**
 * A port instance of a module 
 */
typedef struct {
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
	VFEMUStatus	(*receive)(struct vfemu_module* from, struct vfemu_module* to, void* data);

	/**
	 * This method needs to be set to NULL 
	 * during init. It will be filled during the
	 * matching process.
	 */
	VFEMUStatus	(*send)(struct vfemu_module* from, struct vfemu_module* to, void* data);

	/**
	 * Parent module instance the port belongs to
	 * It will be set during module instance init process.
	 */
	struct vfemu_module*	module;

	/**
	 * Destination module instance of the port.
	 * It will be set during matching process.
	 */
	struct vfemu_module*	dest;

	/**
	 * Additional private data.
	 */
	void		*data;
} VFEMUPort;

} // namespace vfemu


#endif /* VFEMU_PORT_H */
