
/*
 * Test NES PPU Module (depends on SDL2)
 *
 * Ports:
 * 	clk:		pin1	# clock source
 * 	rst:		pin1	# reset (low for reset state, high to run)
 * 	rw:		pin1	# CPU Read/Write (low for write, high for read)
 * 	int:		pin1	# connects to /NMI of CPU
 * 	addr:		pin16	# CPU address (only use 3 bits)
 * 	data:		pin8	# CPU data
 * 	prd:		pin1	# PPU read
 * 	pwr:		pin1	# PPU write
 * 	paddr:		pin16	# PPU address
 * 	pdata:		pin8	# PPU data
 */

#ifndef VFEMU_NES_NESPPU_H
#define VFEMU_NES_NESPPU_H

#include <thread>
#include <vfemu/Module.h>
#include <nes/nes.h>


namespace vfemu {

namespace nes {


class NESPPUModule : public Module {
public:
	inline NESPPUModule() : Module({
		std::make_pair("clk", new Port("pin1", clock_receive)),
		std::make_pair("rst", new Port("pin1", reset_receive)),
		std::make_pair("rw", new Port("pin1", rw_receive)),
		std::make_pair("int", new Port("pin1")),
		std::make_pair("addr", new Port("pin16", addr_receive)),
		std::make_pair("data", new Port("pin8", data_receive)),
		std::make_pair("prd", new Port("pin8")),
		std::make_pair("pwr", new Port("pin8")),
		std::make_pair("paddr", new Port("pin16")),
		std::make_pair("pdata", new Port("pin8", pdata_receive)),
	}) { }

	Status init();
	Status exit();

	static const int IDX_CLK = 0, IDX_RST = 1, IDX_RW = 2, IDX_INT = 3;
	static const int IDX_ADDR = 4, IDX_DATA = 5;
	static const int IDX_PRD = 6, IDX_PWR = 7, IDX_PADDR = 8, IDX_PDATA = 9;

	static const int MCTRL_I = 0x04, MCTRL_S = 0x08, MCTRL_B = 0x10, MCTRL_H = 0x20, MCTRL_P = 0x40, MCTRL_V = 0x80;
	static const int MSTATUS_V = 0x80;

private:
	bool		running = false;

	u16		addr;
	u8		data;

	bool		vblank = true;
	bool		nmi = false;
	//bool		inc = true;

	/** Base nametable address */
	u16		baseNameTable = 0x2000;
	/** sprite pattern table address */
	u16		spPTable = 0x0000;
	/** background pattern table address */
	u16		bgPTable = 0x0000;

	static Status clock_receive(Module*, u64);
	static Status reset_receive(Module*, u64);
	static Status rw_receive(Module*, u64);
	static Status addr_receive(Module*, u64);
	static Status data_receive(Module*, u64);
	static Status pdata_receive(Module*, u64);

	static void render_thread_func(NESPPUModule*);
	std::thread*	render_thread = nullptr;
};


} // namespace nes

} // namespace vfemu


#endif /*VFEMU_NES_NESPPU_H */
