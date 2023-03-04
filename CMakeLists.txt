cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project("plover")

include(GNUInstallDirs)

set(ENV{VULKAN_SDK} $ENV{VULKAN_SDK_PATH})
find_package(Vulkan REQUIRED)
message(STATUS ${PROJECT_NAME})

add_library(${PROJECT_NAME} "src/Renderer.cpp" "src/includes.h" "src/VulkanContext.h" "src/VulkanContext.cpp" "src/VulkanAllocator.h" "src/VulkanAllocator.cpp")

add_subdirectory(libraries/glfw)
add_subdirectory(libraries/glm)

target_include_directories(${PROJECT_NAME}
	PUBLIC include/
	PUBLIC libraries/glfw/include
	PUBLIC libraries/glm/include
)

target_link_directories(${PROJECT_NAME}
	PUBLIC libraries/glfw/src
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC Vulkan::Vulkan
	PUBLIC glfw
	PUBLIC glm
)

install(TARGETS ${PROJECT_NAME}
		EXPORT ${PROJECT_NAME}Targets
		INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")