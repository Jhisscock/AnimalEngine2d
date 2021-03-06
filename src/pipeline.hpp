#pragma once

#include"device.hpp"

#include<string>
#include<vector>

namespace engine {
	struct PipelineConfigInfo
	{
		PipelineConfigInfo( const PipelineConfigInfo& ) = delete;
		PipelineConfigInfo& operator=( const PipelineConfigInfo& ) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline
	{
		public:
		Pipeline( Device &device, const std::string &vertFilepath, const std::string &fragFilepath, const PipelineConfigInfo &configInfo );
		~Pipeline();

		Pipeline( const Pipeline& ) = delete;
		Pipeline& operator=( const Pipeline& ) = delete;
		Pipeline() = default;

		static void defaultPipelineConfigInfo( PipelineConfigInfo& configInfo );

		void bind( VkCommandBuffer commandBuffer );

		private:
		static std::vector<char> readFile( const std::string& filePath );

		void createGraphicsPipeline( const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo );
		void createShaderModule( const std::vector<char>& code, VkShaderModule* shaderModule );

		Device& activeDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}