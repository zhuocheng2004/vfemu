
/*
 * Toy Test CPU module
 *
 * tst001: 8bit-CPU
 * CPU doesn't start on init. Send signal to "reset" to start.
 * Ports:
 * 	reset:		pin1	# signal to reset and start
 * 	clock:		pin1	# clock source
 * 	load:		pin1	# signal the RAM to send data
 * 	store:		pin1	# signal the RAM to accept data
 * 	addr:		pin8	# set operation address of RAM
 * 	data:		pin8	# data transfer port with RAM
 * 	io:		pin8	# in/out port
 */

#ifndef VFEMU_MODULES_TST001_H
#define VFEMU_MODULES_TST001_H

#include <vfemu/Module.h>


namespace vfemu {

namespace tst {


class TST001Module : public Module {
public:
	inline TST001Module() : Module({
		Port("reset", "pin1", reset_receive),	// 0
		Port("clock", "pin1", clock_receive),	// 1
		Port("load",  "pin1"),			// 2
		Port("store", "pin1"),			// 3
		Port("addr",  "pin8"),			// 4
		Port("data",  "pin8", data_receive),	// 5
		Port("io",    "pin8", io_receive),	// 6
	}) { }

private:
	/** is CPU running */
	bool	running = false;

	/** instruction pointer */
	u8	ip = 0;

	/** two registers */
	u8	a = 0, b = 0;

	u8	data = 0;
	u8	io_data = 0;

	static Status reset_receive(Module* receiver, void* data);
	static Status clock_receive(Module* receiver, void* data);
	static Status data_receive(Module* receiver, void* data);
	static Status io_receive(Module* receiver, void* data);

	void reset();
	void stop();

	inline u8 loadData(u8 addr) {
		sendToPort(4, addr);	// "addr" < addr
		sendToPort(2, 1);	// "load" < 1
		return data;
	}

	inline void storeData(u8 addr, u8 value) {
		sendToPort(4, addr);	// "addr"  < addr
		sendToPort(5, value);	// "data"  < value
		sendToPort(3, 1);	// "store" < 1
	}

	void action();
	void action_control(u8 instr);
	void action_arith(u8 instr);
	void action_mem(u8 instr);
	void action_io(u8 instr);
};


/*
 * instruction codes
 * 'D' means the instruction needs an additional byte.
 */
const u8 NOP = 0x00, HLT = 0x01, RESET = 0x02, STOP = 0x03;

const u8 SETA = 0x10 /*D*/, SETB = 0x11 /*D*/, XCHG = 0x12, 
	NOT = 0x14, AND = 0x15, OR = 0x16, XOR = 0x17,
	INC = 0x18, DEC = 0x19, ADD = 0x1A, SUB = 0x1B;

const u8 LDA = 0x20 /*D*/, LDB = 0x21 /*D*/, STRA = 0x22 /*D*/, STRB = 0x23 /*D*/;

const u8 IN = 0x40, OUT = 0x41;


}

}


#endif /* VFEMU_MODULES_TST001_H */
