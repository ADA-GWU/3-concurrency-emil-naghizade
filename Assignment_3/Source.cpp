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
                        std::lock_guard<std::mutex> lock(repaintMutex);
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
