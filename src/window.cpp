#include "window.hpp"
#include<stdexcept>
namespace engine {
	Window::Window( int w, int h, std::string name ) : width{w}, height{h}, windowName{name}
	{
		initWindow();
	}

	Window::~Window()
	{
		glfwDestroyWindow( window );
		glfwTerminate();
	}

	void Window::initWindow()
	{
		glfwInit();
		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
		glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );

		window = glfwCreateWindow( width, height, windowName.c_str(), nullptr, nullptr );
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizedCallback);
	}

	void Window::createWindowSurface( VkInstance instance, VkSurfaceKHR* surface )
	{
		if (glfwCreateWindowSurface( instance, window, nullptr, surface ) != VK_SUCCESS)
		{
			throw::std::runtime_error( "Failed to create window surface." );
		}
	}

	void Window::frameBufferResizedCallback(GLFWwindow *windowPtr, int width, int height){
		auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(windowPtr));
		window->frameBufferResized = true;
		window->width = width;
		window->height = height;
	}

}

