
/*
 * direct data transfer
 *
 * pin%d:
 * 	port type which transfers %d-bit data (%d should be power of two)
 * 
 * pin2pin:
 * 	connector type which connects two pinX's of the same bit-count X.
 */

#ifndef VFEMU_MODULES_PIN_H
#define VFEMU_MODULES_PIN_H

#include <vfemu/Port.h>
#include <vfemu/Connector.h>


namespace vfemu {

namespace pin {


class Pin2pinConnector : public Connector {
public:
	Pin2pinConnector(Port* dest);

	virtual Status send(void* data);

private:
	Port*		dest;
	u8		bits;
	unsigned long	mask;
};

class Pin2pin : public ConnectorType {
public:
	inline Pin2pin() : ConnectorType("pin2pin") { }

	virtual Status connect(Port* port1, Port* port2);
	virtual Status disconnect(Port* port1, Port* port2);
};


} // namespace pin

} // namespace vfemu


#endif /* VFEMU_MODULES_PIN_H */
