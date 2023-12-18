
/**
 * NES Control and Mmeory Mapping Module
 *
 * Ports:
 * 	clk:		pin1	# clock source input
 * 	irq:		pin1	# CPU IRQ
 * 	rw:		pin1	# Read/Write signal from CPU (low for write, high for read)
 * 	rw_ram:		pin1	# Read/Write signal to RAM (low for write, high for read)
 * 	rw_ppu:		pin1	# Read/Write signal to PPU (low for write, high for read)
 * 	addr:		pin16	# address bus
 * 	data:		pin8	# data bus
 * 	prd:		pin1	# PPU read
 * 	pwr:		pin1	# PPU write
 * 	paddr:		pin16	# PPU address
 * 	pdata:		pin8	# PPU data
 * 	cart:		nes:nesconn	# port to connect to cartridge
 */

#ifndef VFEMU_NES_NESCM_H
#define VFEMU_NES_NESCM_H

#include <vfemu/Module.h>


namespace vfemu {

namespace nes {


class NESCM01Module : public Module {
public:
	inline NESCM01Module() : Module({
		std::make_pair("clk", new Port("pin1", clock_receive)),
		std::make_pair("irq", new Port("pin1")),
		std::make_pair("rw", new Port("pin1", rw_receive)),
		std::make_pair("rw_ram", new Port("pin1")),
		std::make_pair("rw_ppu", new Port("pin1")),
		std::make_pair("addr", new Port("pin16", addr_receive)),
		std::make_pair("data", new Port("pin8", data_receive)),
		std::make_pair("prd", new Port("pin1", prd_receive)),
		std::make_pair("pwr", new Port("pin1")),
		std::make_pair("paddr", new Port("pin16", paddr_receive)),
		std::make_pair("pdata", new Port("pin8")),
		std::make_pair("cart", new Port("nes:nesconn", cart_receive)),
	}) { }

	static const int IDX_CLK = 0, IDX_IRQ = 1;
	static const int IDX_RW = 2, IDX_RW_RAM = 3, IDX_RW_PPU = 4;
	static const int IDX_ADDR = 5, IDX_DATA = 6;
	static const int IDX_PRD = 7, IDX_PWR = 8;
	static const int IDX_PADDR = 9, IDX_PDATA = 10;
	static const int IDX_CART = 11;

private:
	u16	addr;
	u8	data;

	static Status clock_receive(Module*, u64);
	static Status rw_receive(Module*, u64);
	static Status addr_receive(Module*, u64);
	static Status data_receive(Module*, u64);
	static Status cart_receive(Module*, u64);
	static Status paddr_receive(Module*, u64);
	static Status prd_receive(Module*, u64);
};



} // namespace nes

} // namespace vfemu


#endif /* VFEMU_NES_NESCM_H */
