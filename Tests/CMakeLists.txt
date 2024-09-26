target_sources(Pixelizer-Tests PRIVATE
        main.cpp
        Environment.cpp
        TestLogger.cpp
        TestUtils.cpp)

add_subdirectory(Mocks)
add_subdirectory(UnitTests)
