
/*
 * direct data transfer
 *
 * pinX:
 * 	port type which transfers X-bit data (X should be power of two)
 * 
 * pin2pin:
 * 	connector type which connects two pinX with the same bit-count X.
 */

#ifndef VFEMU_MODULES_PIN_H
#define VFEMU_MODULES_PIN_H

#include <vfemu/Port.h>
#include <vfemu/Connector.h>


namespace vfemu {

namespace pin {


#define PIN1	"pin1"
#define PIN2	"pin2"
#define PIN4	"pin4"
#define PIN8	"pin8"
#define PIN16	"pin16"
#define PIN32	"pin32"


class Pin1 : public PortType {
public:
	inline Pin1() : PortType(PIN1) { }
};

class Pin2 : public PortType {
public:
	inline Pin2() : PortType(PIN1) { }
};

class Pin4 : public PortType {
public:
	inline Pin4() : PortType(PIN1) { }
};

class Pin8 : public PortType {
public:
	inline Pin8() : PortType(PIN8) { }
};


class Pin2pinConnector : public Connector {
public:
	Pin2pinConnector(Port* dest);

	inline virtual Status send(void* data) {
		unsigned long v = (unsigned long) data;
		v &= mask;
		if (dest && dest->receive && dest->module) {
			dest->receive(dest->module, (void *) v);
		}
		return Status::SUCCESS;
	}
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
