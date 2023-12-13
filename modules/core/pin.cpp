
#include <cstring>
#include <modules/pin.h>

namespace vfemu {

namespace pin {


static int getPinBits(Port* port) {
	int n = -1;
	sscanf(port->type, "pin%d", &n);
	return n;
}

inline static bool isPowerOfTwo(int n) {
	return (n & (n-1)) == 0;
}


Pin2pinConnector::Pin2pinConnector(Port* dest) : dest(dest) {
	bits = getPinBits(dest);
	if (bits <= 0) {
		dest = nullptr;
		bits = 0;
		mask = 0;
	} else {
		mask = (1 << bits) - 1;
	}
}

 Status Pin2pinConnector::send(void* data) {
	unsigned long v = (unsigned long) data;
	v &= mask;
	if (dest && dest->receive && dest->module) {
		dest->receive(dest->module, (void *) v);
	}
	return Status::SUCCESS;
}


Status Pin2pin::connect(Port* port1, Port* port2) {
	if (!port1 || !port2) {
		return Status::ERR_NULL;
	}
	if (port1->connector || port2->connector) {
		return Status::ERR_EXIST;
	}

	int bits1 = getPinBits(port1), bits2 = getPinBits(port2);
	if (bits1 <= 0 || bits1 > 32 || bits2 <= 0 || bits2 > 32
		|| !isPowerOfTwo(bits1) || !isPowerOfTwo(bits2)
		|| bits1 != bits2) {
		return Status::ERR_INVALID;
	}

	port1->connector = new Pin2pinConnector(port2);
	port2->connector = new Pin2pinConnector(port1);
	return Status::SUCCESS;
}

Status Pin2pin::disconnect(Port* port1, Port* port2) {
	if (port1) {
		delete port1->connector;
		port1->connector = nullptr;
	}
	if (port2) {
		delete port2->connector;
		port2->connector = nullptr;
	}

	if (!port1 || !port2) {
		return Status::ERR_NULL;
	}
	return Status::SUCCESS;
}


} // namespace pin

} // namespace vfemu

