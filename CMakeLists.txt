cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

project(vulxels)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
	-Wall
	-Wextra
	-Wpedantic
	-Wold-style-cast
)

set(CXX_SOURCE
	src/app.cpp
	src/main.cpp
	src/gfx/device.cpp
	src/gfx/instance.cpp
	src/gfx/renderer.cpp
	src/gfx/shader.cpp
	src/gfx/swapchain.cpp
	src/gfx/window.cpp
)

set(SHADER_SOURCE
	shaders/simple.frag
	shaders/simple.vert
)

add_executable(${PROJECT_NAME} ${CXX_SOURCE})

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)
find_package(EnTT REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${Vulkan_INCLUDE}
	${SDL2_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${EnTT_INCLUDE_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	${Vulkan_LIBRARIES}
	${SDL2_LIBRARIES}
	${GLM_LIBRARIES}
	${EnTT_LIBRARIES}
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
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
