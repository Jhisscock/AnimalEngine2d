#pragma once

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "game_object.hpp"

#include <memory>
#include<vector>

namespace engine {
	class Application
	{
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HEIGHT = 600;

			Application();
			~Application();

			Application( const Application& ) = delete;
			Application& operator=( const Application& ) = delete;
			void run();

		private:
			void loadGameObjects();
	
			Window window{ WIDTH,HEIGHT, "Animal Engine" };
			Device activeDevice{ window };
			Renderer renderer{window, activeDevice};
	
			std::vector<GameObject> gameObjects;
	};
}