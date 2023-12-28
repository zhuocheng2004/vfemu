
/*
 * Test NES CPU Module
 *
 * Ports:
 * 	clk:		pin1	# clock source
 * 	rst:		pin1	# reset (low for reset state, high to run)
 * 	nmi:		pin1	# Non-Maskable Interrupt
 * 	irq:		pin1	# IRQ
 * 	rw:		pin1	# Read/Write (low for write, high for read)
 * 	addr:		pin16	# memory address
 * 	data:		pin8	# memory r/w data
 */

#ifndef VFEMU_NES_NESCT01_H
#define VFEMU_NES_NESCT01_H

#include <mutex>
#include <vfemu/Module.h>
#include <nes/nes.h>


namespace vfemu {

namespace nes {


class NESCT01Module : public Module {
public:
	inline NESCT01Module() : Module({
		std::make_pair("clk", new Port("pin1", clock_receive)),
		std::make_pair("rst", new Port("pin1", reset_receive)),
		std::make_pair("nmi", new Port("pin1", nmi_receive)),
		std::make_pair("irq", new Port("pin1")),
		std::make_pair("rw", new Port("pin1")),
		std::make_pair("addr", new Port("pin16")),
		std::make_pair("data", new Port("pin8", data_receive)),
	}) { }

	static const int IDX_CLK = 0, IDX_RST = 1, IDX_NMI = 2, IDX_IRQ = 3;
	static const int IDX_RW = 4, IDX_ADDR = 5, IDX_DATA = 6;

	static const u8 MSK_CARRY = 0x01, MSK_ZERO = 0x02, MSK_INT = 0x04 /* set to disable non-NMI interrupts */, MSK_DEC = 0x08;
	static const u8 MSK_B = 0x30, MSK_OVERFLOW = 0x40, MSK_NEG = 0x80;

private:
	/** is CPU running? */
	bool		running = false;
	
	/** level of IRQ interrupt handling */
	int			in_irq = 0;

	/** action lock */
	std::mutex	lock;

	u8		data = 0;

	/* registers */
	/** program counter */
	u16		pc;
	/** stack pointer */
	u8		sp;

	u8		a, x, y;

	/** flag */
	u8		p;

	static Status clock_receive(Module*, u64);
	static Status reset_receive(Module*, u64);
	static Status nmi_receive(Module*, u64);
	static Status data_receive(Module*, u64);

	u8 loadData(u16 addr);
	void storeData(u16 addr, u8 value);
	void oamDMAWrite(u8 base);

	inline void pushStack(u8 value) {
		storeData(0x0100 + sp, value);
		sp--;
	}

	inline u8 popStack() {
		sp++;
		u8 v = loadData(0x0100 + sp);
		return v;
	}

	u8 adjustZ(u8 v);
	u8 adjustN(u8 v);

	inline u8 adjustZN(u8 v) {
		adjustZ(v);
		adjustN(v);
		return v;
	}

	u8 add(u8 v1, u8 v2);
	u8 sub(u8 v1, u8 v2);
	void bit(u8 v);
	void cmp(u8 v1, u8 v2);
	u8 asl(u8 v);
	u8 lsr(u8 v);
	u8 rol(u8 v);
	u8 ror(u8 v);
	

	u16 branch(u8 offset);

	void reset();
	void action();
	void branch_irq(u16 vector_addr);

	void action_control(u8 instr);
	void action_alu(u8 instr);
	void action_mv(u8 instr);	// read-modify-write
	void action_other(u8 instr);	// unofficial opcodes
};


} // namespace nes

} // namespace vfemu


#endif /* VFEMU_NES_NESCT01_H */
