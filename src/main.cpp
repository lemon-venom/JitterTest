#include <thread>

#include "../headers/Entity.hpp"
#include "../headers/Logger.hpp"
#include "../headers/Renderer.hpp"
#include "../headers/Timer.hpp"

Renderer* renderer;
Entity entity;

bool quit = false;


const uint64_t fpsLimiterMsec_ = (1.0 / 60.0) * 1000;
uint64_t frameEndTimeMsec = 0;
uint64_t frameStartTimeMsec = 0;

void handleInput(SDL_Keycode keycode) {

	switch (keycode) {
	case SDLK_ESCAPE:
		quit = true;
		break;
	case SDLK_v:
		renderer->toggleVSync();
		break;
	case SDLK_l:
		logger.toggleLog();
		break;
	case SDLK_s:
		if (entity.started())
		{
			entity.stop();
		}
		else
		{
			entity.start();
		}
		break;
	}
}

bool setup() {

	renderer = new Renderer();

	return renderer->initialize();
}

void shutdown()
{
	renderer->shutdown();

	delete renderer;

	SDL_Quit();
}

void update() {

	if (timer.timeAccumulator >= timer.fixedTimeStep)
	{
		entity.updateBegin();

		while (timer.timeAccumulator >= timer.fixedTimeStep)
		{
			entity.update(timer.fixedTimeStep);

			timer.timeAccumulator -= timer.fixedTimeStep;
		}
	}
}

void updateEnd() {

	frameEndTimeMsec = timer.tick() / 1000L;

	uint64_t frameTimeMsec = frameEndTimeMsec - frameStartTimeMsec;

	if (fpsLimiterMsec_ > 0)
	{
		uint64_t frameTimeTargetMsec = fpsLimiterMsec_;

		uint64_t sleepTimeMsec = frameTimeTargetMsec - frameTimeMsec;

		if (frameTimeTargetMsec > frameTimeMsec)
		{
			// Consume the remaining time to fix it to the limiter.
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMsec));
		}
	}

	// Get the start time now, so we don't lose any time to the overhead tasks between now and when the next tick happens.
	frameStartTimeMsec = timer.tick() / 1000L;
}

void render() {

	float lerp = timer.timeAccumulator / timer.fixedTimeStep;

	renderer->sceneBegin();

	entity.render(renderer, lerp);

	renderer->sceneComplete();
}

int main(int argc, char* args[]) {

	if (setup() == false) {		
		return 0;
	}

	SDL_Event event;

	timer.start();

	while (quit == false) {

		while (SDL_PollEvent(&event)) {

			switch (event.type)	{
			case SDL_QUIT: {
				quit = true;
				break;
			}
			case SDL_KEYDOWN: {
				handleInput(event.key.keysym.sym);
				break;
			}
			default:
				break;
			}
		}

		timer.frameTick();

		timer.timeAccumulator += timer.getDeltaTime();

		update();

		render();

		updateEnd();
	}

	shutdown();

	return 0;
}