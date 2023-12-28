
/*
 * Test NES PPU Module (depends on SDL2)
 *
 * Ports:
 * 	clk:		pin1	# clock source
 * 	rst:		pin1	# reset (low for reset state, high to run)
 * 	rw:			pin1	# CPU Read/Write (low for write, high for read)
 * 	int:		pin1	# connects to /NMI of CPU
 * 	addr:		pin16	# CPU address (only use 3 bits)
 * 	data:		pin8	# CPU data
 * 	prd:		pin1	# PPU read
 * 	pwr:		pin1	# PPU write
 * 	paddr:		pin16	# PPU address
 * 	pdata:		pin8	# PPU data
 *  exit:		pin1	# output 1 when exited
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
	inline NESPPUModule(bool vertical_mirroring = true) : Module({
		std::make_pair("clk", new Port("pin1", clock_receive)),
		std::make_pair("rst", new Port("pin1", reset_receive)),
		std::make_pair("rw", new Port("pin1", rw_receive)),
		std::make_pair("int", new Port("pin1")),
		std::make_pair("addr", new Port("pin16", addr_receive)),
		std::make_pair("data", new Port("pin8", data_receive)),
		std::make_pair("prd", new Port("pin1")),
		std::make_pair("pwr", new Port("pin1")),
		std::make_pair("paddr", new Port("pin16")),
		std::make_pair("pdata", new Port("pin8", pdata_receive)),
		std::make_pair("exit", new Port("pin1")),
	}), vertical_mirroring(vertical_mirroring) { }

	Status init();
	Status exit();

	static const int IDX_CLK = 0, IDX_RST = 1, IDX_RW = 2, IDX_INT = 3;
	static const int IDX_ADDR = 4, IDX_DATA = 5;
	static const int IDX_PRD = 6, IDX_PWR = 7, IDX_PADDR = 8, IDX_PDATA = 9;
	static const int IDX_EXIT = 10;

	static const int MCTRL_I = 0x04, MCTRL_S = 0x08, MCTRL_B = 0x10, MCTRL_H = 0x20, MCTRL_P = 0x40, MCTRL_V = 0x80;
	static const int MMSK_G = 0x01, MMSK_B = 0x08, MMSK_S = 0x10;
	static const int MSTATUS_S = 0x40, MSTATUS_V = 0x80;

private:
	bool		vertical_mirroring = true;
	bool		running = false;

	u16		addr = 0;	// addr from CPU
	u8		data = 0;	// data from CPU
	u8		pdata = 0;	// data from cartridge

	u8*		vram = nullptr;
	u8*		oam = nullptr;

	u8		universalBGColor;
	u8		bgPalettes[4][4];
	u8		spPalettes[4][4];

	bool		vblank = true;
	bool		nmi = false;
	bool		inc32 = false;
	bool		sprite_large = false;
	bool		sprite0_hit = false;

	bool		greyscale = false;
	bool		showBG = true, showSP = true;

	/** internal registers */
	u16		v = 0;
	u16		t = 0;
	u8		x = 0;
	/** 1st/2nd write toggle */
	u8		w = 0;

	u8		oamAddr = 0;

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

	inline u8 getChrData(u16 addr) {
		sendToPort(IDX_PADDR, addr);
		sendToPort(IDX_PRD, 1);
		return pdata;
	}

	u8 readVRAM(u16 addr);
	void writeVRAM(u16 addr, u8 data);

	static void fillPixels(NESPPUModule*, u32*);
	static void render_thread_func(NESPPUModule*);
	std::thread*	render_thread = nullptr;
};


} // namespace nes

} // namespace vfemu


#endif /*VFEMU_NES_NESPPU_H */
