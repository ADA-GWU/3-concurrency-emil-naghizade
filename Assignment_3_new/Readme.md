# C++ Project with OpenCV

This project demonstrates how to build a C++ program using OpenCV. The setup instructions below guide you through installing CMake, configuring OpenCV, and building the project.

---

## Prerequisites

### 1. Install a C++ Compiler
Ensure you have a C++ compiler installed on your system:
- **Windows**: Install the [Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/) or [MinGW](https://www.mingw-w64.org/).
- **Linux**: Install GCC using your package manager (e.g., `sudo apt install g++`).
- **Mac**: Install Xcode Command Line Tools: `xcode-select --install`.

### 2. Install OpenCV
Download and install OpenCV from [OpenCV.org](https://opencv.org/releases/):
- On **Windows**: Download pre-built binaries and unzip them to a folder.
- On **Linux**: Use your package manager, e.g., `sudo apt install libopencv-dev`.
- On **Mac**: Install via Homebrew, e.g., `brew install opencv`.

### 3. Install CMake
CMake is required to configure and build the project. Follow the instructions below to install it:

#### **Windows:**
1. Download the latest installer from the [CMake website](https://cmake.org/download/).
2. Run the installer and check the option to **Add CMake to the system PATH** during installation.

#### **Linux:**
1. Install CMake via your package manager:
   ```sh
   sudo apt install cmake

Or install the latest version from source:
```
wget https://github.com/Kitware/CMake/releases/download/v3.xx.x/cmake-3.xx.x.tar.gz
tar -xvf cmake-3.xx.x.tar.gz
cd cmake-3.xx.x
./bootstrap
make
sudo make install
```

#### **MAC:**
Install CMake via Homebrew:
```
brew install cmake
```
</br>

#### **After installation:**
After installation add the cmake `bin` directory to the system environment `path`

## Setting up the Project
### File Structure

After downloading the project, the file structure should look like this:

```
Assignment_3_new/
├── src/           # Source code files
├── CMakeLists.txt # CMake configuration
├── README.md      # Instructions
```

### Build Instructions

#### 1. Change Directory to Project Path
```
cd path/to/project   # path/to/project should be replaced with actual path in the machine
```
#### 2. Create a Build Directory:
```
mkdir build && cd build
```
#### 3. Configure the Project with CMake:
Run CMake to generate build files:
```
cmake ..
```
If OpenCV is in a custom location, specify the OpenCV_DIR:
```
cmake -DOpenCV_DIR=/path/to/opencv/build ..

```

#### 4. Build the Project:
```
cmake --build .

```

## Running the program
1. After building, the executable will be located in the build directory (you might need to check the `build/Debug` directory).</br>
2. Ensure OpenCV's .dll files (on Windows) or .so files (on Linux/Mac) are accessible:
   *Copy them to the same directory as the executable, or
   *Add OpenCV's bin folder to the system PATH.
3. Run the program:
   ```
    ./my_program
   ```
   If this do not work, change the directory to `build/Debug` directory and run `my_program`


## Input to program
There will be a text that requires the file name, square size, and processing mode. Enter this data to run the code. For example:
```
my_program "C:\Users\emiln\OneDrive\Desktop\nature.jpg" 100 M
```
To stop the code in the middle press `CTRL + C`
