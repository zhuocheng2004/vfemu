
#include <cstdio>
#include <vfemu/Connector.h>
#include <modules/gen.h>


namespace vfemu {

namespace gen {


void Gen1Module::send(const u8 data) {
	sendToPort(0, data);
}

void Gen8Module::send(const u8 data) {
	sendToPort(0, data);
}

void Gen16Module::send(const u16 data) {
	sendToPort(0, data);
}


}

}
