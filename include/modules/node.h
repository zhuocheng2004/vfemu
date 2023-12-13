
/*
 * node module that serves as a joint for multiple connections.
 * 
 * Note that data will allways be transferred to all other connected ports. 
 * So this is not directional.
 *
 * node3u8:	3-ends u8 node
 * Ports:
 * 	p0:	pin8
 * 	p1:	pin8
 * 	p2:	pin8
 */

#ifndef VFEMU_MODULES_NODE_H
#define VFEMU_MODULES_NODE_H

#include <vfemu/Module.h>


namespace vfemu {

namespace node {


class Node3u8Module : public Module {
public:
	inline Node3u8Module() : Module({
		Port("p0", "pin8", Node3u8Module::p1_receive),
		Port("p1", "pin8", Node3u8Module::p2_receive),
		Port("p2", "pin8", Node3u8Module::p3_receive)
	}) { }

private:
	static Status generic_receive(Module* receiver, int index, u8 data);

	inline static Status p1_receive(Module* receiver, void* data) {
		return generic_receive(receiver, 0, (u8) (unsigned long) data);
	}

	inline static Status p2_receive(Module* receiver, void* data) {
		return generic_receive(receiver, 1, (u8) (unsigned long) data);
	}

	inline static Status p3_receive(Module* receiver, void* data) {
		return generic_receive(receiver, 2, (u8) (unsigned long) data);
	}
};


} // namespace nod

} // namespace vfemu



#endif /* VFEMU_MODULES_NODE_H */
