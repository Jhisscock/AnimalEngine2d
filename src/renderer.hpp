#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "game_object.hpp"

#include <memory>
#include<vector>

namespace engine {
	class Renderer
	{
		public:
		    Renderer(Window &windowRef, Device &deviceRef);
		    ~Renderer();

		    Renderer( const Renderer& ) = delete;
		    Renderer& operator=( const Renderer& ) = delete;

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

            bool isFrameInProgress() const { return isFrameStarted; }

            VkCommandBuffer getCurrentCommandBuffer() const {
                assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
                return commandBuffers[currentFrameIndex]; 
            }

            int getFrameIndex() const {
                assert(isFrameStarted && "Cannot get frame index when frame not in progress");
                return currentFrameIndex;
            }

            VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }

		private:
		    void createCommandBuffers();
		    void freeCommandBuffers();
		    void recreateSwapChain();

		    Window &window;
		    Device &activeDevice;
		    std::unique_ptr<SwapChain> swapChain;
		    std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex{0};
            bool isFrameStarted{false};
	};
}