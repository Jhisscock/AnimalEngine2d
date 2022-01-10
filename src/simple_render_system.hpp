#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "game_object.hpp"

#include <memory>
#include<vector>

namespace engine {
	class SimpleRenderSystem
	{
		public:
		    SimpleRenderSystem( Device &deviceRef, VkRenderPass renderPass);
		    ~SimpleRenderSystem();

		    SimpleRenderSystem( const SimpleRenderSystem& ) = delete;
		    SimpleRenderSystem& operator=( const SimpleRenderSystem& ) = delete;

		    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects);

		private:
		    void createPipelineLayout();
		    void createPipeline(VkRenderPass renderPass);

		    Device &activeDevice;
            
		    std::unique_ptr<Pipeline> pipeline;
		    VkPipelineLayout pipelineLayout;
	};
}