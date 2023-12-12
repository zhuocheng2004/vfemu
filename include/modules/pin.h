
#ifndef VFEMU_MODULES_PIN_H
#define VFEMU_MODULES_PIN_H

#include <vfemu/Port.h>
#include <vfemu/Connector.h>


namespace vfemu {

namespace pin {


#define PIN8 "pin8"


class Pin8 : public PortType {
public:
	Pin8() : PortType(PIN8) { }
};

class Pin2pinConnector : public Connector {
public:
	inline Pin2pinConnector(Port* dest) : dest(dest) { }

	inline virtual Status send(void* data) {
		dest->receive(data);
		return Status::SUCCESS;
	}
private:
	Port* dest;
};

class Pin2pin : public ConnectorType {
public:
	virtual Status connect(Port* port1, Port* port2);
	virtual Status disconnect(Port* port1, Port* port2);
private:
	static bool arePortsValid(Port* port1, Port* port2);
};


} // namespace pin

} // namespace vfemu


#endif /* VFEMU_MODULES_PIN_H */
