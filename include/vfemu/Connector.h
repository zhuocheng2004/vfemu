
#ifndef VFEMU_CONNECTOR_H
#define VFEMU_CONNECTOR_H


#include <vfemu/types.h>
#include <vfemu/Port.h>


namespace vfemu {

/**
 * A connector defines how two ports should be connected
 */
typedef struct {
	/**
	 * name of the connector
	 */
	const char*	name;

	/**
	 * method to connect two ports
	 */
	VFEMUStatus	(*connect)(VFEMUPort* port1, VFEMUPort* port2);

	/**
	 * method to disconnect two ports
	 */
	VFEMUStatus	(*disconnect)(VFEMUPort* port1, VFEMUPort* port2);
} VFEMUConnector;


extern VFEMUConnector* getConnector(const char* name);

extern VFEMUStatus registerConnector(VFEMUConnector* connector);
extern VFEMUStatus unregisterConnector(VFEMUConnector* connector);

extern VFEMUStatus connectPort(const char* connectorName, VFEMUPort* port1, VFEMUPort* port2);
extern VFEMUStatus disconnectPort(const char* connectorName, VFEMUPort* port1, VFEMUPort* port2);


} // namespace vfemu


#endif /* VFEMU_CONNECTOR_H */
