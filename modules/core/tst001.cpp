
#include <vfemu/Module.h>
#include <modules/tst001.h>

namespace vfemu {

namespace tst {


Status TST001Module::reset_receive(Module* receiver, void* data) {
	auto module = (TST001Module*) receiver;
	module->reset();
	return Status::SUCCESS;
}

Status TST001Module::clock_receive(Module* receiver, void* data) {
	auto module = (TST001Module*) receiver;
	if (module->running) {
		module->action();
	}
	return Status::SUCCESS;
}

Status TST001Module::data_receive(Module* receiver, void* data) {
	auto module = (TST001Module*) receiver;
	module->data = (u8) (unsigned long) data;
	return Status::SUCCESS;
}

Status TST001Module::io_receive(Module* receiver, void* data) {
	auto module = (TST001Module*) receiver;
	module->io_data = (u8) (unsigned long) data;
	return Status::SUCCESS;
}


void TST001Module::reset() {
	running = false;
	a = b = 0;
	ip = 0;
	running = true;
}

void TST001Module::stop() {
	running = false;
}


void TST001Module::action() {
	u8 instr = loadData(ip++);
	switch (instr >> 4) {
		case 0x0:
			action_control(instr);
			break;
		case 0x1:
			action_arith(instr);
			break;
		case 0x2:
			action_mem(instr);
			break;
		case 0x4:
			action_io(instr);
			break;
		default:
			reset();
	}
}

void TST001Module::action_control(u8 instr) {
	switch (instr) {
		case STOP:
			stop();
			break;
		case RESET:
			reset();
			break;
		case HLT:
		case NOP:
			break;
		default:
			reset();
	}
}

void TST001Module::action_arith(u8 instr) {
	u8 tmp;
	switch (instr) {
		case SETA:
			a = loadData(ip++); break;
		case SETB:
			b = loadData(ip++); break;
		case XCHG:
			tmp = a;
			a = b;
			b = tmp;
			break;
		case NOT:
			a = ~a; break;
		case AND:
			a &= b; break;
		case OR:
			a |= b; break;
		case XOR:
			a ^= b; break;
		case INC:
			a++; break;
		case DEC:
			a--; break;
		case ADD:
			a += b; break;
		case SUB:
			a -= b; break;
		default:
			reset();
	}
}

void TST001Module::action_mem(u8 instr) {
	u8 addr = loadData(ip++);
	switch (instr) {
		case LDA:
			a = loadData(addr); break;
		case LDB:
			b = loadData(addr); break;
		case STRA:
			storeData(addr, a); break;
		case STRB:
			storeData(addr, b); break;
		default:
			reset();
	}
}

void TST001Module::action_io(u8 instr) {
	switch (instr) {
		case IN:
			a = io_data; break;
		case OUT:
			sendToPort(6, a); break;
		default:
			reset();
	}
}


} // namespace tst

} // namespace vfemu


