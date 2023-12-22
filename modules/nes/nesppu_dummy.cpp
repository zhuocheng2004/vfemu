
#include <iostream>
#include <nes/nesppu.h>


namespace vfemu {

namespace nes {


void NESPPUModule::render_thread_func(NESPPUModule* module) {
	std::cout << "Warning: running without GUI." << std::endl;
}


} // namespace nes

} // namespace vfemu
