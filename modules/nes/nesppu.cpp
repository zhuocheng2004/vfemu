
#include <iostream>
#include <thread>
#include <nes/nesppu.h>


namespace vfemu {

namespace nes {

using namespace std::chrono_literals;


#define ROWS		30
#define COLUMNS		32
#define PITCH		COLUMNS * 8


Status NESPPUModule::init() {
	vram = new u8[0x1000];

	render_thread = new std::thread(render_thread_func, this);
	return Status::SUCCESS;
}

Status NESPPUModule::exit() {
	running = false;
	sendToPort(IDX_EXIT, 1);

	delete vram;
	return Status::SUCCESS;
}


u8 NESPPUModule::readVRAM(u16 addr) {
	return vram[addr & 0xFFF];
}

void NESPPUModule::writeVRAM(u16 addr, u8 data) {
	addr &= 0x0FFF;
	if (vertical_mirroring)
		vram[addr] = vram[addr ^ 0x0800] = data;
	else
		vram[addr] = vram[addr ^ 0x0400] = data;
}


Status NESPPUModule::clock_receive(Module* receiver, u64 signal) {
	return Status::SUCCESS;
}

Status NESPPUModule::reset_receive(Module* receiver, u64 signal) {
	auto module = (NESPPUModule*) receiver;
	module->running = false;

	module->vblank = true;
	module->nmi = false;

	module->v = 0;
	module->t = 0;
	module->x = 0;
	module->w = 0;

	module->bgPTable = 0x0000;

	module->greyscale = false;
	module->showBG = module->showSP = true;

	module->scrollX = module->scrollY = 0;

	module->running = true;
	return Status::SUCCESS;
}

Status NESPPUModule::rw_receive(Module* receiver, u64 signal) {
	auto module = (NESPPUModule*) receiver;
	u8 data;
	u16 paddr;
	if (!module->running)
		return Status::SUCCESS;
	if (signal & 0x1) {	// read
		switch (module->addr & 0xF) {
			case 0x2:	// PPUSTATUS
				module->w = 0;	// clear toggle
				data = 0x0;
				if (module->vblank)
					data |= MSTATUS_V;
				module->sendToPort(IDX_DATA, data);
				break;
			case 0x7:	// PPUDATA
				paddr = module->v;
				//printf("  { PPU: want to read data @%04X }  ", paddr);
				if ((paddr & 0xE000) == 0) {
					// CHR ROM
					data = module->getChrData(paddr);
					module->sendToPort(IDX_DATA, data);
					module->v += 1;
				}
				break;
		}
	} else {	// write
		data = module->data;
		switch (module->addr & 0xF) {
			case 0x0:	// PPUCTRL
				// choose nametable base addr
				module->t &= 0xf3ff;
				module->t |= (data & 0x3) << 10;

				// chose sprite pattern table base addr
				module->spPTable = (data & MCTRL_S) << 9;

				// chose background pattern table base addr
				module->bgPTable = (data & MCTRL_B) << 8;

				// generate NMI ?
				module->nmi = (data & MCTRL_V) ? true : false;
				break;
			case 0x1:	// PPUMASK
				module->greyscale = (data & MMSK_G) ? true : false;
				module->showBG = (data & MMSK_B) ? true : false;
				module->showSP = (data & MMSK_S) ? true : false;
				break;
			case 0x5:	// PPUSCROLL
				if (module->w & 0x1) { /* 2nd: Y scroll */
					module->t &= ~0x73E0;
					module->t |= (data & 0x7) << 12;
					module->t |= (data & 0xF8) << 2;
					//printf("PPU scroll:  t=%04X \n", module->t);
				} else { /* 1st: X scroll */
					module->t &= ~0x1F;
					module->t |= (data & 0xF8) >> 3;
					module->x = data & 0x7;
				}
				module->w ^= 0x1;
				break;
			case 0x6:	// PPUADDR
				if (module->w & 0x1 /* 2nd: low */) {
					module->t &= ~0x00FF;
					module->t |= data & 0x00FF;
					module->v = module->t;
					//printf("PPU addr:  v=%04X \n", module->v);
				} else { /* 1st: high */
					module->t &= ~0x7F00;
					module->t |= (data & 0x3F) << 8;
				}
				module->w ^= 0x1;
				break;
			case 0x7:	// PPUDATA
				paddr = module->v & 0x3FFF;
				if ((paddr & 0xF000) == 0x2000 || 
					((paddr & 0xF000) == 0x3000 && (paddr < 0x3F00))) {
					// nametable
					module->writeVRAM(paddr, data);
				}

				u8 paletteIdx = 0, fineIdx;
				if ((paddr & 0xFFE0) == 0x3F00) {
					// palette
					paletteIdx = (paddr & 0x000F) >> 2;
					fineIdx = paddr & 0x0003;
					if ((paddr & 0x000F) == 0x0000) {
						// universal background
						module->universalBGColor = data;
						for (int i = 0; i < 3; i++) {
							module->bgPalettes[i][0] = data;
							module->spPalettes[i][0] = data;
						}
					} else if (paddr & 0x0010) {
						// sprite palette
						module->spPalettes[paletteIdx][fineIdx] = data;
					} else {
						// background palette
						module->bgPalettes[paletteIdx][fineIdx] = data;
					}
				}
				module->v += 1;
				break;
		}
	}
	return Status::SUCCESS;
}

Status NESPPUModule::addr_receive(Module* receiver, u64 addr) {
	auto module = (NESPPUModule*) receiver;
	module->addr = addr;
	return Status::SUCCESS;
}

Status NESPPUModule::data_receive(Module* receiver, u64 data) {
	auto module = (NESPPUModule*) receiver;
	module->data = data;
	return Status::SUCCESS;
}

Status NESPPUModule::pdata_receive(Module* receiver, u64 data) {
	auto module = (NESPPUModule*) receiver;
	module->pdata = data & 0xFF;
	return Status::SUCCESS;
}


static const u32 paletteRGBs[] = {
	// RGBA
	0x6D6D6DFF, 0x002491FF, 0x0000DAFF, 0x6D48DAFF,
	0x91006DFF, 0xB6006DFF, 0xB62400FF, 0x914800FF,
	0x6D4800FF, 0x244800FF, 0x006D24FF, 0x009100FF,
	0x004848FF, 0x000000FF, 0x000000FF, 0x000000FF,

	0xB6B6B6FF, 0x006DDAFF, 0x0048FFFF, 0x9100FFFF,
	0xB600FFFF, 0xFF0091FF, 0xFF0000FF, 0xDA6D00FF,
	0x916D00FF, 0x249100FF, 0x009100FF, 0x00B66DFF,
	0x009191FF, 0x000000FF, 0x000000FF, 0x000000FF,

	0xFFFFFFFF, 0x6DB6FFFF, 0x9191FFFF, 0xDA6DFFFF,
	0xFF00FFFF, 0xFF6DFFFF, 0xFF9100FF, 0xFFB600FF,
	0xDADA00FF, 0x6DDA00FF, 0x00FF00FF, 0x48FFDAFF,
	0x00FFFFFF, 0x000000FF, 0x000000FF, 0x000000FF,

	0xFFFFFFFF, 0xB6DAFFFF, 0xDAB6FFFF, 0xFFB6FFFF,
	0xFF91FFFF, 0xFFB6B6FF, 0xFFDA91FF, 0xFFFF48FF,
	0xFFFF6DFF, 0xB6FF48FF, 0x91FF6DFF, 0x48FFDAFF,
	0x91DAFFFF, 0x000000FF, 0x000000FF, 0x000000FF,
};


void NESPPUModule::fillPixels(NESPPUModule* module, u32* pixels) {
	static u8* bgChrCache = new u8[0x1000];

	// refresh CHR ROM Cache
	// in the future, we should save each of them as texture
	// and use shade program to render them with palette
	for (int i = 0; i < ((16*16)<<4); i += (1<<4)) {
		for (int l = 0; l < 8; l++) {
			bgChrCache[i + (0 << 3) + l] = module->getChrData(module->bgPTable + i + (0 << 3) + l);
			bgChrCache[i + (1 << 3) + l] = module->getChrData(module->bgPTable + i + (1 << 3) + l);
		}
	}

	// build palette
	static u32 bgPalettes[4][4];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (module->greyscale)
				bgPalettes[i][j] = paletteRGBs[module->bgPalettes[i][j] & 0x30];
			else
				bgPalettes[i][j] = paletteRGBs[module->bgPalettes[i][j]];
		}
	}

	u16 v = module->t;	// coarse X/Y and fine Y
	u8 x = module->x & 0x7;	// fine X

	for (int i = 0; i < ROWS * 8; i++) {
		u8 fineY = (v >> 12) & 0x7;
		for (int j = 0; j < COLUMNS * 8; j++) {
			u16 tileAddr = 0x2000 | (v & 0x0FFF);
			u16 attrAddr = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
			u8 tileIdx = module->readVRAM(tileAddr);
			u8 attrData = module->readVRAM(attrAddr);
			u8 attrIdx;
			u8 col = v & 0x1F;
			u8 row = (v & 0x03E0) >> 5;
			if (row & 0x2) {
				if (col & 0x2)
					attrIdx = (attrData >> 6) & 0x3;
				else
					attrIdx = (attrData >> 4) & 0x3;
			} else {
				if (col & 0x2)
					attrIdx = (attrData >> 2) & 0x3;
				else
					attrIdx = (attrData >> 0) & 0x3;
			}
			
			u8 data1 = bgChrCache[(tileIdx << 4) + (0 << 3) + fineY];
			u8 data2 = bgChrCache[(tileIdx << 4) + (1 << 3) + fineY];
			u8 plIdx = (((data2 >> (7-x)) & 0x1) << 1) + ((data1 >> (7-x)) & 0x1);
			if (module->showBG)
				pixels[i * PITCH + j] = bgPalettes[attrIdx][plIdx];

			// X increment
			if (x != 0x7) {		// fine X != 7
				x++;
			} else {			// Coarse X increment
				x = 0;
				if ((v & 0x001F) == 31) {
					v &= ~0x001F;
					v ^= 0x0400;	// switch horizontal nametable
				} else {
					v++;
				}
			}
		}
		v ^= 0x0400;	// switch horizontal nametable

		// Y increment
		if ((v & 0x7000) != 0x7000) {	// fine Y != 7
			v += 0x1000;
		} else {
			v &= ~0x7000;
			int y = (v & 0x03E0) >> 5;
			if (y == 29) {
				y = 0;
				v ^= 0x0800;		// switch vertical nametable
			} else if (y == 31) {
				y = 0;
			} else {
				y++;
			}
			v = (v & ~0x03E0) | (y << 5);
		}
	}
}


} // namespace nes

} // namespace vfemu
