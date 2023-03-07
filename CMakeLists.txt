cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project("plover")

include(GNUInstallDirs)

set(ENV{VULKAN_SDK} $ENV{VULKAN_SDK_PATH})
find_package(Vulkan REQUIRED)
message(STATUS ${PROJECT_NAME})

add_library(${PROJECT_NAME} "src/Renderer.cpp" "src/includes.h" "src/VulkanContext.h" "src/VulkanContext.cpp" "src/VulkanAllocator.h" "src/VulkanAllocator.cpp" src/Mesh.h "src/DescriptorAllocator.h" "src/DescriptorAllocator.cpp" "src/Mesh.cpp")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
	target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

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

install(TARGETS ${PROJECT_NAME}
		EXPORT ${PROJECT_NAME}Targets
		INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")