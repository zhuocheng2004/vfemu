
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


class Pin2pin : public Connector {
public:
	static Status connect(Port* port1, Port* port2);

	static Status disconnect(Port* port1, Port* port2);
private:
	static bool arePortsValid(Port* port1, Port* port2);
};


} // namespace pin

} // namespace vfemu


#endif /* VFEMU_MODULES_PIN_H */
