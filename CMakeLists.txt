cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project("plover")

include(GNUInstallDirs)

set(Vulkan_LIBRARY $ENV{Vulkan_LIBRARY})
find_package(Vulkan REQUIRED)
message(STATUS ${PROJECT_NAME})

if (WIN32)
	set(WIN32_MAIN WIN32)
	set(MAIN_FILE "src/win32_plover.cpp")
endif()

add_executable(${PROJECT_NAME} ${WIN32_MAIN} ${MAIN_FILE} "src/Renderer.cpp" "src/includes.h" "src/VulkanContext.h" "src/VulkanContext.cpp" "src/VulkanAllocator.h" "src/VulkanAllocator.cpp" src/Mesh.h "src/DescriptorAllocator.h" "src/DescriptorAllocator.cpp" "src/Mesh.cpp" "include/plover/plover.h")

add_subdirectory(libraries/glfw)
add_subdirectory(libraries/glm)
add_subdirectory(libraries/header_libs)

target_include_directories(${PROJECT_NAME}
	PUBLIC include/
	PUBLIC libraries/glfw/include
	PUBLIC libraries/glm/include
	PUBLIC libraries/header_libs/include
)

target_link_directories(${PROJECT_NAME}
	PUBLIC libraries/glfw/src
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC Vulkan::Vulkan
	PUBLIC glfw
	PUBLIC glm
)
