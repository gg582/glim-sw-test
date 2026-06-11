cmake_minimum_required(VERSION 3.14)
project(ImageProcessor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable AVX2 if supported (for SIMD acceleration)
include(CheckCXXCompilerFlag)
if(MSVC)
    check_cxx_compiler_flag("/arch:AVX2" COMPILER_SUPPORTS_AVX2)
    if(COMPILER_SUPPORTS_AVX2)
        add_compile_options(/arch:AVX2)
    endif()
else()
    check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
    if(COMPILER_SUPPORTS_AVX2)
        add_compile_options(-mavx2)
    endif()
endif()

set(SOURCES
    src/main.cpp
    src/BmpParser.cpp
    src/ImageBuffer.cpp
    src/CommandLineParser.cpp
    src/Logger.cpp
    src/FilterPipelineBuilder.cpp
    src/utils/ConvolutionEngine.cpp
    src/utils/HistogramAnalyzer.cpp
    src/filters/GrayscaleFilter.cpp
    src/filters/ThresholdFilter.cpp
    src/filters/BlurFilter.cpp
    src/filters/SharpenFilter.cpp
    src/filters/FlipFilter.cpp
    src/filters/CropFilter.cpp
    src/filters/ResizeFilter.cpp
)

set(HEADERS
    src/BmpParser.h
    src/ImageBuffer.h
    src/CommandLineParser.h
    src/Exceptions.h
    src/FilterBase.h
    src/FilterPipelineBuilder.h
    src/Logger.h
    src/utils/ConvolutionEngine.h
    src/utils/HistogramAnalyzer.h
    src/filters/GrayscaleFilter.h
    src/filters/ThresholdFilter.h
    src/filters/BlurFilter.h
    src/filters/SharpenFilter.h
    src/filters/FlipFilter.h
    src/filters/CropFilter.h
    src/filters/ResizeFilter.h
)

add_executable(ImageProcessor ${SOURCES} ${HEADERS})

target_include_directories(ImageProcessor PRIVATE src)

find_package(Threads REQUIRED)
target_link_libraries(ImageProcessor PRIVATE Threads::Threads)
