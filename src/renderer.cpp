#include "renderer.hpp"
#include <stdexcept>
#include <array>
#include <iostream>

namespace engine {

	Renderer::Renderer( Window &windowRef, Device &deviceRef ) : window{windowRef}, activeDevice{deviceRef}
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	Renderer::~Renderer()
	{
		freeCommandBuffers();
	}

	void Renderer::recreateSwapChain(){
		auto extent = window.getExtent();
		while(extent.width == 0 || extent.height == 0){
			extent = window.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(activeDevice.device());
		
		if(swapChain == nullptr){
			swapChain = std::make_unique<SwapChain>(activeDevice, extent);
		}
		else{
            std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
			swapChain = std::make_unique<SwapChain>(activeDevice, extent, oldSwapChain);

            if(!oldSwapChain->compareSwapFormats(*swapChain.get())){
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
		}
	}

	void Renderer::createCommandBuffers()
	{
		commandBuffers.resize( SwapChain::MAX_FRAMES_IN_FLIGHT );

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = activeDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>( commandBuffers.size() );

		if (vkAllocateCommandBuffers( activeDevice.device(), &allocInfo, commandBuffers.data() ) != VK_SUCCESS)
		{
			throw std::runtime_error( "Failed to allocate command buffers!" );
		}
	}

	void Renderer::freeCommandBuffers(){
		vkFreeCommandBuffers(activeDevice.device(), activeDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

    VkCommandBuffer Renderer::beginFrame(){
        assert(!isFrameStarted && "Can't call beginFram while already in progress");

		auto result = swapChain->acquireNextImage( &currentImageIndex );

		//Check to see if anything has changed about the window before submitting command buffer
		//i.e. window resized or minimized
		if( result == VK_ERROR_OUT_OF_DATE_KHR){
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error( "Failed to acquire swap chain image!" );
		}

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer( commandBuffer, &beginInfo ) != VK_SUCCESS)
		{
			throw std::runtime_error( "Failed to begin recording command buffer!" );
		}

        return commandBuffer;
    }

    void Renderer::endFrame(){
        assert(isFrameStarted && "Can't call end frame while frame is not in progress");

        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS)
		{
			throw std::runtime_error( "Failed to record command buffers!" );
		}

        auto result = swapChain->submitCommandBuffers( &commandBuffer, &currentImageIndex );

		if( result == VK_ERROR_OUT_OF_DATE_KHR || VK_SUBOPTIMAL_KHR || window.wasWindowResized()){
			window.resetWindowResizedFlage();
			recreateSwapChain();
		}
        else if (result != VK_SUCCESS)
		{
			throw std::runtime_error( "Failed to present swap chain image!" );
		}

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer){
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass while frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain->getRenderPass();
		renderPassInfo.framebuffer = swapChain->getFrameBuffer( currentImageIndex );
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[ 0 ].color = { 0.01f, 0.01f, 0.01f, 0.1f };
		clearValues[ 1 ].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>( clearValues.size() );
		renderPassInfo.pClearValues = clearValues.data();
		vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
		
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer){
        assert(isFrameStarted && "Can't call endSwapChainRenderPass while frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass( commandBuffer );
    }

}