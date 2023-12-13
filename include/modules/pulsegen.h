
/*
 * Toy Periodic Pulse Generator Module
 *
 * pulsegen
 * Ports:
 * 	out:	pin1	# pulses will be emitted from this port
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
			Port("out", "pin1")
		}), period(period) { }

	
	virtual Status init();

	virtual Status exit();

	inline void enable() {
		masked = false;
	}

	inline void disable() {
		masked = true;
	}

private:
	std::chrono::microseconds	period;
	bool				masked = false;

	bool				running = false;
	std::thread			*thread = nullptr;

	static void pulse_gen_thread(PulseGenModule*);
};


} // namespace pulsegen

} // namespace vfemu


#endif /* VFEMU_MODULES_PULSEGEN_H */
