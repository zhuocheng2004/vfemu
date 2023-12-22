
#include <iostream>
#include <SDL2/SDL.h>
#include <nes/nesppu.h>


namespace vfemu {

namespace nes {

using namespace std::chrono_literals;

#define ROWS		30
#define COLUMNS		32
#define PITCH		COLUMNS * 8

static void updateRenderRect(SDL_Rect* rect, int screenWidth, int screenHeight) {
	static const float RATIO = (float) ROWS / COLUMNS;
	if ((float) screenHeight / screenWidth >= RATIO) {
		rect->x = 0;
		rect->w = screenWidth;
		rect->h = (int) (screenWidth * RATIO);
		rect->y = (int) ((screenHeight - rect->h) / 2);
	} else {
		rect->y = 0;
		rect->h = screenHeight;
		rect->w = (int) (screenHeight / RATIO);
		rect->x = (int) ((screenWidth - rect->w) / 2);
	}
}


void NESPPUModule::render_thread_func(NESPPUModule* module) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return;

	SDL_Window* 	window;
	SDL_Renderer*	renderer;
	SDL_Texture*	texture;
	SDL_Rect		dstRect;
	u32 width, height;

	static const int INIT_WIDTH = (COLUMNS * 8) * 2;
	static const int INIT_HEIGHT = (ROWS * 8) * 2;

	window = SDL_CreateWindow("NES Toy Simulator",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		INIT_WIDTH, INIT_HEIGHT, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!window)
		return;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
		return;

	texture = SDL_CreateTexture(renderer, 
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		8 * COLUMNS,  8 * ROWS);
	if (!texture)
		return;

	updateRenderRect(&dstRect, INIT_WIDTH, INIT_HEIGHT);

	bool quit = false;
	u32* pixels = new u32[(8 * COLUMNS) * (8 * ROWS)];
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
						updateRenderRect(&dstRect, width, height);
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
			if (module->nmi) {
				module->sendToPort(IDX_INT, 0x1);	// tell CPU
			}

			SDL_UpdateTexture(texture, NULL, pixels, PITCH * 4);
			/*  ========  */

			SDL_SetRenderTarget(renderer, NULL);
			SDL_RenderCopy(renderer, texture, NULL, &dstRect);
		}
		
		SDL_RenderFlush(renderer);

		SDL_RenderPresent(renderer);
		SDL_UpdateWindowSurface(window);
	}


	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	module->sendToPort(IDX_EXIT, 1);

	delete pixels;
}


} // namespace nes

} // namespace vfemu
