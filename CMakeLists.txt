cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)

project(Pixelizer LANGUAGES C CXX)

set(CMAKE_BUILD_PARALLEL_LEVEL)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Pixelizer-Lib STATIC Source/Pixelizer.cpp)

add_subdirectory(Source)
target_include_directories(Pixelizer-Lib PUBLIC Source)
target_include_directories(Pixelizer-Lib PUBLIC ThirdParty)
target_include_directories(Pixelizer-Lib PUBLIC Shaders)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)
include_directories(${Vulkan_INCLUDE_DIRS})
link_libraries(${Vulkan_LIBRARIES})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(ThirdParty/GLFW)
target_link_libraries(Pixelizer-Lib PUBLIC glfw)

add_subdirectory(ThirdParty/imgui)
target_include_directories(Pixelizer-Lib PUBLIC ThirdParty/imgui)

add_subdirectory(ThirdParty/GLM)
target_link_libraries(Pixelizer-Lib PUBLIC glm::glm)

add_subdirectory(ThirdParty/VMA)
target_link_libraries(Pixelizer-Lib PUBLIC GPUOpen::VulkanMemoryAllocator)

add_executable(Pixelizer main.cpp)
target_link_libraries(Pixelizer Pixelizer-Lib)
set_target_properties(Pixelizer PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
file(GLOB_RECURSE SHADERS
    ${SHADER_SOURCE_DIR}/*.vert
    ${SHADER_SOURCE_DIR}/*.frag
    ${SHADER_SOURCE_DIR}/*.rchit
    ${SHADER_SOURCE_DIR}/*.rahit
    ${SHADER_SOURCE_DIR}/*.rgen
    ${SHADER_SOURCE_DIR}/*.rmiss
    ${SHADER_SOURCE_DIR}/*.comp
    ${SHADER_SOURCE_DIR}/*.rint)

add_custom_command(
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
    OUTPUT ${SHADER_BINARY_DIR}
    COMMENT "Creating ${SHADER_BINARY_DIR}"
)

foreach (source IN LISTS SHADERS)
	get_filename_component(FILENAME ${source} NAME)
	add_custom_command(
        COMMAND ${glslc_executable} -o ${SHADER_BINARY_DIR}/${FILENAME}.spv ${source} --target-env=vulkan1.3
        OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
        DEPENDS ${source} ${SHADER_BINARY_DIR}
        COMMENT "Compiling ${FILENAME}"
	)
	list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach ()

add_custom_target(SHADERS ALL DEPENDS ${SPV_SHADERS})
add_dependencies(Pixelizer SHADERS)

add_subdirectory(ThirdParty/GTest)

include(GoogleTest)
enable_testing()

if (BUILD_TESTING)
	file(GLOB_RECURSE SOURCES
			${CMAKE_SOURCE_DIR}/Source/*.cpp
			${CMAKE_SOURCE_DIR}/Source/*.h)

	add_executable(Pixelizer-Tests ${SOURCES})
	target_link_libraries(Pixelizer-Tests PRIVATE Pixelizer-Lib)
	target_link_libraries(Pixelizer-Tests PRIVATE GTest::gtest_main)
	target_link_libraries(Pixelizer-Tests PRIVATE GTest::gmock)
	target_include_directories(Pixelizer-Tests PRIVATE ${CMAKE_SOURCE_DIR}/Tests)
	add_test(NAME Pixelizer-Tests COMMAND Pixelizer-Tests WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif ()

add_subdirectory(Tests)