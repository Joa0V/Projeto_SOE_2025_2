# Install minimal prerequisites (Ubuntu 18.04 as reference)
sudo apt update && sudo apt install -y cmake make g++ wget unzip

### OpenCV
# Create opencv directory on /home/rasp-soe
cd 
mkdir opencv
cd opencv
 
# Download and unpack OpenCV sources
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.12.0.zip
unzip opencv.zip
 
# Create build directory 
mkdir -p build && cd build
 
# Configure OpenCV
cmake  ../opencv-4.12.0

echo 'export PATH="~/opencv/build:$PATH"' >> ~/.bashrc
source .bashrc

### Detection-test

# Build
cd ~/Projeto_SOE_2025_2/cpp_detection
cmake --build .

echo 'export PATH="~/Projeto_SOE_2025_2/cpp_detection/build:$PATH"' >> ~/.bashrc
source .bashrc

### Compile application
cd ~/application

make
make clean

mkdir frames
cd ..

### Optional: make the application a service
#sudo cp rasp.service /etc/systemd/system
#sudo systemctl enable rasp.service
#sudo systemctl start rasp.service

