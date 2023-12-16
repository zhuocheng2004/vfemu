
#include <vfemu/Connector.h>
#include <modules/node.h>


namespace vfemu {

namespace node {


Status Node3u1Module::generic_receive(Module* receiver, int index, u64 data) {
	auto module = (Node3u1Module*) receiver;
	for (int i = 0; i < 3; i++) {
		if (i == index)
			continue;
		module->sendToPort(i, data & 0x1);
	}
	return Status::SUCCESS;
}

Status Node4u1Module::generic_receive(Module* receiver, int index, u64 data) {
	auto module = (Node4u1Module*) receiver;
	for (int i = 0; i < 4; i++) {
		if (i == index)
			continue;
		module->sendToPort(i, data & 0x1);
	}
	return Status::SUCCESS;
}


Status Node3u8Module::generic_receive(Module* receiver, int index, u64 data) {
	auto module = (Node3u8Module*) receiver;
	for (int i = 0; i < 3; i++) {
		if (i == index)
			continue;
		module->sendToPort(i, data & 0xff);
	}
	return Status::SUCCESS;
}

Status Node4u8Module::generic_receive(Module* receiver, int index, u64 data) {
	auto module = (Node4u8Module*) receiver;
	for (int i = 0; i < 4; i++) {
		if (i == index)
			continue;
		module->sendToPort(i, data & 0xff);
	}
	return Status::SUCCESS;
}


Status Node3u16Module::generic_receive(Module* receiver, int index, u64 data) {
	auto module = (Node3u16Module*) receiver;
	for (int i = 0; i < 3; i++) {
		if (i == index)
			continue;
		module->sendToPort(i, data & 0xffff);
	}
	return Status::SUCCESS;
}

Status Node4u16Module::generic_receive(Module* receiver, int index, u64 data) {
	auto module = (Node4u16Module*) receiver;
	for (int i = 0; i < 4; i++) {
		if (i == index)
			continue;
		module->sendToPort(i, data & 0xffff);
	}
	return Status::SUCCESS;
}


} // namespace node

} // namespace vfemu

