
#include <chrono>
#include <cstdarg>
#include <iostream>
#include <thread>
#include <nes/nesct01.h>

namespace vfemu {

namespace nes {

using namespace std::chrono_literals;


static bool dump = false;

static auto t1 = std::chrono::steady_clock::now();
static auto t2 = std::chrono::steady_clock::now();

static int _log(const char* fmt, ...) {
	if (!dump)
		return 0;
	va_list args;
	int n = 0;
	va_start(args, fmt);
	n = vprintf(fmt, args);
	va_end(args);
	return n;
}


Status NESCT01Module::clock_receive(Module* receiver, u64 signal) {
	auto module = (NESCT01Module*) receiver;
	if (module->running && (signal & 0x1)) {
		module->lock.lock();
		module->action();
		module->lock.unlock();
	}
	return Status::SUCCESS;
}

Status NESCT01Module::reset_receive(Module* receiver, u64 signal) {
	auto module = (NESCT01Module*) receiver;
	if (signal & 0x1) {
		module->lock.lock();
		module->reset();
		module->lock.unlock();
	}
	return Status::SUCCESS;
}

Status NESCT01Module::nmi_receive(Module* receiver, u64 signal) {
	auto module = (NESCT01Module*) receiver;
	if (signal) {
		//module->lock.lock();
		//module->branch_irq(0xFFFA);
		//module->lock.unlock();
		if (module->in_nmi < 1 && module->lock.try_lock()) {
			t1 = std::chrono::steady_clock::now();
			module->branch_irq(0xFFFA);
			module->lock.unlock();
		}
	}
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

u8 NESCT01Module::add(u8 a1, u8 a2, u8 carry = 0) {
	if (((u16) a1 + a2 + carry) & 0xff00)
		p |= MSK_CARRY;
	else
		p &= ~MSK_CARRY;

	return adjustZN(a1 + a2 + carry);
}

u8 NESCT01Module::sub(u8 a1, u8 a2) {
	if (((short) a1 - a2) < 0)
		p |= MSK_CARRY;
	else
		p &= ~MSK_CARRY;
	
	return adjustZN(a1 - a2);
}

u16 NESCT01Module::branch(u8 offset) {
	pc += offset;
	if (offset & 0x80) {	// negative
		pc -= 0x100;
	}
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
	puts("  ==== CPU RESET ====  ");
	running = true;
}


static bool start_break = false;
static int bcnt = -1;
static u16 advance_addr = 0x0000;

void NESCT01Module::action() {
	u16 pc_saved = pc;

	if (advance_addr && pc == advance_addr) {
		bcnt = 0; //dump = true;
		advance_addr = 0x0000;
	}

	if (bcnt >= 0) {
		putchar('>');
		char ch = getchar();
		bcnt++;

		if (ch == 'c') {
			bcnt = -1; //dump = false;
		} else if (ch == 'r') {
			puts("  ==== registers ====  ");
			printf("  A=%02X, X=%02X, Y=%02X \n", a, x, y);
			printf("  PC=%04X, SP=%04X \n", pc, sp);
			printf("  P=%02X \n", p);
			puts("  ========  \n");
			getchar();
			return;
		} else if (ch == 'a') {
			scanf("%hx", &advance_addr);
			getchar();
			bcnt = -1; //dump = false;
			printf("Advancing to %04X \n", advance_addr);
		} else if (ch == 'x') {
			u16 addr;
			scanf("%hx", &addr);
			getchar();
			printf("  [%04X] = %02X \n", addr, loadData(addr));
			return;
		}
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

void NESCT01Module::branch_irq(u16 vector_addr) {
	if (in_nmi > 0)
		puts("WARNING: nested IRQ handling");
	in_nmi++;
	pushStack(pc >> 8);
	pushStack(pc & 0xff);
	pushStack(p);
	u8 addrl = loadData(vector_addr);
	p |= MSK_INT;
	u8 addrh = loadData(vector_addr + 1);
	u16 addr = addrl + (addrh << 8);
	pc = addr;
	printf("  ==== INT @[%04X]=%04X ====  \n", vector_addr, addr);
	
	// only break the first time
	if (start_break) {
		bcnt = 0; //dump = true;
		start_break = false;
	}
			
}


void NESCT01Module::action_control(u8 instr) {
	u8 v = 0;	// instant value
	u8 zaddr = 0;	// zero page addr
	u8 addrl = 0, addrh = 0;
	u16 addr, v3;	// absolute addr
	switch (instr & 0x1C) {
		case 0x00:
			switch (instr & 0xE0) {
				case 0x20:	// JSR a
					addrl = loadData(pc++);
					addrh = loadData(pc++);
					addr = addrl + (addrh << 8);
					_log(" %02X", addrl);
					_log(" %02X", addrh);
					v3 = pc - 1;
					pushStack(v3 >> 8);
					pushStack(v3 & 0xff);
					pc = addr;
					_log("\t\tJSR  [%04X]\t\t(S=%02X)", pc, sp);
					break;
				case 0x40:	// RTI
					p = popStack();
					addrl = popStack();
					addrh = popStack();
					pc = ((u16) addrh << 8) + addrl;
					in_nmi--;
					_log("\t\t\tRTI\t\t\t(pc=%04X, S=%02X)", pc, sp);
					std::cout << "TIME: " << (std::chrono::steady_clock::now() - t1).count() << std::endl;
					break;
				case 0x60:	// RTS
					addrl = popStack();
					addrh = popStack();
					pc = ((u16) addrh << 8) + addrl + 1;
					_log("\t\t\tRTS\t\t\t(pc=%04X, S=%02X)", pc, sp);
					break;
				case 0xA0:	// LDY #i
					v = loadData(pc++);
					_log(" %02X", v);
					adjustZN(y = v);
					_log("\t\tLDY  %02X\t\t\t(Y=%02X)", v, y);
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
				default:
					bcnt = 0;
			}
			break;
		case 0x04:	// z
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x20:	// BIT	z
					v = loadData(zaddr);
					if ((a & v) == 0)
						p |= MSK_ZERO;
					else
						p &= ~MSK_ZERO;
					p &= ~(MSK_OVERFLOW | MSK_NEG);
					p |= (v & 0xC0);
					_log("\t\tBIT  [%02X]\t\t(flag=%02X, [%04X]=%02X)", zaddr, p, zaddr, v);
					break;
				case 0x80:	// STY z
					storeData(zaddr, y);
					_log("\t\tSTY  [%02X]\t\t(Y=%02X)", zaddr, y);
					break;
				case 0xA0:	// LDY z
					adjustZN(y = loadData(zaddr));
					_log("\t\tLDY  [%02X]\t\t(Y=%02X)", zaddr, y);
					break;
				case 0xC0:	// CPY z
					v = loadData(zaddr);
					sub(y, v);
					p ^= MSK_CARRY;
					_log("\t\tCPY  [%02X]\t\t(Y=%02X, [%02X]=%02X, flag=%02X)", zaddr, y, zaddr, loadData(zaddr), p);
					break;
				case 0xE0:	// CPX z
					v = loadData(zaddr);
					sub(x, v);
					p ^= MSK_CARRY;
					_log("\t\tCPX  [%02X]\t\t(X=%02X, [%02X]=%02X, flag=%02X)", zaddr, x, zaddr, loadData(zaddr), p);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x08:
			switch (instr & 0xE0) {
				case 0x00:	// PHP
					pushStack(p);
					_log("\t\t\tPHP\t\t\t(flag=%02X)", p);
					break;
				case 0x20:	// PLP
					p = popStack();
					_log("\t\t\tPLP\t\t\t(flag=%02X)", p);
					break;
				case 0x40:	// PHA
					pushStack(a);
					_log("\t\t\tPHA\t\t\t(A=%02X)", a);
					break;
				case 0x60:	// PLA
					adjustZN(a = popStack());
					_log("\t\t\tPLA\t\t\t(A=%02X)", a);
					break;
				case 0x80:	// DEY
					adjustZN(--y);
					_log("\t\t\tDEY\t\t\t(Y=%02X)", y);
					break;
				case 0xA0:	// TAY
					adjustZN(y = a);
					_log("\t\t\tTAY\t\t\t(Y=%02X)", y);
					break;
				case 0xC0:	// INY
					adjustZN(++y);
					_log("\t\t\tINY\t\t\t(Y=%02X)", y);
					break;
				case 0xE0:	// INX
					adjustZN(++x);
					_log("\t\t\tINX\t\t\t(X=%02X)", x);
					break;
				default:
					bcnt = 0;
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
					_log("\t\tJMP  %04X", addr);
					break;
				case 0x60:	// JMP [a]
					addrl = loadData(addr);
					addrh = loadData(addr + 1);
					pc = addrl + (addrh << 8);
					_log("\t\tJMP  [%04X]\t\t([%04X]=%04X)", addr, addr, pc);
					break;
				case 0x80:	// STY a
					storeData(addr, y);
					_log("\t\tSTY  [%04X]\t\t(Y=%02X)", addr, y);
					break;
				case 0xA0:	// LDY a
					adjustZN(y = loadData(addr));
					_log("\t\tLDY  [%04X]\t\t(Y=%02X)", addr, y);
					break;
				default:
					bcnt = 0;
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
				case 0x20:	// BMI
					if (p & MSK_NEG)
						branch(zaddr);
					_log("\t\tBMI  +%02X\t\t(flag=%02X)", zaddr, p);
					break;
				case 0x80:	// BCC
					if (!(p & MSK_CARRY))
						branch(zaddr);
					_log("\t\tBCC  +%02X\t\t(flag=%02X)", zaddr, p);
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
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x14:	// d,x
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0xA0:	// LDY d,x
					adjustZN(y = loadData(zaddr + x));
					_log("\t\tLDY  [%02X+x]\t\t(Y=%02X, X=%02X)", zaddr, y, x);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x18:
			switch (instr & 0xE0) {
				case 0x00:	// CLC
					p &= ~MSK_CARRY;
					_log("\t\t\tCLC\t\t\t(flag=%02X)", p);
					break;
				case 0x20:	// SEC
					p |= MSK_CARRY;
					_log("\t\t\tSEC\t\t\t(flag=%02X)", p);
					break;
				case 0x40:	// CLI
					p &= ~MSK_INT;
					_log("\t\t\tCLI\t\t\t(flag=%02X)", p);
					break;
				case 0x60:	// SEI
					p |= MSK_INT;
					_log("\t\t\tSEI\t\t\t(flag=%02X)", p);
					break;
				case 0x80:	// TYA
					adjustZN(a = y);
					_log("\t\t\tTYA\t\t\t(A=%02X)", a);
					break;
				case 0xC0:	// CLD
					p &= ~MSK_DEC;
					_log("\t\t\tCLD\t\t\t(flag=%02X)", p);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x1C:	// a,x
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0xA0:	// LDY a,x
					adjustZN(y = loadData(addr + x));
					_log("\t\tLDY  [%04X+x]\t\t(Y=%02X, X=%02X)", addr, y, x);
					break;
				default:
					bcnt = 0;
			}
			break;
		default:
					bcnt = 0;
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
				case 0x00:	// ORA z
					adjustZN(a |= loadData(zaddr));
					_log("\t\tORA  [%02X]\t\t(A=%02X)", zaddr, a);
					break;
				case 0x20:	// AND z
					adjustZN(a &= loadData(zaddr));
					_log("\t\tAND  [%02X]\t\t(A=%02X)", zaddr, a);
					break;
				case 0x40:	// EOR z
					adjustZN(a ^= loadData(zaddr));
					_log("\t\tEOR  [%02X]\t\t(A=%02X)", zaddr, a);
					break;
				case 0x60:	// ADC z
					adjustZN(a = add(a, loadData(zaddr), p & MSK_CARRY));
					_log("\t\tADC  [%02X]\t\t(A=%02X, [%02X]=%02X)", zaddr, a, zaddr, loadData(zaddr));
					break;
				case 0x80:	// STA z
					storeData(zaddr, a);
					_log("\t\tSTA  [%02X]\t\t(A=%02X)", zaddr, a);
					break;
				case 0xA0:	// LDA z
					adjustZN(a = loadData(zaddr));
					_log("\t\tLDA  [%02X]\t\t(A=%02X)", zaddr, a);
					break;
				case 0xC0:	// CMP z
					sub(a, loadData(zaddr));
					p ^= MSK_CARRY;
					_log("\t\tCMP  [%02X]\t\t([%02X]=%02X, flag=%02X)", zaddr, zaddr, loadData(zaddr), p);
					break;
				case 0xE0:	// SBC z
					adjustZN(a = sub(a, loadData(zaddr)));
					_log("\t\tSBC  [%02X]\t\t(A=%02X, [%02X]=%02X)", zaddr, a, zaddr, loadData(zaddr));
					break;
				default:
					bcnt = 0;
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
				case 0x40:	// EOR #i
					adjustZN(a ^= v);
					_log("\t\tEOR  %02X\t\t\t(A=%02X)", v, a);
					break;
				case 0x60:	// ADC #i
					adjustZN(a = add(a, v, p & MSK_CARRY));
					_log("\t\tADC  %02X\t\t\t(A=%02X, flag=%02X)", v, a, p);
					break;
				case 0xA0:	// LDA #i
					adjustZN(a = v);
					_log("\t\tLDA  %02X\t\t\t(A=%02X)", v, a);
					break;
				case 0xC0:	// CMP #i
					sub(a, v);
					p ^= MSK_CARRY;
					_log("\t\tCMP  %02X\t\t\t(flag=%02X)", v, p);
					break;
				case 0xE0:	// SBC #i
					adjustZN(a = sub(a, v));
					_log("\t\tSBC  %02X\t\t\t(A=%02X)", v, a);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x0C:	// a
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0x60:	// ADC a
					v = loadData(addr);
					adjustZN(a = add(a, v, p & MSK_CARRY));
					_log("\t\tADC  [%04X]\t\t(A=%02X)", addr, a);
					break;
				case 0x80:	// STA a
					storeData(addr, a);
					_log("\t\tSTA  [%04X]\t\t(A=%02X)", addr, a);
					break;
				case 0xA0:	// LDA a
					adjustZN(a = loadData(addr));
					_log("\t\tLDA  [%04X]\t\t(A=%02X)", addr, a);
					break;
				case 0xC0:	// CMP a
					sub(a, loadData(addr));
					p ^= MSK_CARRY;
					_log("\t\tCMP  [%04X]\t\t(flag=%02X, [%04X]=%02X)", v, p, addr, loadData(addr));
					break;
				default:
					bcnt = 0;
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
				case 0xA0:	// LDA (d),y
					adjustZN(a = loadData(addr + y));
					_log("\t\tLDA  [[%02X]+y]\t\t(A=%02X, Y=%02X, [%02X]=%04X)", v, a, y, v, addr);
					break;
				case 0xC0:	// CMP (d),y
					sub(a, loadData(addr + y));
					p ^= MSK_CARRY;
					_log("\t\tCMP  [[%02X]+y]\t\t(A=%02X, Y=%02X, [%02X]=%04X, flag=%02X)", v, a, y, v, addr, p);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x14:	// d,x
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x80:	// STA d,x
					storeData(zaddr + x, a);
					_log("\t\tSTA  [%02X+x]\t\t(A=%02X, X=%02X)", zaddr, a, x);
					break;
				case 0xA0:	// LDA d,x
					adjustZN(a = loadData(zaddr + x));
					_log("\t\tLDA  [%02X+x]\t\t(A=%02X, X=%02X)", zaddr, a, x);
					break;
				case 0xC0:	// CMP d,x
					sub(a, loadData(zaddr + x));
					p ^= MSK_CARRY;
					_log("\t\tCMP  [%02X+x]\t\t(A=%02X, X=%02X, flag=%02X)", zaddr, a, x, p);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x18:	// a,y
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0x60:	// ADC a,y
					v = loadData(addr + y);
					adjustZN(a = add(a, v, p & MSK_CARRY));
					_log("\t\tADC  [%04X+y]\t\t(A=%02X, Y=%02X)", addr, a, y);
					break;
				case 0x80:	// STA a,y
					storeData(addr + y, a);
					_log("\t\tSTA  [%04X+y]\t\t(A=%02X, Y=%02X)", addr, a, y);
					break;
				case 0xA0:	// LDA a,y
					adjustZN(a = loadData(addr + y));
					_log("\t\tLDA  [%04X+y]\t\t(A=%02X, Y=%02X)", addr, a, y);
					break;
				case 0xC0:	// CMP a,y
					sub(a, loadData(addr + y));
					p ^= MSK_CARRY;
					_log("\t\tCMP  [%04X+y]\t\t(flag=%02X, Y=%02X, [%04X+y]=%02X)", addr, p, y, addr, loadData(addr + y));
					break;
				case 0xE0:	// SBC a,y
					adjustZN(a = sub(a, loadData(addr + y)));
					_log("\t\tSBC  [%04X+y]\t\t(A=%02X, Y=%02X)", addr, a, y);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x1C:	// a,x
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0x00:	// ORA a,x
					adjustZN(a |= loadData(addr + x));
					_log("\t\tORA  [%04X+x]\t\t(A=%02X, X=%02X)", addr, a, x);
					break;
				case 0x20:	// AND a,x
					adjustZN(a &= loadData(addr + x));
					_log("\t\tAND  [%04X+x]\t\t(A=%02X, X=%02X)", addr, a, x);
					break;
				case 0x40:	// EOR a,x
					adjustZN(a ^= loadData(addr + x));
					_log("\t\tEOR  [%04X+x]\t\t(A=%02X, X=%02X)", addr, a, x);
					break;
				case 0x80:	// STA a,x
					storeData(addr + x, a);
					_log("\t\tSTA  [%04X+x]\t\t(A=%02X, X=%02X)", addr, a, x);
					break;
				case 0xA0:	// LDA a,x
					adjustZN(a = loadData(addr + x));
					_log("\t\tLDA  [%04X+x]\t\t(A=%02X, X=%02X)", addr, a, x);
					break;
				case 0xC0:	// CMP a,x
					sub(a, loadData(addr + x));
					p ^= MSK_CARRY;
					_log("\t\tCMP  [%04X+x]\t\t(flag=%02X, X=%02X, [%04X+x]=%02X)", addr, p, x, addr, loadData(addr + x));
					break;
				default:
					bcnt = 0;
			}
			break;
		default:
					bcnt = 0;
	}
}

void NESCT01Module::action_mv(u8 instr) {
	u8 v = 0, v2 = 0;	// instant value
	u8 zaddr = 0;		// zero page addr
	u8 addrl = 0, addrh = 0;
	u16 addr;			// absolute addr
	switch (instr & 0x1C) {
		case 0x00:	// #i
			v = loadData(pc++);
			_log(" %02X", v);
			switch (instr & 0xE0) {
				case 0xA0:	// LDX #i
					adjustZN(x = v);
					_log("\t\tLDX  %02X", x);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x04:	// z
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x00:	// ASL
					v2 = loadData(zaddr);
					adjustZN(v = (v2 << 1));
					storeData(zaddr, v);
					if (v2 & 0x80)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\tASL  [%02X]\t\t([%02X]=%02X, flag=%02X)", zaddr, zaddr, loadData(zaddr), p);
					break;
				case 0x20:	// ROL z
					v2 = loadData(zaddr);
					adjustZN(v = (v2 << 1) + (p & MSK_CARRY));
					storeData(zaddr, v);
					if (v2 & 0x80)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\tROL  [%02X]\t\t([%02X]=%02X, flag=%02X)", zaddr, zaddr, loadData(zaddr), p);
					break;
				case 0x40:	// LSR z
					v = loadData(zaddr);
					if (v & 0x1)
						p | MSK_CARRY;
					else
						p & ~MSK_CARRY;
					v >>= 1;
					storeData(zaddr, v);
					_log("\t\tLSR  [%02X]\t\t([%02X]=%02X, flag=%02X)", zaddr, zaddr, loadData(zaddr), p);
					break;
				case 0x60:	// ROR z
					v2 = loadData(zaddr);
					adjustZN(v = (v2 >> 1) + (p & MSK_CARRY) * 0x80);
					storeData(zaddr, v);
					if (v2 & 0x1)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\tROR  [%02X]\t\t([%02X]=%02X, flag=%02X)", zaddr, loadData(zaddr), p);
					break;
				case 0x80:	// STX z
					storeData(zaddr, x);
					_log("\t\tSTX  [%02X]\t\t(X=%02X)", zaddr, x);
					break;
				case 0xA0:	// LDX z
					adjustZN(x = loadData(zaddr));
					_log("\t\tLDX  [%02X]\t\t(X=%02X)", zaddr, x);
					break;
				case 0xC0:	// DEC z
					v = loadData(zaddr);
					adjustZN(--v);
					storeData(zaddr, v);
					_log("\t\tDEC  [%02X]\t\t([%02X]=%02X)", zaddr, zaddr, loadData(zaddr));
					break;
				case 0xE0:	// INC z
					v = loadData(zaddr);
					adjustZN(++v);
					storeData(zaddr, v);
					_log("\t\tINC  [%02X]\t\t([%02X]=%02X)", zaddr, zaddr, loadData(zaddr));
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x08:
			switch (instr & 0xE0) {
				case 0x00:	// ASL
					v = a;
					adjustZN(a = (v << 1));
					if (v & 0x80)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\t\tASL\t\t\t(A=%02X, flag=%02X)", a, p);
					break;
				case 0x20:	// ROL
					v = a;
					adjustZN(a = (v << 1) + (p & MSK_CARRY));
					if (v & 0x80)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\t\tROL\t\t\t(A=%02X, flag=%02X)", a, p);
					break;
				case 0x40:	// LSR
					if (a & 0x1)
						p | MSK_CARRY;
					else
						p & ~MSK_CARRY;
					a >>= 1;
					_log("\t\t\tLSR\t\t\t(A=%02X, flag=%02X)", a, p);
					break;
				case 0x60:	// ROR
					v = a;
					adjustZN(a = (v >> 1) + (p & MSK_CARRY) * 0x80);
					if (v & 0x1)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\t\tROR\t\t\t(A=%02X, flag=%02X)", a, p);
					break;
				case 0x80:	// TXA
					adjustZN(a = x);
					_log("\t\t\tTXA\t\t\t(A=%02X)", a);
					break;
				case 0xA0:	// TAX
					adjustZN(x = a);
					_log("\t\t\tTAX\t\t\t(X=%02X)", x);
					break;
				case 0xC0:	// DEX
					adjustZN(--x);
					_log("\t\t\tDEX\t\t\t(X=%02X)", x);
					break;
				case 0xE0:	// NOP
					_log("\t\t\tNOP");
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x0C:	// a
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0x20:	// ROL a
					v2 = loadData(addr);
					adjustZN(v = (v2 << 1) + (p & MSK_CARRY));
					storeData(addr, v);
					if (v2 & 0x80)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\tROL  [%04X]\t\t([%04X]=%02X, flag=%02X)", addr, addr, loadData(addr), p);
					break;
				case 0x40:	// LSR a
					v = loadData(addr);
					if (v & 0x1)
						p | MSK_CARRY;
					else
						p & ~MSK_CARRY;
					v >>= 1;
					storeData(addr, v);
					_log("\t\tLSR  [%04X]\t\t([%04X]=%02X, flag=%02X)", addr, addr, loadData(addr), p);
					break;
				case 0x60:	// ROR a
					v2 = loadData(addr);
					adjustZN(v = (v2 >> 1) + (p & MSK_CARRY) * 0x80);
					storeData(addr, v);
					if (v2 & 0x1)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\tROR  [%04X]\t\t([%04X]=%02X, flag=%02X)", addr, loadData(addr), p);
					break;
				case 0x80:	// STX a
					storeData(addr, x);
					_log("\t\tSTX  [%04X]\t\t(X=%02X)", addr, x);
					break;
				case 0xA0:	// LDX a
					adjustZN(x = loadData(addr));
					_log("\t\tLDX  [%04X]\t\t(X=%02X)", addr, x);
					break;
				case 0xC0:	// DEC a
					v = loadData(addr);
					adjustZN(--v);
					storeData(addr, v);
					_log("\t\tDEC  [%04X]\t\t([%04X]=%02X)", addr, addr, v);
					break;
				case 0xE0:	// INC a
					v = loadData(addr);
					adjustZN(++v);
					storeData(addr, v);
					_log("\t\tINC  [%04X]\t\t([%04X]=%02X)", addr, addr, v);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x14:	// d,x
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0xC0:	// DEC d,x
					v = loadData(zaddr + x);
					adjustZN(--v);
					storeData(zaddr + x, v);
					_log("\t\tDEC  [%02X+x]\t\t([%02X+x]=%02X, X=%02X)", zaddr, zaddr, v, x);
					break;
				case 0xE0:	// INC d,x
					v = loadData(zaddr + x);
					adjustZN(++v);
					storeData(zaddr + x, v);
					_log("\t\tINC  [%02X+x]\t\t([%02X+x]=%02X, X=%02X)", zaddr, zaddr, v, x);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x18:
			switch (instr & 0xE0) {
				case 0x80:	// TXS
					sp = x;
					_log("\t\t\tTXS\t\t\t(S=%02X)", sp);
					break;
				case 0xA0:	// TSX
					adjustZN(x = sp);
					_log("\t\t\tTSX\t\t\t(X=%02X)", x);
					break;
				default:
					bcnt = 0;
			}
			break;
		case 0x1C:	// a,[x|y]
			addrl = loadData(pc++);
			addrh = loadData(pc++);
			addr = addrl + (addrh << 8);
			_log(" %02X", addrl);
			_log(" %02X", addrh);
			switch (instr & 0xE0) {
				case 0x20:	// ROL a,x
					v2 = loadData(addr + x);
					adjustZN(v = (v2 << 1) + (p & MSK_CARRY));
					storeData(addr + x, v);
					if (v2 & 0x80)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\tROR  [%04X+x]\t\t([%04X+x]=%02X, X=%02X, flag=%02X)", addr, addr, loadData(addr + x), x, p);
					break;
				case 0x60:	// ROR a,x
					v2 = loadData(addr + x);
					adjustZN(v = (v2 >> 1) + (p & MSK_CARRY) * 0x80);
					storeData(addr + x, v);
					if (v2 & 0x1)
						p |= MSK_CARRY;
					else
						p &= ~MSK_CARRY;
					_log("\t\tROR  [%04X+x]\t\t([%04X+x]=%02X, X=%02X, flag=%02X)", addr, addr, loadData(addr + x), x, p);
					break;
				case 0xA0:	// LDX a,y
					adjustZN(x = loadData(addr + y));
					_log("\t\tLDX  [%04X+y]\t\t(X=%02X, Y=%02X)", addr, x, y);
					break;
				case 0xC0:	// DEC a,x
					v = loadData(addr + x);
					adjustZN(--v);
					storeData(addr + x, v);
					_log("\t\tINC  [%04X+x]\t\t([%04X+x]=%02X)", addr, addr, v);
					break;
				case 0xE0:	// INC a,x
					v = loadData(addr + x);
					adjustZN(++v);
					storeData(addr + x, v);
					_log("\t\tINC  [%04X+x]\t\t([%04X+x]=%02X)", addr, addr, v);
					break;
				default:
					bcnt = 0;
			}
			break;
		default:
					bcnt = 0;
	}
}

void NESCT01Module::action_other(u8 instr) {
	// not implemented yet
}


} // namespace nes

} // namespace vfemu

