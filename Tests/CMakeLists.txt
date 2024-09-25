include(CTest)
include(GoogleTest)
enable_testing()

file(GLOB_RECURSE SOURCES
        Source/*.cpp
        Source/*.h)

add_executable(Pixelizer-Tests ${SOURCES})
target_link_libraries(Pixelizer-Tests PRIVATE Pixelizer-Lib)
target_link_libraries(Pixelizer-Tests PRIVATE GTest::gtest_main)
target_link_libraries(Pixelizer-Tests PRIVATE GTest::gmock)
target_include_directories(Pixelizer-Tests PRIVATE Tests)
add_test(NAME Pixelizer-Tests COMMAND Pixelizer-Tests WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_sources(Pixelizer-Tests PRIVATE
        main.cpp
        UnitTests/DummyUnitTest.cpp)
