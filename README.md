
# Assignment 3
## Task Description
<br/>
The main aim of the task is performing multi-threaded programming in any physical concurrency supporting programming language. To achieve this following procedure should be implemented on the image:
<br/> 
1. Find the average color for the (square size) x (square size) boxes and set the color of the whole square to this average color. Result should be demonstrated by progress, not at once. This is an averaging operation.
<br/> 
2. Application shall take three arguments from the command line: file name, square size and the processing mode (Example: yourprogram somefile.jpg 5 S):
<br/>     
file name : the name of the graphic file of jpg format (no size constraints)
<br/>     
square size : the side of the square for the averaging
<br/>     
processing mode : 'S' - single threaded and 'M' - multi threaded
<br/> 
3. In the multi-processing mode, same procedure should be performed in parallel threads. The number of threads shall be selected according to the computer's CPU cores. It is recommended by instructor to perform the process in Single Thereaded from left-to-right and top-to-down, in Multi-threaded doing the same but in different regions in parallel.
<br/> 
4. There result shall be saved in a result.jpg file
<br/>
5. If image is bigger than user's screen size, then image should be resized to the window size. In case of such resize, the operation shall be performed on the original image, not on the resized one.
<br/>

## Code in C++ Language

```
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>
#include <opencv2/opencv.hpp>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
// Function to get screen size on Windows
std::pair<int, int> getScreenSize() {
    return { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
}
#elif defined(__linux__)
#include <X11/Xlib.h>
// Function to get screen size on Linux
std::pair<int, int> getScreenSize() {
    Display* display = XOpenDisplay(NULL);
    if (display == nullptr) {
        throw std::runtime_error("Unable to open X display");
    }

    Screen* screen = DefaultScreenOfDisplay(display);
    int width = screen->width;
    int height = screen->height;

    XCloseDisplay(display);
    return { width, height };
}
#elif defined(__APPLE__)
#include <CoreGraphics/CoreGraphics.h>
// Function to get screen size on macOS
std::pair<int, int> getScreenSize() {
    CGDirectDisplayID displayId = CGMainDisplayID();
    int width = static_cast<int>(CGDisplayPixelsWide(displayId));
    int height = static_cast<int>(CGDisplayPixelsHigh(displayId));
    return { width, height };
}
#else
#error Platform not supported
#endif

std::mutex repaintMutex; // Mutex to prevent race conditions during repaint
int screenWidth = 0, screenHeight = 0; // Variables to store screen dimensions

// Function to process a single square of the image
void processSquare(cv::Mat& image, int x, int y, int squareSize) {
    int red = 0, green = 0, blue = 0, pixelCount = 0;

    // Compute the average color of the square
    for (int i = y; i < y + squareSize && i < image.rows; ++i) {
        for (int j = x; j < x + squareSize && j < image.cols; ++j) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            red += pixel[2];
            green += pixel[1];
            blue += pixel[0];
            ++pixelCount;
        }
    }

    red /= pixelCount;
    green /= pixelCount;
    blue /= pixelCount;

    // Apply the average color to the square
    for (int i = y; i < y + squareSize && i < image.rows; ++i) {
        for (int j = x; j < x + squareSize && j < image.cols; ++j) {
            image.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
        }
    }
}

// Function to resize the image to fit within the display window
cv::Mat resizeImageForDisplay(const cv::Mat& image, int windowWidth, int windowHeight) {
    cv::Mat resizedImage;
    int newWidth = image.cols, newHeight = image.rows;

    // Compute scaling factors and determine the new size
    if (image.cols > windowWidth || image.rows > windowHeight) {
        float widthRatio = static_cast<float>(windowWidth) / image.cols;
        float heightRatio = static_cast<float>(windowHeight) / image.rows;
        float scale = std::min<float>(widthRatio, heightRatio);

        newWidth = static_cast<int>(image.cols * scale);
        newHeight = static_cast<int>(image.rows * scale);
    }

    cv::resize(image, resizedImage, cv::Size(newWidth, newHeight));
    return resizedImage;
}

// Single-threaded image processing function
void singleThreadProcess(cv::Mat& image, int squareSize) {
    std::tie(screenWidth, screenHeight) = getScreenSize(); // Get screen dimensions
    for (int y = 0; y < image.rows; y += squareSize) {
        for (int x = 0; x < image.cols; x += squareSize) {
            processSquare(image, x, y, squareSize);

            // Display the progress of repainting
            cv::Mat resizedProcessing = resizeImageForDisplay(image, screenWidth, screenHeight);
            cv::imshow("Single Thread Processing", resizedProcessing);
            cv::waitKey(1); // Allow the window to update
        }
    }
}

// Multi-threaded image processing function
void multiThreadProcess(cv::Mat& image, int squareSize) {
    int numThreads = std::thread::hardware_concurrency(); // Get the number of hardware threads
    std::vector<std::thread> threads;

    std::tie(screenWidth, screenHeight) = getScreenSize();
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            // Each thread processes a portion of the image
            for (int y = t * squareSize; y < image.rows; y += numThreads * squareSize) {
                for (int x = 0; x < image.cols; x += squareSize) {
                    processSquare(image, x, y, squareSize);

                    // Display the progress of repainting
                    {
                        std::lock_guard<std::mutex> lock(repaintMutex); // Prevent race conditions during repaint
                        cv::Mat resizedProcessing = resizeImageForDisplay(image, screenWidth, screenHeight);
                        cv::imshow("Multi Thread Processing", resizedProcessing);
                        cv::waitKey(1); // Allow the window to update
                    }
                }
            }
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <file_name> <square_size> <processing_mode>\n";
        return 1;
    }

    const char* fileName = argv[1]; // Input file name
    int squareSize = std::stoi(argv[2]); // Size of the squares to process
    char mode = argv[3][0]; // Processing mode: 'S' for single-threaded, 'M' for multi-threaded

    try {
        std::tie(screenWidth, screenHeight) = getScreenSize(); // Get screen dimensions

        cv::Mat img = cv::imread(fileName, cv::IMREAD_COLOR); // Load the input image
        if (img.empty()) {
            throw std::runtime_error("Failed to load image");
        }

        // Display the resized original image
        cv::Mat resizedImage = resizeImageForDisplay(img, screenWidth, screenHeight);
        cv::imshow("Resized Original", resizedImage);
        cv::waitKey(1);

        // Process the image based on the chosen mode
        if (mode == 'S') {
            singleThreadProcess(img, squareSize);
        }
        else if (mode == 'M') {
            multiThreadProcess(img, squareSize);
        }
        else {
            throw std::runtime_error("Invalid processing mode. Use 'S' for single-threaded or 'M' for multi-threaded.");
        }

        // Save the processed image
        cv::imwrite("result.jpg", img);

        // Display the final processed image
        cv::Mat resizedResultImage = resizeImageForDisplay(img, screenWidth, screenHeight);
        cv::imshow("Result Image", resizedResultImage);
        cv::waitKey(0);

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::cout << "Press Enter to exit...";
        std::cin.ignore();  // Wait for user input before exiting
        return 1;
    }

    return 0;
}

```

## Code Explanation

This code uses OpenCV library functions to read color channels, modify and display the image data. <br/>
First of all, this code is intended to be OS-independent. Therefore in order to obtain the screen size information from the operation system to use in the resizing function, below code that gets the width and height size of the screen from the operation system (Windows, Linux, or MacOS) was implemented.

```
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
// Function to get screen size on Windows
std::pair<int, int> getScreenSize() {
    return { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
}
#elif defined(__linux__)
#include <X11/Xlib.h>
// Function to get screen size on Linux
std::pair<int, int> getScreenSize() {
    Display* display = XOpenDisplay(NULL);
    if (display == nullptr) {
        throw std::runtime_error("Unable to open X display");
    }

    Screen* screen = DefaultScreenOfDisplay(display);
    int width = screen->width;
    int height = screen->height;

    XCloseDisplay(display);
    return { width, height };
}
#elif defined(__APPLE__)
#include <CoreGraphics/CoreGraphics.h>
// Function to get screen size on macOS
std::pair<int, int> getScreenSize() {
    CGDirectDisplayID displayId = CGMainDisplayID();
    int width = static_cast<int>(CGDisplayPixelsWide(displayId));
    int height = static_cast<int>(CGDisplayPixelsHigh(displayId));
    return { width, height };
}
#else
#error Platform not supported
#endif
```

Below `processSquare` function is implemented to calculate the average color of squareSize x squareSize matrix of pixels and then repaint all pixels in the matrix with this average. This is achieved by extracting RGB values of the pixel via `Vec3b`, which is fixed-sized vector with 3 elements (BGR format), then calculating the average of three color channels separately and re-writing the pixel with new average values. It should be noted that if the size of the matrix is bigger than image boundary or if the matrix is iterated continuously till beyond image boundary, then only the pixels within image boundary will be calculated for averaging.

```
// Function to process a single square of the image
void processSquare(cv::Mat& image, int x, int y, int squareSize) {
    int red = 0, green = 0, blue = 0, pixelCount = 0;

    // Compute the average color of the square
    for (int i = y; i < y + squareSize && i < image.rows; ++i) {
        for (int j = x; j < x + squareSize && j < image.cols; ++j) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            red += pixel[2];
            green += pixel[1];
            blue += pixel[0];
            ++pixelCount;
        }
    }

    red /= pixelCount;
    green /= pixelCount;
    blue /= pixelCount;

    // Apply the average color to the square
    for (int i = y; i < y + squareSize && i < image.rows; ++i) {
        for (int j = x; j < x + squareSize && j < image.cols; ++j) {
            image.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
        }
    }
}
```
In case of the image being larger than the user's screen size it should be resized so that entire image should be displayed before, during and after averaging. Therefore, scale factor is calculated by ratio of window dimensions over image dimensions. Later this scale factor is multipled by image dimensions to resize it to dimensions of the screen.
```
    // Compute scaling factors and determine the new size
    if (image.cols > windowWidth || image.rows > windowHeight) {
        float widthRatio = static_cast<float>(windowWidth) / image.cols;
        float heightRatio = static_cast<float>(windowHeight) / image.rows;
        float scale = std::min<float>(widthRatio, heightRatio);

        newWidth = static_cast<int>(image.cols * scale);
        newHeight = static_cast<int>(image.rows * scale);
    }

    cv::resize(image, resizedImage, cv::Size(newWidth, newHeight));
    return resizedImage;
}
```
In single-thread mode, `processSquare` function is called and iterated over pixels within the boundary of original image in left-to-right and top-to-down direction. Each time this function called, the image resized to screen dimensions (if image is bigger than screen originally) and single-thread averaging is displayed via `imshow()` function. Also, `waitkey()` function is used which waits for a key press for a specified amount of time (delay) in milliseconds. This function enables the display to reflect changes made to the image after processing each matrix with 1 ms wait time, which seems to be done immediately. If not used, the window becomes unresponsive until the entire process is finished.
```
// Single-threaded image processing function
void singleThreadProcess(cv::Mat& image, int squareSize) {
    std::tie(screenWidth, screenHeight) = getScreenSize(); // Get screen dimensions
    for (int y = 0; y < image.rows; y += squareSize) {
        for (int x = 0; x < image.cols; x += squareSize) {
            processSquare(image, x, y, squareSize);

            // Display the progress of repainting
            cv::Mat resizedProcessing = resizeImageForDisplay(image, screenWidth, screenHeight);
            cv::imshow("Single Thread Processing", resizedProcessing);
            cv::waitKey(1); // Allow the window to update
        }
    }
}
```
In multi-thread mode the process should progress in prallel in the different threads. To achieve this, first, the number of threads available in the system is obtained with `hardware_concurrency()` function. Then each thread processes certain rows of the image. The second loop assigns different rows (t*squareSize) to different threads while third loop iterates the matrixes.

```
for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            // Each thread processes a portion of the image
            for (int y = t * squareSize; y < image.rows; y += numThreads * squareSize) {
                for (int x = 0; x < image.cols; x += squareSize) {
                    processSquare(image, x, y, squareSize);

                    // Display the progress of repainting
                    {
                        std::lock_guard<std::mutex> lock(repaintMutex);
                        cv::Mat resizedProcessing = resizeImageForDisplay(image, screenWidth, screenHeight);
                        cv::imshow("Multi Thread Processing", resizedProcessing);
                        cv::waitKey(1); // Allow the window to update
                    }
                }
            }
            });
    }
```
Here multiple threads might call `cv::imshow` or `cv::waitKey` simultaneously, which could cause conflicts. `std::lock_guard<std::mutex> lock(repaintMutex)` ensures that only one thread updates the display at a time and avoids crashes. <br/>

In the main part, the program takes inputs from command line and these inputs are assigned to variables. The image is loaded via `imread()` function, in case of failure an exception is thrown. Then the process mode is checked for calling the proper processing functions, in case of improper input an exception is thrown. After the process is finished, the result of the process is written to _result.jpg_ file with `imwrite()` function and also is displayed in separate window. In the end, all thrown exceptions are catched and displayed. Here `cin.ignore` is used to prevent the program automatically exiting and makes it wait for user input.

```
int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <file_name> <square_size> <processing_mode>\n";
        return 1;
    }

    const char* fileName = argv[1]; // Input file name
    int squareSize = std::stoi(argv[2]); // Size of the squares to process
    char mode = argv[3][0]; // Processing mode: 'S' for single-threaded, 'M' for multi-threaded

    try {
        std::tie(screenWidth, screenHeight) = getScreenSize(); // Get screen dimensions

        cv::Mat img = cv::imread(fileName, cv::IMREAD_COLOR); // Load the input image
        if (img.empty()) {
            throw std::runtime_error("Failed to load image");
        }

        // Display the resized original image
        cv::Mat resizedImage = resizeImageForDisplay(img, screenWidth, screenHeight);
        cv::imshow("Resized Original", resizedImage);
        cv::waitKey(1);

        // Process the image based on the chosen mode
        if (mode == 'S') {
            singleThreadProcess(img, squareSize);
        }
        else if (mode == 'M') {
            multiThreadProcess(img, squareSize);
        }
        else {
            throw std::runtime_error("Invalid processing mode. Use 'S' for single-threaded or 'M' for multi-threaded.");
        }

        // Save the processed image
        cv::imwrite("result.jpg", img);

        // Display the final processed image
        cv::Mat resizedResultImage = resizeImageForDisplay(img, screenWidth, screenHeight);
        cv::imshow("Result Image", resizedResultImage);
        cv::waitKey(0);

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::cout << "Press Enter to exit...";
        std::cin.ignore();  // Wait for user input before exiting
        return 1;
    }

    return 0;
}
```
