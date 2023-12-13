
/*
 * Toy Periodic Pulse Generator Module
 *
 * pulsegen
 * Module is disabled on init.
 * Ports:
 * 	out:		pin1	# pulses will be emitted from this port
 * 	enable:		pin1	# clock enable
 */

#ifndef VFEMU_MODULES_PULSEGEN_H
#define VFEMU_MODULES_PULSEGEN_H

#include <chrono>
#include <thread>
#include <vfemu/Module.h>


namespace vfemu {

namespace pulsegen {


class PulseGenModule : public Module {
public:
	inline PulseGenModule(const std::chrono::microseconds& period) 
		: Module({
			Port("out", "pin1"),
			Port("enable", "pin1", enable_receive)
		}), period(period) { }

	
	virtual Status init();

	virtual Status exit();

	inline void enable() {
		enabled = true;
	}

	inline void disable() {
		enabled = false;
	}

private:
	std::chrono::microseconds	period;
	bool				enabled = false;

	bool				running = false;
	std::thread			*thread = nullptr;

	static Status enable_receive(Module* receiver, void* data);

	static void pulse_gen_thread(PulseGenModule*);
};


} // namespace pulsegen

} // namespace vfemu


#endif /* VFEMU_MODULES_PULSEGEN_H */
