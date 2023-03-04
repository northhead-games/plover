#pragma once

#include "VulkanAllocator.h"
#include "includes.h"

namespace Plover {
	const int MAX_FRAMES_IN_FLIGHT = 2;
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

    const std::string MODEL_PATH = "resources/models/viking_room.obj";
    const std::string TEXTURE_PATH = "resources/textures/viking_room.png";

	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

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

	struct VulkanContext {
		const std::vector<Vertex> vertices_ = {
			{{ -0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{  0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{  0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{ -0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{ -0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{  0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{  0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{ -0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		};

		const std::vector<uint16_t> indices_ = {
				0, 1, 2, 2, 3, 0,
				4, 5, 6, 6, 7, 4,
		};

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

		VkInstance instance;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		VkDevice device;

		VulkanAllocator allocator;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkSurfaceKHR surface;

		VkSwapchainKHR swapChain;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;

		VkRenderPass renderPass;

		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;

		VkPipeline graphicsPipeline;

		VkCommandPool drawCommandPool;
		VkCommandPool transientCommandPool;

		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
		VkBuffer vertexBuffer;
		Allocation vertexBufferAllocation;
		VkBuffer indexBuffer;
		Allocation indexBufferAllocation;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<Allocation> uniformBuffersAllocation;
		std::vector<void*> uniformBuffersMapped;

		VkImage textureImage;
		Allocation textureAllocation;
		VkImageView textureImageView;
		VkSampler textureSampler;

		VkImage depthImage;
		Allocation depthImageAllocation;
		VkImageView depthImageView;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		bool framebufferResized = false;

		uint32_t currentFrame = 0;

		VkDebugUtilsMessengerEXT debugMessenger;

		GLFWwindow* window;

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		std::vector<const char*> getRequiredExtensions();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);

		void initWindow();

		void initVulkan();

		void mainLoop();

		void cleanup();

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

		void initAllocator();

		void createSurface();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void createSwapChain();

		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		void createImageViews();

		void createRenderPass();

		void createDescriptorSetLayout();

		VkShaderModule createShaderModule(const std::vector<char>& code);

		void createGraphicsPipeline();

		void createFramebuffers();

		void createCommandPool(VkCommandPoolCreateFlagBits, VkCommandPool& commandPool);

		void createCommandPools();

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void createTextureImage();

		void createTextureImageView();

		void createTextureSampler();

		VkFormat findSupportedFormats(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		VkFormat findDepthFormat();

		void createDepthResources();

		VkCommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void loadModel();

		void createVertexBuffer();

		void createIndexBuffer();

		void createUniformBuffers();

		void createDescriptorPool();

		void createDescriptorSets();

		void createCommandBuffer();

		void createSyncObjects();

		void recordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex);

		void updateUniformBuffer(uint32_t currentImage);

		void drawFrame();

		void recreateSwapChain();

		void cleanupSwapChain();
	};
}