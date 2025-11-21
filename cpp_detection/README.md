# Requisitos

- Compilador C++ compatível com C++14
- CMake 3.0.0 ou mais recente
- OpenCV 4.5.5 ou mais recente
- ONNX Runtime

### CMake
`sudo apt install cmake`

### OpenCV

```
# Install minimal prerequisites (Ubuntu 18.04 as reference)
sudo apt update && sudo apt install -y cmake g++ wget unzip
 
# Download and unpack sources
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.12.0.zip
unzip opencv.zip
 
# Create build directory
mkdir -p build && cd build
 
# Configure
cmake  ../opencv-4.12.0
 
# Build
cmake --build .
```

# Build

Para a build após atender os requisitos:
`./build.sh`

# Executar

Para executar o código:
```
cd build

./detection_test
```
