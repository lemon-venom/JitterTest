#include <thread>

#include "../headers/Entity.hpp"
#include "../headers/Logger.hpp"
#include "../headers/Renderer.hpp"
#include "../headers/Timer.hpp"

Renderer* renderer;
Entity entity;

bool quit = false;


// Turn off limiter, rely on VSync instead.
const uint64_t fpsLimiterMsec_ = 0.0;// (1.0 / 60.0) * 1000;


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

	std::cout << "\rFrame time: " << frameTimeMsec << " msec FPS: " << timer.getFps() << "     " << std::flush;

	// Get the start time now, so we don't lose any time to the overhead tasks between now and when the next tick happens.
	frameStartTimeMsec = timer.tick() / 1000L;
}

void render() {

	double lerp = timer.timeAccumulator / timer.fixedTimeStep;

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

		timer.frameTick();

		double timeDelta = timer.getDeltaTime();

		timer.timeAccumulator += timeDelta;
		//std::cout << timeDelta << std::endl;

		if (timeDelta > 0.016)
		{
			bool breaker = true;
		}

		uint64_t start = timer.tick();
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

		uint64_t end = timer.tick();

		uint64_t duration = end - start;

		double durationMsec = (double)duration / 1000.0;

		//if (durationMsec > 8.0)
		//{
		//	std::cout << "\rMessage pump took " << durationMsec << "ms in frame " << timer.getFrameCount() << std::endl;
		//}

		update();

		render();

		updateEnd();
	}

	shutdown();

	return 0;
}