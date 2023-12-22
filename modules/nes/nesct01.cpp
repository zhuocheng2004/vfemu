
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
		if (module->in_irq < 1 && module->lock.try_lock()) {
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


u8 NESCT01Module::adjustZ(u8 v) {
	if (v == 0)
		p |= MSK_ZERO;
	else
		p &= ~MSK_ZERO;

	return v;
}

u8 NESCT01Module::adjustN(u8 v) {
	if (v & 0x80)
		p |= MSK_NEG;
	else
		p &= ~MSK_NEG;

	return v;
}

u8 NESCT01Module::add(u8 v1, u8 v2) {
	u16 result = v1 + v2 + (p & MSK_CARRY);
	if (result & 0xff00)
		p |= MSK_CARRY;
	else
		p &= ~MSK_CARRY;

	return result & 0xff;
}

u8 NESCT01Module::sub(u8 v1, u8 v2) {
	u16 result = v1 - v2 + (p & MSK_CARRY) - 1;
	if (result & 0xff00)
		p &= ~MSK_CARRY;
	else
		p |= MSK_CARRY;
	
	return result & 0xff;
}

void NESCT01Module::bit(u8 v) {
	adjustZ(a & v);
	adjustN(v);
	if (v && 0x40)
		p |= MSK_OVERFLOW;
	else
		p &= ~MSK_OVERFLOW;
}

void NESCT01Module::cmp(u8 v1, u8 v2) {
	u16 diff = v1 - v2;
	if (diff & 0xff00)
		p &= ~MSK_CARRY;
	else
		p |= MSK_CARRY;
	
	adjustZN(diff & 0xff);
}

u8 NESCT01Module::asl(u8 v) {
	if (v & 0x80)
		p |= MSK_CARRY;
	else
		p &= ~MSK_CARRY;

	return v << 1;
}

u8 NESCT01Module::lsr(u8 v) {
	if (v & 0x01)
		p |= MSK_CARRY;
	else
		p &= ~MSK_CARRY;

	return v >> 1;
}

u8 NESCT01Module::rol(u8 v) {
	u8 result = (v << 1) | (p & MSK_CARRY);
	if (v & 0x80)
		p |= MSK_CARRY;
	else
		p &= ~MSK_CARRY;

	return result;
}

u8 NESCT01Module::ror(u8 v) {
	u8 result = (v >> 1) | (p << 7);
	if (v & 0x01)
		p |= MSK_CARRY;
	else
		p &= ~MSK_CARRY;

	return result;
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

static bool break_invalid_op = true;
static bool invalid_op = false;

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

	if (break_invalid_op && invalid_op) {
		puts("Invalid Opcode.");
		invalid_op = false;
		bcnt = 0;
	}
}

void NESCT01Module::branch_irq(u16 vector_addr) {
	in_irq++;
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
	u8 v = 0;
	u8 zaddr = 0, addrl = 0, addrh = 0;
	u16 addr, v3;
	switch (instr & 0x1C) {
		case 0x00:
			switch (instr & 0xE0) {
				case 0x00:	// BRK
					branch_irq(0xFFFE);
					p |= MSK_INT;
					_log("\t\t\tBRK");
					break;
				case 0x20:	// JSR a
					addrl = loadData(pc++); addrh = loadData(pc++);
					_log(" %02X %02X", addrl, addrh);
					addr = addrl + (addrh << 8);
					v3 = pc - 1;
					pushStack(v3 >> 8);
					pushStack(v3 & 0xff);
					pc = addr;
					_log("\t\tJSR\t%04X", addr);
					break;
				case 0x40:	// RTI
					p = popStack();
					addrl = popStack(); addrh = popStack();
					pc = (u16) addrl +(addrh << 8);
					if (in_irq > 0)
						in_irq--;
					_log("\t\t\tRTI");
					std::cout << "IRQ HANDLE TIME: " << (std::chrono::steady_clock::now() - t1).count() << std::endl;
					break;
				case 0x60:	// RTS
					addrl = popStack(); addrh = popStack();
					pc = (u16) addrl +(addrh << 8) + 1;
					_log("\t\t\tRTS");
					break;
				case 0xA0:	// LDY #i
					v = loadData(pc++);
					_log(" %02X", v);
					adjustZN(y = v);
					_log("\t\tLDY\t%02X", v);
					break;
				case 0xC0:	// CPY #i
					v = loadData(pc++);
					_log(" %02X", v);
					cmp(y, v);
					_log("\t\tCPY\t%02X", v);
					break;
				case 0xE0:	// CPX #i
					v = loadData(pc++);
					_log(" %02X", v);
					cmp(x, v);
					_log("\t\tCPX\t%02X", v);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x04:	// d
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x20:	// BIT d
					bit(loadData(zaddr));
					_log("\t\tBIT\t[%02X]", zaddr);
					break;
				case 0x80:	// STY d
					storeData(zaddr, y);
					_log("\t\tSTY\t[%02X]", zaddr);
					break;
				case 0xA0:	// LDY d
					adjustZN(y = loadData(zaddr));
					_log("\t\tLDY\t[%02X]", zaddr);
					break;
				case 0xC0:	// CPY d
					cmp(y, loadData(zaddr));
					_log("\t\tCPY\t[%02X]", zaddr);
					break;
				case 0xE0:	// CPX d
					cmp(x, loadData(zaddr));
					_log("\t\tCPX\t[%02X]", zaddr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x08:
			switch (instr & 0xE0) {
				case 0x00:	// PHP
					pushStack(p);
					_log("\t\t\tPHP");
					break;
				case 0x20:	// PLP
					p = popStack();
					_log("\t\t\tPLP");
					break;
				case 0x40:	// PHA
					pushStack(a);
					_log("\t\t\tPHA");
					break;
				case 0x60:	// PLA
					adjustZN(a = popStack());
					_log("\t\t\tPLA");
					break;
				case 0x80:	// DEY
					adjustZN(--y);
					_log("\t\t\tDEY");
					break;
				case 0xA0:	// TAY
					adjustZN(y = a);
					_log("\t\t\tTAY");
					break;
				case 0xC0:	// INY
					adjustZN(++y);
					_log("\t\t\tINY");
					break;
				case 0xE0:	// INX
					adjustZN(++x);
					_log("\t\t\tINX");
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x0C:	// a
			addrl = loadData(pc++); addrh = loadData(pc++);
			_log(" %02X %02X", addrl, addrh);
			addr = addrl + (addrh << 8);
			switch (instr & 0xE0) {
				case 0x20:	// BIT a
					bit(loadData(addr));
					_log("\t\tBIT\t[%04X]");
					break;
				case 0x40:	// JMP a
					pc = addr;
					_log("\t\tJMP\t%04X", addr);
					break;
				case 0x60:	// JMP (a)
					addrl = loadData(addr); addrh = loadData(addr + 1);
					pc = addrl + (addrh << 8);
					_log("\t\tJMP\t[%04X]", addr);
					break;
				case 0x80:	// STY a
					storeData(addr, y);
					_log("\t\tSTY\t[%04X]", addr);
					break;
				case 0xA0:	// LDY a
					adjustZN(y = loadData(addr));
					_log("\t\tLDY\t[%04X]", addr);
					break;
				case 0xC0:	// CPY a
					cmp(y, loadData(addr));
					_log("\t\tCPY\t[%04X]", addr);
					break;
				case 0xE0:	// CPX a
					cmp(x, loadData(addr));
					_log("\t\tCPX\t[%04X]", addr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x10:	// *+d
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x00:	// BPL
					if (!(p & MSK_NEG))
						branch(zaddr);
					_log("\t\tBPL\t+%02X", zaddr);
					break;
				case 0x20:	// BMI
					if (p & MSK_NEG)
						branch(zaddr);
					_log("\t\tBMI\t+%02X", zaddr);
					break;
				case 0x40:	// BVC
					if (!(p & MSK_OVERFLOW))
						branch(zaddr);
					_log("\t\tBVC\t+%02X", zaddr);
					break;
				case 0x60:	// BVS
					if (p & MSK_OVERFLOW)
						branch(zaddr);
					_log("\t\tBVS\t+%02X", zaddr);
					break;
				case 0x80:	// BCC
					if (!(p & MSK_CARRY))
						branch(zaddr);
					_log("\t\tBCC\t+%02X", zaddr);
					break;
				case 0xA0:	// BCS
					if (p & MSK_CARRY)
						branch(zaddr);
					_log("\t\tBCS\t+%02X", zaddr);
					break;
				case 0xC0:	// BNE
					if (!(p & MSK_ZERO))
						branch(zaddr);
					_log("\t\tBNE\t+%02X", zaddr);
					break;
				case 0xE0:	// BEQ
					if (p & MSK_ZERO)
						branch(zaddr);
					_log("\t\tBEQ\t+%02X", zaddr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x14:	// d,x
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			addr = zaddr + x;
			switch (instr & 0xE0) {
				case 0x80:	// STY d,x
					storeData(addr, y);
					_log("\t\tSTY  [%02X+x]", zaddr);
					break;
				case 0xA0:	// LDY d,x
					adjustZN(y = loadData(addr));
					_log("\t\tLDY  [%02X+x]", zaddr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x18:
			switch (instr & 0xE0) {
				case 0x00:	// CLC
					p &= ~MSK_CARRY;
					_log("\t\t\tCLC");
					break;
				case 0x20:	// SEC
					p |= MSK_CARRY;
					_log("\t\t\tSEC");
					break;
				case 0x40:	// CLI
					p &= ~MSK_INT;
					_log("\t\t\tCLI");
					break;
				case 0x60:	// SEI
					p |= MSK_INT;
					_log("\t\t\tSEI");
					break;
				case 0x80:	// TYA
					adjustZN(a = y);
					_log("\t\t\tTYA");
					break;
				case 0xA0:	// CLV
					p &= ~MSK_OVERFLOW;
					_log("\t\t\tCLV");
					break;
				case 0xC0:	// CLD
					p &= ~MSK_DEC;
					_log("\t\t\tCLD");
					break;
				case 0xE0:	// SED
					p |= MSK_DEC;
					_log("\t\t\tSED");
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x1C:	// a,x
			addrl = loadData(pc++); addrh = loadData(pc++);
			_log(" %02X %02X", addrl, addrh);
			addr = addrl + (addrh << 8);
			switch (instr & 0xE0) {
				case 0xA0:	// LDY a,x
					adjustZN(y = loadData(addr + x));
					_log("\t\tLDY\t[%04X+x]", addr);
					break;
				default:
					invalid_op = true;
			}
			break;
		default:
			invalid_op = true;
	}
}

void NESCT01Module::action_alu(u8 instr) {
	u8 v = 0;
	u8 zaddr = 0, addrl = 0, addrh = 0;
	u16 addr;
	switch (instr & 0x1C) {
		case 0x00:	// (d,x)
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			addrl = loadData(zaddr + x); addrh = loadData(zaddr + x + 1);
			addr = addrl + (addrh << 8);
			switch (instr & 0xE0) {
				case 0x00:	// ORA (d,x)
					adjustZN(a |= loadData(addr));
					_log("\t\tORA\t[[%02X+x]]", zaddr);
					break;
				case 0x20:	// AND (d,x)
					adjustZN(a &= loadData(addr));
					_log("\t\tAND\t[[%02X+x]]", zaddr);
					break;
				case 0x40:	// EOR (d,x)
					adjustZN(a ^= loadData(addr));
					_log("\t\tEOR\t[[%02X+x]]", zaddr);
					break;
				case 0x60:	// ADC (d,x)
					adjustZN(a = add(a, loadData(addr)));
					_log("\t\tADC\t[[%02X+x]]", zaddr);
					break;
				case 0x80:	// STA (d,x)
					storeData(addr, a);
					_log("\t\tSTA\t[[%02X+x]]", zaddr);
					break;
				case 0xA0:	// LDA (d,x)
					adjustZN(a = loadData(addr));
					_log("\t\tLDA\t[[%02X+x]]", zaddr);
					break;
				case 0xC0:	// CMP (d,x)
					cmp(a, loadData(addr));
					_log("\t\tCMP\t[[%02X+x]]", zaddr);
					break;
				case 0xE0:	// SBC (d,x)
					adjustZN(a = sub(a, loadData(addr)));
					_log("\t\tSBC\t[[%02X+x]]", zaddr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x04:	// d
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x00:	// ORA d
					adjustZN(a |= loadData(zaddr));
					_log("\t\tORA\t[%02X]", zaddr);
					break;
				case 0x20:	// AND d
					adjustZN(a &= loadData(zaddr));
					_log("\t\tAND\t[%02X]", zaddr);
					break;
				case 0x40:	// EOR d
					adjustZN(a ^= loadData(zaddr));
					_log("\t\tEOR\t[%02X]", zaddr);
					break;
				case 0x60:	// ADC d
					adjustZN(a = add(a, loadData(zaddr)));
					_log("\t\tADC\t[%02X]", zaddr);
					break;
				case 0x80:	// STA d
					storeData(zaddr, a);
					_log("\t\tSTA\t[%02X]", zaddr);
					break;
				case 0xA0:	// LDA d
					adjustZN(a = loadData(zaddr));
					_log("\t\tLDA\t[%02X]", zaddr);
					break;
				case 0xC0:	// CMP d
					cmp(a, loadData(zaddr));
					_log("\t\tCMP\t[%02X]", zaddr);
					break;
				case 0xE0:	// SBC d
					adjustZN(a = sub(a, loadData(zaddr)));
					_log("\t\tSBC\t[%02X]", zaddr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x08:	// #i
			v = loadData(pc++);
			_log(" %02X", v);
			switch (instr & 0xE0) {
				case 0x00:	// ORA #i
					adjustZN(a |= v);
					_log("\t\tORA\t%02X", v);
					break;
				case 0x20:	// AND #i
					adjustZN(a &= v);
					_log("\t\tAND\t%02X", v);
					break;
				case 0x40:	// EOR #i
					adjustZN(a ^= v);
					_log("\t\tEOR\t%02X", v);
					break;
				case 0x60:	// ADC #i
					adjustZN(a = add(a, v));
					_log("\t\tADC\t%02X", v);
					break;
				case 0xA0:	// LDA #i
					adjustZN(a = v);
					_log("\t\tLDA\t%02X", v);
					break;
				case 0xC0:	// CMP #i
					cmp(a, v);
					_log("\t\tCMP\t%02X", v);
					break;
				case 0xE0:	// SBC #i
					adjustZN(a = sub(a, v));
					_log("\t\tSBC\t%02X", v);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x0C:	// a
			addrl = loadData(pc++); addrh = loadData(pc++);
			_log(" %02X %02X", addrl, addrh);
			addr = addrl + (addrh << 8);
			switch (instr & 0xE0) {
				case 0x00:	// ORA a
					adjustZN(a |= loadData(addr));
					_log("\t\tORA\t[%04X]", addr);
					break;
				case 0x20:	// AND a
					adjustZN(a &= loadData(addr));
					_log("\t\tAND\t[%04X]", addr);
					break;
				case 0x40:	// EOR a
					adjustZN(a ^= loadData(addr));
					_log("\t\tEOR\t[%04X]", addr);
					break;
				case 0x60:	// ADC a
					adjustZN(a = add(a, loadData(addr)));
					_log("\t\tADC\t[%04X]", addr);
					break;
				case 0x80:	// STA a
					storeData(addr, a);
					_log("\t\tSTA\t[%04X]", addr);
					break;
				case 0xA0:	// LDA a
					adjustZN(a = loadData(addr));
					_log("\t\tLDA\t[%04X]", addr);
					break;
				case 0xC0:	// CMP a
					cmp(a, loadData(addr));
					_log("\t\tCMP\t[%04X]", addr);
					break;
				case 0xE0:	// SBC a
					adjustZN(a = sub(a, loadData(addr)));
					_log("\t\tSBC\t[%04X]", addr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x10:	// (d),y
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			addrl = loadData(zaddr); addrh = loadData(zaddr + 1);
			addr = addrl + (addrh << 8) + y;
			switch (instr & 0xE0) {
				case 0x00:	// ORA (d),y
					adjustZN(a |= loadData(addr));
					_log("\t\tORA\t[[%02X]+y]", zaddr);
					break;
				case 0x20:	// AND (d),y
					adjustZN(a &= loadData(addr));
					_log("\t\tAND\t[[%02X]+y]", zaddr);
					break;
				case 0x40:	// EOR (d),y
					adjustZN(a ^= loadData(addr));
					_log("\t\tEOR\t[[%02X]+y]", zaddr);
					break;
				case 0x60:	// ADC (d),y
					adjustZN(a = add(a, loadData(addr)));
					_log("\t\tADC\t[[%02X]+y]", zaddr);
					break;
				case 0x80:	// STA (d),y
					storeData(addr, a);
					_log("\t\tSTA\t[[%02X]+y]", zaddr);
					break;
				case 0xA0:	// LDA (d),y
					adjustZN(a = loadData(addr));
					_log("\t\tLDA\t[[%02X]+y]", zaddr);
					break;
				case 0xC0:	// CMP (d),y
					cmp(a, loadData(addr));
					_log("\t\tCMP\t[[%02X]+y]", zaddr);
					break;
				case 0xE0:	// SBC (d),y
					adjustZN(a = sub(a, loadData(addr)));
					_log("\t\tSBC\t[[%02X]+y]", zaddr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x14:	// d,x
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			addr = zaddr + x;
			switch (instr & 0xE0) {
				case 0x00:	// ORA a
					adjustZN(a |= loadData(addr));
					_log("\t\tORA\t[%02X+x]", zaddr);
					break;
				case 0x20:	// AND a
					adjustZN(a &= loadData(addr));
					_log("\t\tAND\t[%02X+x]", zaddr);
					break;
				case 0x40:	// EOR a
					adjustZN(a ^= loadData(addr));
					_log("\t\tEOR\t[%02X+x]", zaddr);
					break;
				case 0x60:	// ADC a
					adjustZN(a = add(a, loadData(addr)));
					_log("\t\tADC\t[%02X+x]", zaddr);
					break;
				case 0x80:	// STA a
					storeData(addr, a);
					_log("\t\tSTA\t[%02X+x]", zaddr);
					break;
				case 0xA0:	// LDA a
					adjustZN(a = loadData(addr));
					_log("\t\tLDA\t[%02X+x]", zaddr);
					break;
				case 0xC0:	// CMP a
					cmp(a, loadData(addr));
					_log("\t\tCMP\t[%02X+x]", zaddr);
					break;
				case 0xE0:	// SBC a
					adjustZN(a = sub(a, loadData(addr)));
					_log("\t\tSBC\t[%02X+x]", zaddr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x18:	// a,y
			addrl = loadData(pc++); addrh = loadData(pc++);
			_log(" %02X %02X", addrl, addrh);
			addr = addrl + (addrh << 8);
			switch (instr & 0xE0) {
				case 0x00:	// ORA a,y
					adjustZN(a |= loadData(addr + y));
					_log("\t\tORA\t[%04X+y]", addr);
					break;
				case 0x20:	// AND a,y
					adjustZN(a &= loadData(addr + y));
					_log("\t\tAND\t[%04X+y]", addr);
					break;
				case 0x40:	// EOR a,y
					adjustZN(a ^= loadData(addr + y));
					_log("\t\tEOR\t[%04X+y]", addr);
					break;
				case 0x60:	// ADC a,y
					adjustZN(a = add(a, loadData(addr + y)));
					_log("\t\tADC\t[%04X+y]", addr);
					break;
				case 0x80:	// STA a,y
					storeData(addr + y, a);
					_log("\t\tSTA\t[%04X+y]", addr);
					break;
				case 0xA0:	// LDA a,y
					adjustZN(a = loadData(addr + y));
					_log("\t\tLDA\t[%04X+y]", addr);
					break;
				case 0xC0:	// CMP a,y
					cmp(a, loadData(addr + y));
					_log("\t\tCMP\t[%04X+y]", addr);
					break;
				case 0xE0:	// SBC a,y
					adjustZN(a = sub(a, loadData(addr + y)));
					_log("\t\tSBC\t[%04X+y]", addr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x1C:	// a,x
			addrl = loadData(pc++); addrh = loadData(pc++);
			_log(" %02X %02X", addrl, addrh);
			addr = addrl + (addrh << 8);
			switch (instr & 0xE0) {
				case 0x00:	// ORA a,x
					adjustZN(a |= loadData(addr + x));
					_log("\t\tORA\t[%04X+x]", addr);
					break;
				case 0x20:	// AND a,x
					adjustZN(a &= loadData(addr + x));
					_log("\t\tAND\t[%04X+x]", addr);
					break;
				case 0x40:	// EOR a,x
					adjustZN(a ^= loadData(addr + x));
					_log("\t\tEOR\t[%04X+x]", addr);
					break;
				case 0x60:	// ADC a,x
					adjustZN(a = add(a, loadData(addr + x)));
					_log("\t\tADC\t[%04X+x]", addr);
					break;
				case 0x80:	// STA a,x
					storeData(addr + x, a);
					_log("\t\tSTA\t[%04X+x]", addr);
					break;
				case 0xA0:	// LDA a,x
					adjustZN(a = loadData(addr + x));
					_log("\t\tLDA\t[%04X+x]", addr);
					break;
				case 0xC0:	// CMP a,x
					cmp(a, loadData(addr + x));
					_log("\t\tCMP\t[%04X+x]", addr);
					break;
				case 0xE0:	// SBC a,x
					adjustZN(a = sub(a, loadData(addr + x)));
					_log("\t\tSBC\t[%04X+x]", addr);
					break;
				default:
					invalid_op = true;
			}
			break;
		default:
			invalid_op = true;
	}
}

void NESCT01Module::action_mv(u8 instr) {
	u8 v = 0, v2 = 0;
	u8 zaddr = 0, addrl = 0, addrh = 0;
	u16 addr;
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
					invalid_op = true;
			}
			break;
		case 0x04:	// d
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x00:	// ASL
					adjustN(v = asl(loadData(zaddr))); adjustZ(a);
					storeData(zaddr, v);
					_log("\t\tASL\t[%02X]", zaddr);
					break;
				case 0x20:	// ROL z
					adjustN(v = rol(loadData(zaddr))); adjustZ(a);
					storeData(zaddr, v);
					_log("\t\tROL\t[%02X]", zaddr);
					break;
				case 0x40:	// LSR z
					adjustZN(v = lsr(loadData(zaddr)));
					storeData(zaddr, v);
					_log("\t\tLSR\t[%02X]", zaddr);
					break;
				case 0x60:	// ROR z
					adjustN(v = ror(loadData(zaddr))); adjustZ(a);
					storeData(zaddr, v);
					_log("\t\tROR\t[%02X]", zaddr);
					break;
				case 0x80:	// STX z
					storeData(zaddr, x);
					_log("\t\tSTX\t[%02X]", zaddr);
					break;
				case 0xA0:	// LDX z
					adjustZN(x = loadData(zaddr));
					_log("\t\tLDX\t[%02X]", zaddr);
					break;
				case 0xC0:	// DEC z
					v = loadData(zaddr);
					adjustZN(--v);
					storeData(zaddr, v);
					_log("\t\tDEC\t[%02X]", zaddr);
					break;
				case 0xE0:	// INC z
					v = loadData(zaddr);
					adjustZN(++v);
					storeData(zaddr, v);
					_log("\t\tINC\t[%02X]", zaddr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x08:
			switch (instr & 0xE0) {
				case 0x00:	// ASL
					adjustZN(a = asl(a));
					_log("\t\t\tASL");
					break;
				case 0x20:	// ROL
					adjustZN(a = rol(a));
					_log("\t\t\tROL");
					break;
				case 0x40:	// LSR
					adjustZN(a = lsr(a));
					_log("\t\t\tLSR");
					break;
				case 0x60:	// ROR
					adjustZN(a = ror(a));
					_log("\t\t\tROR");
					break;
				case 0x80:	// TXA
					adjustZN(a = x);
					_log("\t\t\tTXA");
					break;
				case 0xA0:	// TAX
					adjustZN(x = a);
					_log("\t\t\tTAX");
					break;
				case 0xC0:	// DEX
					adjustZN(--x);
					_log("\t\t\tDEX");
					break;
				case 0xE0:	// NOP
					_log("\t\t\tNOP");
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x0C:	// a
			addrl = loadData(pc++); addrh = loadData(pc++);
			_log(" %02X %02X", addrl, addrh);
			addr = addrl + (addrh << 8);
			switch (instr & 0xE0) {
				case 0x00:	// ASL a
					adjustN(v = asl(loadData(addr))); adjustZ(a);
					storeData(addr, v);
					_log("\t\tASL\t[%04X]", addr);
					break;
				case 0x20:	// ROL a
					adjustN(v = rol(loadData(addr))); adjustZ(a);
					storeData(addr, v);
					_log("\t\tROL\t[%04X]", addr);
					break;
				case 0x40:	// LSR a
					adjustZN(v = lsr(loadData(addr)));
					storeData(addr, v);
					_log("\t\tLSR\t[%04X]", addr);
					break;
				case 0x60:	// ROR a
					adjustN(v = ror(loadData(addr))); adjustZ(a);
					storeData(addr, v);
					_log("\t\tROR\t[%04X]", addr);
					break;
				case 0x80:	// STX a
					storeData(addr, x);
					_log("\t\tSTX\t[%04X]", addr);
					break;
				case 0xA0:	// LDX a
					adjustZN(x = loadData(addr));
					_log("\t\tLDX\t[%04X]", addr);
					break;
				case 0xC0:	// DEC a
					v = loadData(addr);
					adjustZN(--v);
					storeData(addr, v);
					_log("\t\tDEC\t[%04X]", addr);
					break;
				case 0xE0:	// INC a
					v = loadData(addr);
					adjustZN(++v);
					storeData(addr, v);
					_log("\t\tINC\t[%04X]", addr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x14:	// d,[x|y]
			zaddr = loadData(pc++);
			_log(" %02X", zaddr);
			switch (instr & 0xE0) {
				case 0x00:	// ASL d,x
					adjustN(v = asl(loadData(zaddr + x))); adjustZ(a);
					storeData(zaddr + x, v);
					_log("\t\tASL\t[%02X+x]", zaddr);
					break;
				case 0x20:	// ROL d,x
					adjustN(v = rol(loadData(zaddr + x))); adjustZ(a);
					storeData(zaddr + x, v);
					_log("\t\tROL\t[%02X+x]", zaddr);
					break;
				case 0x40:	// LSR d,x
					adjustZN(v = lsr(loadData(zaddr + x)));
					storeData(zaddr + x, v);
					_log("\t\tLSR\t[%02X+x]", zaddr);
					break;
				case 0x60:	// ROR d,x
					adjustN(v = ror(loadData(zaddr + x))); adjustZ(a);
					storeData(zaddr + x, v);
					_log("\t\tROR\t[%02X+x]", zaddr);
					break;
				case 0x80:	// STX d,y
					storeData(zaddr + y, x);
					_log("\t\tSTX\t[%02X+y]", zaddr);
					break;
				case 0xA0:	// LDX d,y
					adjustZN(x = loadData(zaddr + y));
					_log("\t\tLDX\t[%02X+y]", zaddr);
					break;
				case 0xC0:	// DEC d,x
					v = loadData(zaddr + x);
					adjustZN(--v);
					storeData(zaddr + x, v);
					_log("\t\tDEC\t[%02X+x]", zaddr);
					break;
				case 0xE0:	// INC d,x
					v = loadData(zaddr + x);
					adjustZN(++v);
					storeData(zaddr + x, v);
					_log("\t\tINC\t[%02X+x]", zaddr);
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x18:
			switch (instr & 0xE0) {
				case 0x80:	// TXS
					sp = x;
					_log("\t\t\tTXS");
					break;
				case 0xA0:	// TSX
					adjustZN(x = sp);
					_log("\t\t\tTSX");
					break;
				default:
					invalid_op = true;
			}
			break;
		case 0x1C:	// a,[x|y]
			addrl = loadData(pc++); addrh = loadData(pc++);
			_log(" %02X %02X", addrl, addrh);
			addr = addrl + (addrh << 8);
			switch (instr & 0xE0) {
				case 0x00:	// ASL a,x
					adjustN(v = asl(loadData(addr + x))); adjustZ(a);
					storeData(addr + x, v);
					_log("\t\tASL\t[%04X+x]", addr);
					break;
				case 0x20:	// ROL a,x
					adjustN(v = rol(loadData(addr + x))); adjustZ(a);
					storeData(addr + x, v);
					_log("\t\tROL\t[%04X+x]", addr);
					break;
				case 0x40:	// LSR a,x
					adjustZN(v = lsr(loadData(addr + x)));
					storeData(addr + x, v);
					_log("\t\tLSR\t[%04X+x]", addr);
					break;
				case 0x60:	// ROR a,x
					adjustN(v = ror(loadData(addr + x))); adjustZ(a);
					storeData(addr + x, v);
					_log("\t\tROR\t[%04X+x]", addr);
					break;
				case 0xA0:	// LDX a,y
					adjustZN(x = loadData(addr + y));
					_log("\t\tLDX\t[%04X+y]", addr);
					break;
				case 0xC0:	// DEC a,x
					v = loadData(addr + x);
					adjustZN(--v);
					storeData(addr + x, v);
					_log("\t\tDEC\t[%04X+x]", addr);
					break;
				case 0xE0:	// INC a,x
					v = loadData(addr + x);
					adjustZN(++v);
					storeData(addr + x, v);
					_log("\t\tINC\t[%04X+x]", addr);
					break;
				default:
					invalid_op = true;
			}
			break;
		default:
			invalid_op = true;
	}
}

void NESCT01Module::action_other(u8 instr) {
	// not implemented yet
}


} // namespace nes

} // namespace vfemu

