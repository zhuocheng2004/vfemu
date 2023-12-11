
#ifndef VFEMU_MODULE_H
#define VFEMU_MODULE_H

#include <vfemu/types.h>
#include <vfemu/Port.h>


namespace vfemu {

/**
 * A Module is a reusable component.
 * This struct contains the meta information,
 * but can also serves as module instances.
 */
typedef struct vfemu_module {
	/* ================================
	 * Header
	 * This will never change in the future.
	 */

	/**
	 * minimum vfemu version to load this module
	 */
	u32		min_sdk_version;

	/**
	 * vfemu version the module targets to
	 * You can just set it to the VFEMU_VERSION constant.
	 */
	u32		target_sdk_version;

	/**
	 * name of the module
	 */
	const char*		name;

	// Header End

	/* ================================
	 * Module method
	 */

	/**
	 * This will be called when an instance
	 * of this module is plugged in.
	 */
	VFEMUStatus	(*init)(struct vfemu_module* module, void* config);

	/**
	 * This will be called when an instance 
	 * of this module disconnects.
	 */
	VFEMUStatus	(*exit)(struct vfemu_module* module);

	/**
	 * copy module data
	 * this will be called when instanciating.
	 */
	VFEMUStatus	(*copy)(struct vfemu_module* to, struct vfemu_module* from);

	/* ================================
	 * Module port info
	 */

	/**
	 * number of ports
	 */
	int		num_ports;

	/**
	 * the list of ports
	 */
	VFEMUPort*	ports;

	/**
	 * Additional private data.
	 */
	void*		data;
} VFEMUModule;


/**
 * retrieve a module prototype given its name
 */
extern VFEMUModule* getModule(const char* name);

/*
 * [un]register a module prototype
 */
extern VFEMUStatus registerModule(VFEMUModule* module);
extern VFEMUStatus unregisterModule(VFEMUModule* module);


extern VFEMUStatus simple_copy(VFEMUModule* to, VFEMUModule* from);

} // namespace vfemu


#endif /* VFEMU_MODULE_H */
