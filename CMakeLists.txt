cmake_minimum_required(VERSION 3.8)

project("plover")

set(ENV{VULKAN_SDK} $ENV{VULKAN_SDK_PATH})
find_package(Vulkan REQUIRED)

add_library(${PROJECT_NAME} "src/Renderer.cpp")

find_package(Vulkan REQUIRED)
message(STATUS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE include/
	PRIVATE src/
	PRIVATE ${Vulkan_INCLUDE_DIRS}
	PRIVATE libraries/glfw/include
	PRIVATE libraries/glm/include
)

target_link_directories(${PROJECT_NAME}
	PRIVATE libraries/glfw/src
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${Vulkan_LIBRARIES}
	PRIVATE glfw
	PRIVATE glm
)