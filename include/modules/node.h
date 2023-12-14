
/*
 * node module that serves as a joint for multiple connections.
 * 
 * Note that data will allways be transferred to all other connected ports. 
 * So this is not directional.
 *
 * node3u%d:	3-ends u%d node		(%d: 8, 16)
 * Ports:
 * 	p0:	pin%d
 * 	p1:	pin%d
 * 	p2:	pin%d
 */

#ifndef VFEMU_MODULES_NODE_H
#define VFEMU_MODULES_NODE_H

#include <vfemu/Module.h>


namespace vfemu {

namespace node {


class Node3u1Module : public Module {
public:
	inline Node3u1Module() : Module({
		std::make_pair("p0", new Port("pin1", p0_receive)),
		std::make_pair("p1", new Port("pin1", p1_receive)),
		std::make_pair("p2", new Port("pin1", p2_receive)),
	}) { }

private:
	static Status generic_receive(Module* receiver, int index, u64 data);

	inline static Status p0_receive(Module* receiver, u64 data) {
		return generic_receive(receiver, 0, data);
	}

	inline static Status p1_receive(Module* receiver, u64 data) {
		return generic_receive(receiver, 1, data);
	}

	inline static Status p2_receive(Module* receiver, u64 data) {
		return generic_receive(receiver, 2, data);
	}
};


class Node3u8Module : public Module {
public:
	inline Node3u8Module() : Module({
		std::make_pair("p0", new Port("pin8", p0_receive)),
		std::make_pair("p1", new Port("pin8", p1_receive)),
		std::make_pair("p2", new Port("pin8", p2_receive)),
	}) { }

private:
	static Status generic_receive(Module* receiver, int index, u64 data);

	inline static Status p0_receive(Module* receiver, u64 data) {
		return generic_receive(receiver, 0, data);
	}

	inline static Status p1_receive(Module* receiver, u64 data) {
		return generic_receive(receiver, 1, data);
	}

	inline static Status p2_receive(Module* receiver, u64 data) {
		return generic_receive(receiver, 2, data);
	}
};


class Node3u16Module : public Module {
public:
	inline Node3u16Module() : Module({
		std::make_pair("p0", new Port("pin16", p0_receive)),
		std::make_pair("p1", new Port("pin16", p1_receive)),
		std::make_pair("p2", new Port("pin16", p2_receive)),
	}) { }

private:
	static Status generic_receive(Module* receiver, int index, u64 data);

	inline static Status p0_receive(Module* receiver, u64 data) {
		return generic_receive(receiver, 0, data);
	}

	inline static Status p1_receive(Module* receiver, u64 data) {
		return generic_receive(receiver, 1, data);
	}

	inline static Status p2_receive(Module* receiver, u64 data) {
		return generic_receive(receiver, 2, data);
	}
};


} // namespace nod

} // namespace vfemu



#endif /* VFEMU_MODULES_NODE_H */
