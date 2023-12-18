
#include <cstdarg>
#include <iostream>
#include <thread>
#include <nes/nesct01.h>

namespace vfemu {

namespace nes {

using namespace std::chrono_literals;


static int _log(const char* fmt, ...) {
	return 0;
	va_list args;
	int n = 0;
	va_start(args, fmt);
	n = vprintf(fmt, args);
	va_end(args);
	return n;
}


Status NESCT01Module::clock_receive(Module* receiver, u64 data) {
	auto module = (NESCT01Module*) receiver;
	if (module->running && (data & 0x1))
		module->action();
	return Status::SUCCESS;
}

Status NESCT01Module::reset_receive(Module* receiver, u64 data) {
	auto module = (NESCT01Module*) receiver;
	if (data & 0x1)
		module->reset();
	return Status::SUCCESS;
}

Status NESCT01Module::nmi_receive(Module* receiver, u64 data) {
	return Status::SUCCESS;
}

Status NESCT01Module::data_receive(Module* receiver, u64 data) {
	auto module = (NESCT01Module*) receiver;
	module->data = data & 0xff;
	return Status::SUCCESS;
}


u8 NESCT01Module::adjustZN(u8 v) {
	if (v == 0)
		p |= MSK_ZERO;
	else
		p &= ~MSK_ZERO;
	
	if (v & 0x80)
		p |= MSK_NEG;
	else
		p &= ~MSK_NEG;

	return v;
}

u8 NESCT01Module::add(u8 a1, u8 a2) {
	if (((u16) a1 + a2) & 0xff00)
		p |= MSK_CARRY;
	else
		p &= ~MSK_CARRY;

	return adjustZN(a1 + a2);
}

u8 NESCT01Module::sub(u8 a1, u8 a2) {
	if (((short) a1 - a2) < 0)
		p |= MSK_CARRY;
	else
		p &= ~MSK_CARRY;
	
	return adjustZN(a1 - a2);
}

u16 NESCT01Module::branch(u8 offset) {
	u8 pcl = pc & 0xff;
	u16 pch = pc & 0xff00;
	pc = pch + ((pcl + offset) & 0xff);
	return pc;
}

void NESCT01Module::reset() {
	running = false;
	// 0xfffc-0xfffd contains the entry point of reset routine
	u8 resetPosLow = loadData(0xfffc), resetPosHigh = loadData(0xfffd);
	pc = (resetPosHigh << 8) + resetPosLow;
	a = x = y = 0;
	sp = 0xfd;
	data = 0;
	p = MSK_B + MSK_INT + MSK_ZERO;
	running = true;
}

void NESCT01Module::action() {
	u16 pc_saved = pc;

	static int bcnt = -1;
	if (pc_saved == 0x0)
		bcnt = 0;

	if (bcnt >= 0) {
		char ch = getchar();
		if (ch == 'c')
			bcnt = -1;
	}

	u8 instr = loadData(pc++);
	_log("    <%04X>  %02X", pc_saved, instr);


	switch (instr & 0x3) {
		case 0:
			action_control(instr);
			break;
		case 1:
			action_alu(instr);
			break;
		case 2:
			action_mv(instr);
			break;
		case 3:
			action_other(instr);
			break;
	}
	_log("\n");
}


void NESCT01Module::action_control(u8 instr) {
	u8 v = 0;	// instant value
	u8 zaddr = 0;	// zero page addr
	u8 addrl = 0, addrh = 0;
	u16 addr;	// absolute addr
	switch (instr & 0x1C) {
		case 0x00:
			switch (instr & 0xE0) {
				case 0x20:	// JSR a
					addrl = loadData(pc++);
					addrh = loadData(pc++);
					addr = addrl + (addrh << 8);
					_log(" %02X", addrl);
					_log(" %02X", addrh);
					pushStack(pc >> 8);
					pushStack(pc & 0xff);
					pc = addr;
					_log("\t\tJSR  [%04X]\t\t(S=%02X)", pc, sp);
					break;
				case 0x60:	// RTS
					addrl = popStack();
					addrh = popStack();
					pc = ((u16) addrh << 8) + addrl;
					_log("\t\t\tRTS\t\t\t(pc=%04X, S=%02X)", pc, sp);
					break;
				case 0xA0:	// LDY #i
					v = loadData(pc++);
					_log(" %02X", v);
					adjustZN(y = v);
					_log("\t\tLDY  %02X", v);
					break;
				case 0xC0:	// CPY #i
					v = loadData(pc++);
					_log(" %02X", v);
					sub(y, v);
					p ^= MSK_CARRY;
					_log("\t\tCPY  %02X\t\t\t(Y=%02X, flag=%02X)", v, y, p);
					break;
				case 0xE0:	// CPX #i
					v = loadData(pc++);
					_log(" %02X", v);
					sub(x, v);
					p ^= MSK_CARRY;
					_log("\t\tCPX  %02X\t\t\t(X=%02X, flag=%02X)", v, x, p);
					break;
			}
			break;
		case 0x08:
			switch (instr & 0xE0) {
				case 0x80:	// DEY
					adjustZN(--y);
					_log("\t\t\tDEY\t\t\t(Y=%02X)", y);
					break;
				case 0xC0:	// INY
					adjustZN(++y);
					_log("\t\t\tINY\t\t\t(Y=%02X)", y);
					break;
				case 0xE0:	// INX
					adjustZN(++x);
					_log("\t\t\tINX\t\t\t(X=%02X)", x);
					break;
			}
			break;
		case 0x0C:	// a
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0x20:	// BIT	a
					v = loadData(addr);
					if ((a & v) == 0)
						p |= MSK_ZERO;
					else
						p &= ~MSK_ZERO;
					p &= ~(MSK_OVERFLOW | MSK_NEG);
					p |= (v & 0xC0);
					_log("\t\tBIT  [%04X]\t\t(flag=%02X, [%04X]=%02X)", addr, p, addr, v);
					break;
				case 0x40:	// JMP a
					pc = addr;
					_log("\t\tJMP  [%04X]", addr);
					break;
			}
			break;
		case 0x10:	// rel
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x00:	// BPL
					if (!(p & MSK_NEG))
						branch(zaddr);
					_log("\t\tBPL  +%02X\t\t(flag=%02X)", zaddr, p);
					break;
				case 0xA0:	// BCS
					if (p & MSK_CARRY)
						branch(zaddr);
					_log("\t\tBCS  +%02X\t\t(flag=%02X)", zaddr, p);
					break;
				case 0xC0:	// BNE
					if (!(p & MSK_ZERO))
						branch(zaddr);
					_log("\t\tBNE  +%02X", zaddr);
					break;
				case 0xE0:	// BEQ
					if (p & MSK_ZERO)
						branch(zaddr);
					_log("\t\tBEQ  +%02X", zaddr);
			}
			break;
		case 0x18:
			switch (instr & 0xE0) {
				case 0x60:	// SEI
					p |= MSK_INT;
					_log("\t\t\tSEI");
					break;
				case 0xC0:	// CLD
					p &= ~MSK_DEC;
					_log("\t\t\tCLD");
					break;
			}
			break;
	}
}

void NESCT01Module::action_alu(u8 instr) {
	u8 v = 0;	// instant value
	u8 zaddr = 0;	// zero page addr
	u8 addrl = 0, addrh = 0;
	u16 addr;	// absolute addr
	switch (instr & 0x1C) {
		case 0x04:	// z
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x20:	// AND z
					adjustZN(a &= loadData(zaddr));
					_log("\t\tAND  [%02X]\t\t(A=%02X)", zaddr, a);
					break;
				case 0x80:	// STA z
					storeData(zaddr, a);
					_log("\t\tSTA  [%02X]\t\t(A=%02X)", zaddr, a);
					break;
				case 0xA0:	// LDA z
					adjustZN(a = loadData(zaddr));
					_log("\t\tLDA  [%02X\t\t(A=%02X)]", zaddr, a);
					break;
			}
			break;
		case 0x08:	// #i
			v = loadData(pc++);
			_log(" %02X", v);
			switch (instr & 0xE0) {
				case 0x00:	// ORA #i
					adjustZN(a |= v);
					_log("\t\tORA  %02X\t\t\t(A=%02X)", v, a);
					break;
				case 0x20:	// AND #i
					adjustZN(a &= v);
					_log("\t\tAND  %02X\t\t\t(A=%02X)", v, a);
					break;
				case 0xA0:	// LDA #i
					adjustZN(a = v);
					_log("\t\tLDA  %02X", a);
					break;
				case 0xC0:	// CMP #i
					sub(a, v);
					p ^= MSK_CARRY;
					_log("\t\tCMP  %02X\t\t(flag=%02X)", v, p);
					break;
				case 0xE0:	// SBC #i
					adjustZN(a = sub(a, v));
					_log("\t\tSBC  %02X\t\t(A=%02X)", v, a);
					break;
			}
			break;
		case 0x0C:	// a
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0x80:	// STA a
					storeData(addr, a);
					_log("\t\tSTA  [%04X]\t\t(A=%02X)", addr, a);
					break;
				case 0xA0:	// LDA a
					adjustZN(a = loadData(addr));
					_log("\t\tLDA  [%04X]\t\t(A=%02X)", addr, a);
					break;
			}
			break;
		case 0x10:	// (d),y
			v = loadData(pc++);
			_log(" %02X", v);
			addrl = loadData(v);
			addrh = loadData(v + 1);
			addr = addrl + (addrh << 8);
			switch (instr & 0xE0) {
				case 0x80:	// STA (d),y
					storeData(addr + y, a);
					_log("\t\tSTA  [[%02X]+y]\t\t(A=%02X, Y=%02X, [%02X]=%04X)", v, a, y, v, addr);
					break;
			}
			break;
		case 0x18:	// a,y
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0x80) {
				case 0x80:	// STA a,y
					storeData(addr + y, a);
					_log("\t\tSTA  [%04X+y]\t\t(A=%02X, Y=%02X)", addr, a, y);
					break;
			}
			break;
		case 0x1C:	// a,x
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0xA0:	// LDA a,x
					adjustZN(a = loadData(addr + x));
					_log("\t\tLDA  [%04X+x]\t\t(A=%02X, X=%02X)", addr, a, x);
					break;
			}
			break;
	}
}

void NESCT01Module::action_mv(u8 instr) {
	u8 v = 0;	// instant value
	u8 zaddr = 0;	// zero page addr
	u8 addrl = 0, addrh = 0;
	u16 addr;	// absolute addr
	switch (instr & 0x1C) {
		case 0x00:	// #i
			v = loadData(pc++);
			_log(" %02X", v);
			switch (instr & 0xE0) {
				case 0xA0:	// LDX #i
					adjustZN(x = v);
					_log("\t\tLDX  %02X", x);
					break;
			}
			break;
		case 0x04:	// z
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x60:	// ROR z
					v = loadData(zaddr);
					adjustZN(v = (v >> 1) + (p & 0x1) * 0x80);
					storeData(zaddr, v);
					_log("\t\tROR  [%02X]", zaddr);
					break;
				case 0x80:	// STX z
					storeData(zaddr, x);
					_log("\t\tSTX  [%02X]\t\t(X=%02X)", zaddr, x);
					break;
				case 0xA0:	// LDX z
					adjustZN(x = loadData(zaddr));
					_log("\t\tLDX  [%02X]\t\t(X=%02X)", zaddr, x);
					break;
			}
			break;
		case 0x08:
			switch (instr & 0xE0) {
				case 0x80:	// TXA
					adjustZN(a = x);
					_log("\t\t\tTXA\t\t\t(A=%02X)", a);
					break;
				case 0xC0:	// DEX
					adjustZN(--x);
					_log("\t\t\tDEX\t\t\t(X=%02X)", x);
					break;
				case 0xE0:	// NOP
					_log("\t\t\tNOP");
					break;
			}
			break;
		case 0x0C:	// a
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0x80:	// STX a
					storeData(addr, x);
					_log("\t\tSTX  [%04X]\t\t(X=%02X)", addr, x);
					break;
				case 0xE0:	// INC a
					v = loadData(addr);
					adjustZN(++v);
					storeData(addr, v);
					_log("\t\tINC  [%04X]\t\t([%04X]=%02X)", addr, addr, v);
					break;
			}
			break;
		case 0x18:
			switch (instr & 0xE0) {
				case 0x80:	// TXS
					sp = x;
					_log("\t\t\tTXS\t\t\t(S=%02X)", sp);
					break;
			}
			break;
	}
}

void NESCT01Module::action_other(u8 instr) {
	// not implemented yet
}


} // namespace nes

} // namespace vfemu

