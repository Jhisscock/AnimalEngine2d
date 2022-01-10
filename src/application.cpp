#include "application.hpp"
#include "simple_render_system.hpp"
#include "../examples/gravity_2d.hpp"
#include <stdexcept>
#include <array>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace engine {

	std::unique_ptr<Model> createSquareModel(Device& device, glm::vec2 offset) {
  		std::vector<Model::Vertex> vertices = {
  		    {{-0.5f, -0.5f}},
  		    {{0.5f, 0.5f}},
  		    {{-0.5f, 0.5f}},
  		    {{-0.5f, -0.5f}},
  		    {{0.5f, -0.5f}},
  		    {{0.5f, 0.5f}},  //
  		};

  		for (auto& v : vertices) {
  		  v.position += offset;
  		}
  		return std::make_unique<Model>(device, vertices);
	}
 
	std::unique_ptr<Model> createCircleModel(Device& device, unsigned int numSides) {
  		std::vector<Model::Vertex> uniqueVertices{};
		
  		for (int i = 0; i < numSides; i++) {
  		  float angle = i * glm::two_pi<float>() / numSides;
  		  uniqueVertices.push_back({{glm::cos(angle), glm::sin(angle)}});
  		}
  		uniqueVertices.push_back({});  // adds center vertex at 0, 0

  		std::vector<Model::Vertex> vertices{};
  		for (int i = 0; i < numSides; i++) {
  		  vertices.push_back(uniqueVertices[i]);
  		  vertices.push_back(uniqueVertices[(i + 1) % numSides]);
  		  vertices.push_back(uniqueVertices[numSides]);
  		}
		
  		return std::make_unique<Model>(device, vertices);
	}

	Application::Application()
	{
		loadGameObjects();
	}

	Application::~Application(){}

	void Application::run()
	{
		std::shared_ptr<Model> sqaureModel = createSquareModel(activeDevice, {0.5f, 0.0f}); //offset model by .5 so rotation occurs at edge rather than center of square
		std::shared_ptr<Model> circleModel = createCircleModel(activeDevice, 64);

		//create physics objects
		std::vector<GameObject> physicsObjects{};
		auto red = GameObject::createGameObject();
		red.transform2d.scale = glm::vec2{0.05f};
		red.transform2d.translation = {0.5f, 0.5f};
		red.color = {1.0f, 0.0f, 0.0f};
		red.rigidBody2d.velocity = {-0.5f, 0.0f};
		red.model = circleModel;
		physicsObjects.push_back(std::move(red));

		auto blue = GameObject::createGameObject();
		blue.transform2d.scale = glm::vec2{0.05f};
		blue.transform2d.translation = {-0.5f, -0.5f};
		blue.color = {0.0f, 0.0f, 1.0f};
		blue.rigidBody2d.velocity = {0.5f, 0.0f};
		blue.model = circleModel;
		physicsObjects.push_back(std::move(blue));

		auto green = GameObject::createGameObject();
		green.transform2d.scale = glm::vec2{0.05f};
		green.transform2d.translation = {0.0f, 0.0f};
		green.color = {0.0f, 1.0f, 0.0f};
		green.rigidBody2d.velocity = {0.0f, 0.0f};
		green.model = circleModel;
		physicsObjects.push_back(std::move(green));

		std::vector<GameObject> vectorField{};
		int gridCount = 40;
		for(int i = 0; i < gridCount; i++){
			for(int j = 0; j < gridCount; j++){
				auto vf = GameObject::createGameObject();
				vf.transform2d.scale = glm::vec2(0.005f);
				vf.transform2d.translation = {
					-1.0f + (i + 0.5f) * 2.0f / gridCount,
					-1.0f + (j + 0.5f) * 2.0f / gridCount
				};
				vf.color = glm::vec3(1.0f);
				vf.model = sqaureModel;
				vectorField.push_back(std::move(vf));
			}
		}

		GravityPhysicsSystem gravitySystem{0.2f};
		Vec2FieldSystem vecFieldSystem{};

		SimpleRenderSystem simpleRenderSystem{activeDevice, renderer.getSwapChainRenderPass()};

		while (!window.shouldClose())
		{
			glfwPollEvents();

			if(auto commandBuffer = renderer.beginFrame()){

				//update systems
				gravitySystem.update(physicsObjects, 1.0f / 60, 5);
				vecFieldSystem.update(gravitySystem, physicsObjects, vectorField);

				//render
				renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, physicsObjects);
				simpleRenderSystem.renderGameObjects(commandBuffer, vectorField);
				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}
	}

	void Application::loadGameObjects()
	{
		std::vector<Model::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		auto model = std::make_shared<Model>( activeDevice, vertices );

    	auto triangle = GameObject::createGameObject();
    	triangle.model = model;
    	triangle.color = {1.0f, 0.8f, 1.0f};
    	triangle.transform2d.translation.x = 0.2f;
    	triangle.transform2d.scale = {2.0f, 0.5f};
    	triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();
		
    	gameObjects.push_back(std::move(triangle));
	}
}