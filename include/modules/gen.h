
/*
 * toy data input module
 *
 * gen1: 1-bit generator
 * Ports:
 * 	out:	pin1	# data out
 * 
 * gen8: 8-bit generator
 * Ports:
 * 	out:	pin8	# data out
 */

#ifndef VFEMU_MODULES_GEN_H
#define VFEMU_MODULES_GEN_H

#include <vfemu/Module.h>


namespace vfemu {

namespace gen {


/* ================
 * 1-bit generator
 */

class Gen1Module : public Module {
public:
	inline Gen1Module() : Module({ 
		Port("out", "pin1")
	}) { }

	void send(const u8 data);
};


/* ================
 * 8-bit generator
 */

class Gen8Module : public Module {
public:
	inline Gen8Module() : Module({
		Port("out", "pin8")
	}) { }

	void send(const u8 data);
};


}

}


#endif /* VFEMU_MODULES_GEN_H */
