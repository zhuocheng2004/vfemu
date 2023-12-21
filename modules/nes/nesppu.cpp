
#include <iostream>
#include <thread>
#include <SDL2/SDL.h>
#include <nes/nesppu.h>


namespace vfemu {

namespace nes {

using namespace std::chrono_literals;


#define ROWS	30
#define COLUMNS	32


Status NESPPUModule::init() {
	vram = new u8[0x800];

	render_thread = new std::thread(render_thread_func, this);
	return Status::SUCCESS;
}

Status NESPPUModule::exit() {
	running = false;

	delete vram;
	return Status::SUCCESS;
}


Status NESPPUModule::clock_receive(Module* receiver, u64 signal) {
	return Status::SUCCESS;
}

Status NESPPUModule::reset_receive(Module* receiver, u64 signal) {
	auto module = (NESPPUModule*) receiver;
	module->running = false;

	module->vblank = true;
	module->nmi = false;

	module->w = 0;

	module->baseNameTable = 0x2000;
	module->bgPTable = 0x0000;

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
				paddr = module->paddr;
				//printf("  { PPU: want to read data @%04X }  ", paddr);
				if ((paddr & 0xE000) == 0) {
					// CHR ROM
					data = module->getChrData(paddr);
					module->sendToPort(IDX_DATA, data);
					module->paddr += 1;
				}
				break;
		}
	} else {	// write
		data = module->data;
		switch (module->addr & 0xF) {
			case 0x0:	// PPUCTRL
				switch (data & 0x3) {
					// choose base nametable addr
					case 0x0:
						module->baseNameTable = 0x2000;
						break;
					case 0x1:
						module->baseNameTable = 0x2400;
						break;
					case 0x2:
						module->baseNameTable = 0x2800;
						break;
					case 0x3:
						module->baseNameTable = 0x2C00;
						break;
				}
				module->spPTable = (data & MCTRL_S) ? 0x1000 : 0x0000;
				module->bgPTable = (data & MCTRL_B) ? 0x1000 : 0x0000;
				module->nmi = (data & MCTRL_V) ? true : false;
				break;
			case 0x6:	// PPUADDR
				paddr = module->paddr;
				if (module->w & 0x1 /* 2nd: low */) {
					paddr &= 0xFF00;
					paddr |= data;
					module->paddr = paddr;
				} else { /* 1st: high */
					paddr &= 0x00FF;
					paddr |= ((u16) data << 8);
					module->paddr = paddr;
				}
				//printf("  { PPU set addr to %04X }  ", paddr);
				module->w++;
				break;
			case 0x7:	// PPUDATA
				paddr = module->paddr;
				if ((paddr & 0xF000) == 0x2000 || 
					((paddr & 0xF000) == 0x3000 && (paddr < 0x3F00))) {
					// nametable
					paddr &= 0x7FF;
					module->vram[paddr] = data;
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

				module->paddr += 1;
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


void NESPPUModule::render_thread_func(NESPPUModule* module) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return;

	SDL_Window* 	window;
	SDL_Renderer*	renderer;
	SDL_Texture*	texture;
	u32 width, height;

	window = SDL_CreateWindow("NES Toy Simulator",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		(COLUMNS * 8) * 2, (ROWS * 8) * 2, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!window)
		return;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
		return;

	texture = SDL_CreateTexture(renderer, 
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		COLUMNS * 8,  ROWS * 8);
	if (!texture)
		return;

	bool quit = false;
	u32* pixels = new u32[(COLUMNS * 8) * (ROWS * 8)];
	SDL_Event event;
	const unsigned int delta = 1000 / 60;
	Uint32 timer = SDL_GetTicks(), passed = 0;

	std::this_thread::sleep_for(200ms);	// delibrate delay

	while (!quit) {
		// limit FPS
		passed = SDL_GetTicks() - timer;
		while (passed < delta) {
			SDL_Delay(delta - passed);
			passed = SDL_GetTicks() - timer;
		}
		timer = SDL_GetTicks();
	
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT) {
			quit = true;
			break;
		}
		switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						width = event.window.data1;
						height = event.window.data2;
						break;
				}
				break;
		}

		if (module->running) {
			SDL_SetRenderTarget(renderer, texture);
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
			SDL_RenderClear(renderer);

			/*  ==== Main Part ====  */
			module->vblank = false;
			fillPixels(module, pixels);
			module->vblank = true;
			module->sendToPort(IDX_INT, 0x1);	// tell CPU
			SDL_UpdateTexture(texture, NULL, pixels, (COLUMNS * 8) * 4);
			/*  ========  */

			SDL_SetRenderTarget(renderer, NULL);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderFlush(renderer);

			SDL_RenderPresent(renderer);
			SDL_UpdateWindowSurface(window);
		}
	}


	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	delete pixels;
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

	0xFFFFFFFF, 0x6DB6FFFF, 0x9191FFFF, 0xB66DFFFF,
	0xFF00FFFF, 0xFF6D91FF, 0xFF9100FF, 0xFFB600FF,
	0xB6B600FF, 0x6DB600FF, 0x00FF00FF, 0x48FFDAFF,
	0x00FFFFFF, 0x000000FF, 0x000000FF, 0x000000FF,

	0xFFFFFFFF, 0xB6DAFFFF, 0xDAB6FFFF, 0xFFB6FFFF,
	0xFF91FFFF, 0xFFB6B6FF, 0xFFDA91FF, 0xFFFF48FF,
	0xFFFF6DFF, 0xB6FF48FF, 0x91FF6DFF, 0x48FFDAFF,
	0x91DAFFFF, 0x000000FF, 0x000000FF, 0x000000FF,
};


void NESPPUModule::fillPixels(NESPPUModule* module, u32* pixels) {
	u16 ntBase = module->baseNameTable & 0x7FF;
	u16 bgPTable = module->bgPTable;
	u8*	vram = module->vram;
	static u8* bgChrCache = new u8[0x1000];

	// refresh CHR ROM Cache
	// in the future, we should save each of them as texture
	for (int i = 0; i < ((16*16)<<4); i += (1<<4)) {
		for (int l = 0; l < 8; l++) {
			bgChrCache[i + (0 << 3) + l] = module->getChrData(bgPTable + i + (0 << 3) + l);
			bgChrCache[i + (1 << 3) + l] = module->getChrData(bgPTable + i + (1 << 3) + l);
		}
	}

	// build palette
	static u32 bgPalettes[4][4];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) 
			bgPalettes[i][j] = paletteRGBs[module->bgPalettes[i][j]];
	}

	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLUMNS; col++) {
			u8 tileIdx = vram[ntBase + row * COLUMNS + col];
			u8 addrData = vram[ntBase + 0x03C0 + (row >> 2) * (COLUMNS / 4) + (col >> 2)];
			u8 addrIdx;
			if (row & 0x1) {
				if (col & 0x1)
					addrIdx = (addrData >> 6) & 0x3;
				else
					addrIdx = (addrData >> 4) & 0x3;
			} else {
				if (col & 0x1)
					addrIdx = (addrData >> 2) & 0x3;
				else
					addrIdx = (addrData >> 0) & 0x3;
			}
			for (int l = 0; l < 8; l++) {
				u8 data1 = bgChrCache[(tileIdx << 4) + (0 << 3) + l];
				u8 data2 = bgChrCache[(tileIdx << 4) + (1 << 3) + l];
				u8 plIdx = 0;
				int idx = (8 * row + l) * (COLUMNS * 8) + (8 * col);
				for (int c = 7; c >= 0; c--) {
					plIdx = (data2 >> c) & 0x1;
					plIdx <<= 1;
					plIdx |= (data1 >> c) & 0x1;
					pixels[idx] = bgPalettes[addrIdx][plIdx];
					idx++;
				}
			}
		}
	}
}


} // namespace nes

} // namespace vfemu
