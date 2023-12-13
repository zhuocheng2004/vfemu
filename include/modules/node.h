
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


#define NODE3U8 "node3u8"


class Node3u8Module : public Module {
public:
	inline Node3u8Module(const std::vector<Port> ports) : Module(ports) { }

	static Status p1_receive(Module* receiver, void* data);
	static Status p2_receive(Module* receiver, void* data);
	static Status p3_receive(Module* receiver, void* data);

private:
	static Status generic_receive(Module* receiver, int index, u8 data);
};

class Node3u8ModuleType : public ModuleType {
public:
	static std::vector<Port> node3u8_ports;

	inline Node3u8ModuleType() : ModuleType(NODE3U8, node3u8_ports) { }

	inline Node3u8Module* create() {
		return new Node3u8Module(ports);
	}
};


} // namespace nod

} // namespace vfemu



#endif /* VFEMU_MODULES_NODE_H */
