
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

private:
	/** is CPU running */
	bool		running = false;

	/* registers */
	/** program counter */
	u16		pc;
	/** stack pointer */
	u8		sp;

	u8		a, x, y;

	/** flag */
	u8		p;

	static Status clock_receive(Module* receiver, u64 data);
	static Status reset_receive(Module* receiver, u64 data);
	static Status nmi_receive(Module* receiver, u64 data);
	static Status data_receive(Module* receiver, u64 data);
};


} // namespace nes

} // namespace vfemu


#endif /* VFEMU_NES_NESCT01_H */
