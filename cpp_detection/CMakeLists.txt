cmake_minimum_required(VERSION 3.0.0)
project(detection_test)

option(ONNXRUNTIME_DIR "Path to built ONNX Runtime directory." STRING)
message(STATUS "ONNXRUNTIME_DIR: ${ONNXRUNTIME_DIR}")

find_package(OpenCV REQUIRED PATHS "../build")

include_directories("include/")

# Add executable for image inference
add_executable(detection_test
               detection_test.cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set include directories for all executables
target_include_directories(detection_test PRIVATE "${ONNXRUNTIME_DIR}/include")

# Set compile features for all executables
target_compile_features(detection_test PRIVATE cxx_std_14)

# Link libraries for all executables
target_link_libraries(detection_test ${OpenCV_LIBS})


message(STATUS "We are building on Linux!")
target_link_libraries(detection_test "${ONNXRUNTIME_DIR}/lib/libonnxruntime.so")
