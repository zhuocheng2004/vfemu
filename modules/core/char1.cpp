
#include <iostream>
#include <vfemu/constants.h>
#include <vfemu/Module.h>
#include <modules/pin.h>
#include <modules/char1.h>

using namespace std::chrono_literals;


namespace vfemu {

namespace char1 {

const std::vector<Port> char1_out_ports =  {
	Port("in", "pin8", Char1OutModule::char1_out_receive)
};

Status Char1OutModule::char1_out_receive(Module& from, Module& to, void* data) {
	u8 received = ((unsigned long) data & 0xff);
	std::cout << received << std::flush;
	return SUCCESS;
}


}

}
