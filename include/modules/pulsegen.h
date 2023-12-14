
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
	inline PulseGenModule(const std::chrono::nanoseconds& period, bool enabledOnCreate = false) 
		: Module({
			std::make_pair("out", new Port("pin1")),
			std::make_pair("enable", new Port("pin1", enable_receive)),
		}), period(period), enabled(enabledOnCreate) { }

	
	virtual Status init();

	virtual Status exit();

	inline void enable() {
		enabled = true;
	}

	inline void disable() {
		enabled = false;
	}

private:
	std::chrono::nanoseconds	period;
	bool				enabled;

	bool				running = false;
	std::thread			*thread = nullptr;

	static Status enable_receive(Module* receiver, u64 data);

	static void pulse_gen_thread(PulseGenModule*);
};


} // namespace pulsegen

} // namespace vfemu


#endif /* VFEMU_MODULES_PULSEGEN_H */
