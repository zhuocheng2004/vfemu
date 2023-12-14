
/*
 * A Type of NES Cartridge Connector
 *
 * "nes:nesconn":
 * 	port type name
 * 
 * NesConn:
 * 	connector type which connects two "nes:conn" ports.
 */

#ifndef VFEMU_NES_PIN_H
#define VFEMU_NES_PIN_H

#include <vfemu/Port.h>
#include <vfemu/Connector.h>
#include "nes.h"


namespace vfemu {

namespace nes {


class NesConnector : public Connector {
public:
	inline NesConnector(Port* dest) : Connector(dest) { };

	/**
	 * data format:
	 * A. to cartridge:
	 * 	00:	V	# unused
	 * 	01:	CLK	# clock source from CPU
	 * 	02:	R/W	# CPU read/write (high for read, low for write)
	 * 	03:	ROMSEL	# unused
	 * 	04:	PWR	# low for PPU write
	 * 	05:	PRD	# low for PPU read
	 * B. to CPU:
	 * 	00:	IRQ	# send an IRQ to CPU
	 */

	static const u64 MSK_V = 0x1, MSK_CLK = 0x2, MSK_RW = 0x4, MSK_ROMSEL = 0x8;
	static const u64 MSK_PWR = 0x10, MSK_PRD = 0x20;

	constexpr inline u64 toCartridgeData(u8 clk, u8 rw, u8 pwr, u8 prd) {
		return clk * MSK_CLK + rw * MSK_RW + pwr * MSK_PWR + prd * MSK_PRD;
	}

	constexpr inline u64 toCPUData(u8 irq) {
		return irq;
	}

	virtual Status send(u64 data);

	typedef struct {
		/** CPU addr */
		u16	addr;
		/** PPU addr */
		u16	paddr;
		/** CPU data */
		u8	data;
		/** PPU data */
		u8	pdata;
	} Data;
private:
	Port*		dest;
};

class NesConn : public ConnectorType {
public:
	inline NesConn() { }

	virtual Status connect(Port* port1, Port* port2);
	virtual Status disconnect(Port* port1, Port* port2);
};


} // namespace pin

} // namespace vfemu


#endif /* VFEMU_NES_PIN_H */
