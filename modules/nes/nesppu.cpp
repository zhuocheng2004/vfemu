
#include <iostream>
#include <SDL2/SDL.h>
#include <nes/nesppu.h>


namespace vfemu {

namespace nes {


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
				module->w++;
				break;
			case 0x7:	// PPUDATA
				paddr = module->paddr;
				if ((paddr & 0xE000) == 0x2000) {
					// nametable
					paddr &= 0x7FF;
					module->vram[paddr] = data;
					module->paddr += 1;
				}
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
		(32 * 8) * 2, (30 * 8) * 2, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!window)
		return;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
		return;

	texture = SDL_CreateTexture(renderer, 
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		32 * 8,  30 * 8);
	if (!texture)
		return;

	bool quit = false;
	u32* pixels = new u32[(32 * 8) * (30 * 8)];
	SDL_Event event;
	const unsigned int delta = 1000 / 60;
	Uint32 timer = SDL_GetTicks(), passed = 0;

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
			SDL_UpdateTexture(texture, NULL, pixels, (32 * 8) * 4);
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


void NESPPUModule::fillPixels(NESPPUModule* module, u32* pixels) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			for (int r = 0; r < 8; r++) {
				u8 data = module->getChrData((1 << 12) + (i << 8) + (j << 4) + r);
				for (int c = 0; c < 8; c++) {
					int idx = (8 * i + r) * (32 * 8) + (8 * j + c);
					if (data & 0x80)
						pixels[idx] = 0xFFFFFFFF;
					else
						pixels[idx] = 0x00000000;
					data <<= 1;
				}
			}
		}
	}
}


} // namespace nes

} // namespace vfemu
