#pragma once

#include "includes.h"

namespace Plover {
	const int MAX_FRAMES_IN_FLIGHT = 2;
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

    struct CreateBufferInfo {
        VkDeviceSize size;
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties;
    };

	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanContext {
	public:
		void initWindow();

		void initVulkan();

		void mainLoop();

		void cleanup();

	private:
		const std::vector<Vertex> vertices = {
			{{ -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
            {{ -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
		};

        const std::vector<uint16_t> indices = {
                0, 1, 2, 2, 3, 0
        };

		VkInstance instance;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		VkDevice device;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkSurfaceKHR surface;

		// Swap Chain
		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		VkRenderPass renderPass;
		std::vector<VkImageView> swapChainImageViews;

		VkPipelineLayout pipelineLayout;

		VkPipeline graphicsPipeline;

		std::vector<VkFramebuffer> swapChainFramebuffers;

		VkCommandPool drawCommandPool;
        VkCommandPool copyCommandPool;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		bool framebufferResized = false;

		uint32_t currentFrame = 0;

		VkDebugUtilsMessengerEXT debugMessenger;

		const std::vector<const char*> validationLayers = {
				"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
#ifdef __APPLE__
				,"VK_KHR_portability_subset"
#endif
		};

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		GLFWwindow* window;

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		std::vector<const char*> getRequiredExtensions();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);
		void createInstance();

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void setupDebugMessenger();

		bool checkValidationLayerSupport();

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice currentDevice);

		bool checkDeviceExtensionSupport(VkPhysicalDevice currentDevice);

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		uint32_t rateDeviceSuitability(VkPhysicalDevice currentDevice);

		void pickPhysicalDevice();

		void createLogicalDevice();

		void createSurface();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void createSwapChain();

		void createImageViews();

		void createRenderPass();

		VkShaderModule createShaderModule(const std::vector<char>& code);

		void createGraphicsPipeline();

		void createFramebuffers();

		void createCommandPool(VkCommandPoolCreateFlagBits, VkCommandPool &commandPool);

        void createCommandPools();

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

		void createVertexBuffer();

        void createIndexBuffer();

		void createCommandBuffer();

		void createSyncObjects();

		void recordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex);

		void drawFrame();

		void recreateSwapChain();

		void cleanupSwapChain();
	};
}