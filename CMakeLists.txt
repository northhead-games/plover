cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project("plover")

include(GNUInstallDirs)

if (WIN32)
        message(STATUS ">>> WIN32")
	set(WIN32_MAIN WIN32)
	set(MAIN_FILE "src/win32_plover.cpp")
endif()

if (LINUX)
        message(STATUS ">>> LINUX")
        set(MAIN_FILE "src/linux_plover.cpp")
endif()

set(Vulkan_LIBRARY $ENV{Vulkan_LIBRARY})
set(FREETYPE_DIR $ENV{Freetype_DIR})
find_package(Vulkan REQUIRED)
find_package(Freetype REQUIRED)
message(STATUS ${PROJECT_NAME})
message(STATUS ${FREETYPE_LIBRARIES})

add_executable(${PROJECT_NAME} ${WIN32_MAIN} ${MAIN_FILE} "src/Renderer.cpp" "src/includes.h" "src/VulkanContext.h" "src/VulkanContext.cpp" "src/DescriptorAllocator.h" "src/DescriptorAllocator.cpp" "src/Mesh.h" "src/Mesh.cpp" "src/Material.h" "src/Material.cpp" "include/plover/plover.h" "src/ttfRenderer.cpp" "src/ttfRenderer.h" "src/Texture.h" "src/Texture.cpp" "src/plover_int.h" "src/UI.h" "src/UI.cpp" "src/AssetLoader.h" "src/AssetLoader.cpp")

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "C:/NorthheadDeps/lib/freetype.dll" ${CMAKE_CURRENT_BINARY_DIR}/.
    )
endif ()

add_subdirectory(libraries/glfw)
add_subdirectory(libraries/glm)
add_subdirectory(libraries/header_libs)
add_subdirectory("lapwing")

target_include_directories(${PROJECT_NAME}
	PUBLIC include/
	PUBLIC lapwing/include
	PUBLIC ${Vulkan_INCLUDE_DIRS}
	PUBLIC libraries/glfw/include
	PUBLIC libraries/glm/include
	PUBLIC libraries/header_libs/include
	PUBLIC ${FREETYPE_INCLUDE_DIRS}
)

target_link_directories(${PROJECT_NAME}
	PUBLIC libraries/glfw/src
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC Vulkan::Vulkan
	PUBLIC glfw
	PUBLIC glm
	PUBLIC ${FREETYPE_LIBRARIES}
)
