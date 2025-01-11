cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

project(vulxels)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
include(FetchContent)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG 22422f7748d5128135995ed34c8f8012861c7332 # preview-3.1.8
	FIND_PACKAGE_ARGS NAMES SDL3
)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863 # 1.0.1
	FIND_PACKAGE_ARGS NAMES glm
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
	EnTT
	GIT_REPOSITORY https://github.com/skypjack/entt.git
	GIT_TAG 4a2d1a8541228a90e02a873dba6a980506c42c03 # v3.14.0
	FIND_PACKAGE_ARGS NAMES EnTT
)
FetchContent_MakeAvailable(EnTT)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG 947aa9c9722bd6ff740dd757da609ff41f4d3ba3 # v1.91.6-docking
)
FetchContent_MakeAvailable(imgui)

add_library(
	imgui STATIC
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

target_link_libraries(
	imgui PRIVATE
	Vulkan::Vulkan
	SDL3::SDL3
)

if (MSVC)
	add_compile_options(
		/W4
		/permissive-
	)
else ()
	add_compile_options(
		-Wall
		-Wextra
		-Wpedantic
	)
endif ()

set(CXX_SOURCE
	src/app.cpp
	src/main.cpp
	src/gfx/buffer.cpp
	src/gfx/descriptors.cpp
	src/gfx/device.cpp
	src/gfx/instance.cpp
	src/gfx/pipeline.cpp
	src/gfx/queue.cpp
	src/gfx/renderer.cpp
	src/gfx/shader.cpp
	src/gfx/swapchain.cpp
	src/gfx/window.cpp
)

set(SHADER_SOURCE
	shaders/simple.frag
	shaders/simple.vert
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${Vulkan_INCLUDE_DIRS}
	${SDL3_INCLUDE_DIRS}
	${glm_INCLUDE_DIRS}
	${EnTT_INCLUDE_DIRS}
	${imgui_SOURCE_DIR}
)

add_executable(${PROJECT_NAME} ${CXX_SOURCE})

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	Vulkan::Vulkan
	SDL3::SDL3
	glm::glm
	EnTT::EnTT
	imgui
)

if (CMAKE_IMPORT_LIBRARY_SUFFIX)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND_EXPAND_LISTS
	)
endif ()

find_program(GLSLC glslc REQUIRED HINTS Vulkan::glslc)

foreach(GLSL ${SHADER_SOURCE})
	get_filename_component(FILE_NAME ${GLSL} NAME)
	set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.spv")
	add_custom_command(
		OUTPUT ${SPIRV}
		COMMAND ${GLSLC} ${CMAKE_CURRENT_SOURCE_DIR}/${GLSL} -o ${SPIRV}
		DEPENDS ${GLSL}
	)
	list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
	shaders ALL
	DEPENDS ${SPIRV_BINARY_FILES}
)

add_custom_target(run
	COMMAND ${PROJECT_NAME}
	DEPENDS ${PROJECT_NAME}
	DEPENDS shaders
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
