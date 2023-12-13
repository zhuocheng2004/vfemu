
/*
 * toy one-char output/generation module
 *
 * char1_out: output whenever data is reveiced
 * Ports:
 * 	in:	pin8	# data in
 * 
 * cchar1_out: output when signaled.
 * Ports:
 * 	in:	pin8	# data in
 * 	ctrl:	pin1	# signal to output data
 * 
 * char1_gen: periodic generation of one char
 * Ports:
 * 	out:	pin8	# data out
 */

#ifndef VFEMU_MODULES_CHAR1_H
#define VFEMU_MODULES_CHAR1_H

#include <chrono>
#include <thread>
#include <vfemu/Module.h>


namespace vfemu {

namespace char1 {


/*
 * one-char always-output module
 */

class Char1OutModule : public Module {
public:
	inline Char1OutModule() : Module({
		Port("in", "pin8", Char1OutModule::out_receive)
	}) { }

private:
	static Status out_receive(Module* receiver, void* data);
};


/*
 * one-char controlled-output module
 */

class CChar1OutModule : public Module {
public:
	inline CChar1OutModule() : Module({
		Port("in", "pin8", CChar1OutModule::out_receive),
		Port("ctrl", "pin1", CChar1OutModule::ctrl_receive)
	}) { }

private:
	u8	data = 0;

	static Status out_receive(Module* receiver, void* data);
	static Status ctrl_receive(Module* receiver, void* data);
};


/*
 * 1-char periodic generator
 */

class Char1GenModule : public Module {
public:
	inline Char1GenModule(const u8 ch, const std::chrono::milliseconds& interval) 
		: Module({
			Port("out", "pin8")
		}), ch(ch), interval(interval) { }
	
	virtual Status init();

	virtual Status exit();

private:
	const u8			ch;
	const std::chrono::milliseconds	interval;

	bool				running = false;
	std::thread			*thread = nullptr;

	static void char1_gen_thread(Char1GenModule* module);
};


}

}


#endif /* VFEMU_MODULES_CHAR1_H */
