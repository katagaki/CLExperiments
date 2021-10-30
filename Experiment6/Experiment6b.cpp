#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define __CL_ENABLE_EXCEPTIONS

// Import standard C++ libaries
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// Import OpenCL headers
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// CAUTION: This code is provided AS-IS and should be considered an archive for reference purposes only.
// The code may contain errors or bugs that may not be fixed.

// Modified BMP functionality
unsigned char* read_BMP_RGB_to_RGBA(std::string filename, int* widthOut, int* heightOut) {
    
    char fileHeader[54];
    int width, height, offset, imageSize, padSize, rowSize, i, j;
    char colour[3];
    unsigned char* imageData;
    
    std::ifstream inputFileStream(filename, std::ios::in | std::ios::binary);
    if (!inputFileStream.is_open()) {
        std::cout << "File not found." << std::endl;
        return NULL;
    }
    
    inputFileStream.read(fileHeader, 54);
    
    offset = fileHeader[10];
    width = fileHeader[21] << 24;
    width |= fileHeader[20] << 16;
    width |= fileHeader[19] << 8;
    width |= fileHeader[18];
    width = abs(width);
    height = fileHeader[25] << 24;
    height |= fileHeader[24] << 16;
    height |= fileHeader[23] << 8;
    height |= fileHeader[22];
    height = abs(height);
    
    imageSize = width * height * 4;
    imageData = new unsigned char[imageSize];
    
    inputFileStream.seekg(offset, std::ios::beg);
    
    padSize = width % 4;
    if (padSize != 0) {
        padSize = 4 - padSize;
    }
    
    rowSize = width * 4;
    
    for (i = 0; i < height; i++) {
        for (j = 0; j < rowSize; j += 4) {
            inputFileStream.read(colour, 3);
            imageData[i*rowSize + j] = colour[0];
            imageData[i*rowSize + j + 1] = colour[1];
            imageData[i*rowSize + j + 2] = colour[2];
            imageData[i*rowSize + j + 3] = 255;
        }
        for (j = 0; j < padSize; j++) {
            inputFileStream.read(colour, 3);
        }
    }
    
    inputFileStream.close();
    
    *widthOut = width;
    *heightOut = height;
    
    return imageData;
    
}

bool write_BMP_RGBA_to_RGB(std::string filename, unsigned char* imageData, int width, int height) {
    
    char fileHeader[54] = {'B','M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int imageSize, padSize, rowSize, fileSize, i, j;
    char colour[3];
    
    padSize = width % 4;
    if (padSize != 0) {
        padSize = 4 - padSize;
    }
    
    imageSize = (width + padSize) * height * 3;
    
    std::ofstream outputFileStream(filename, std::ios::out | std::ios::binary);
    if (!outputFileStream.is_open()) {
        std::cout << "Unable to save the output image." << std::endl;
        return false;
    }
    
    fileSize = 54 + imageSize;
    
    fileHeader[2] = (unsigned char)fileSize;
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);
    
    fileHeader[18] = (unsigned char)width;
    fileHeader[19] = (unsigned char)(width >> 8);
    fileHeader[20] = (unsigned char)(width >> 16);
    fileHeader[21] = (unsigned char)(width >> 24);
    
    fileHeader[22] = (unsigned char)height;
    fileHeader[23] = (unsigned char)(height >> 8);
    fileHeader[24] = (unsigned char)(height >> 16);
    fileHeader[25] = (unsigned char)(height >> 24);
    
    fileHeader[34] = (unsigned char)imageSize;
    fileHeader[35] = (unsigned char)(imageSize >> 8);
    fileHeader[36] = (unsigned char)(imageSize >> 16);
    fileHeader[37] = (unsigned char)(imageSize >> 24);
    
    outputFileStream.write(fileHeader, 54);
    
    rowSize = width * 4;
    
    for (i = 0; i < height; i++) {
        for (j = 0; j < rowSize; j += 4) {
            colour[0] = (unsigned char)imageData[i*rowSize + j];
            colour[1] = (unsigned char)imageData[i*rowSize + j + 1];
            colour[2] = (unsigned char)imageData[i*rowSize + j + 2];
            outputFileStream.write(colour, 3);
        }
        for (j = 0; j < padSize; j++) {
            outputFileStream.write(colour, 3);
        }
    }
    
    outputFileStream.close();
    
    return true;
    
}

int main(void) {
    
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;
    std::vector<cl::Kernel> kernels;
    
    cl::Context defaultContext;
    cl::CommandQueue defaultQueue;
    cl::Program defaultProgram;
    cl::Program::Sources defaultSource;
    
    int selectedPlatformIndex = 0;
    int selectedDeviceIndex = 0;
    int hasError = 0;
    
    std::ifstream programFile;
    std::string programCode;
    
    std::string inputFileName;
    std::string outputFileName;
    
    unsigned char* inputImage;
    unsigned char* outputImage;
    
    cl::Image2D inputImageBuffer;
    cl::Image2D outputImageBuffer;
    
    bool outputHasError = false;
    
    int imageWidth, imageHeight;
    
    cl::size_t<3> origin, region;
    
    // Select a platform
    cl::Platform::get(&platforms);
    std::cout << "Available Platforms" << std::endl;
    for (int i = 0; i < platforms.size(); i++) {
        std::cout << " " << i + 1 << ": " << platforms[selectedPlatformIndex].getInfo<CL_PLATFORM_VENDOR>() << " " << platforms[selectedPlatformIndex].getInfo<CL_PLATFORM_NAME>() << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Select a platform to continue: ";
    std::cin >> selectedPlatformIndex;
    selectedPlatformIndex -= 1;
    std::cout << std::endl;
    
    // Select a device
    platforms[selectedPlatformIndex].getDevices(CL_DEVICE_TYPE_ALL, &devices);
    std::cout << "Available Devices:" << std::endl;
    for (int i = 0; i < devices.size(); i++) {
        std::cout << " " << i + 1 << ": " << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Select a device to continue: ";
    std::cin >> selectedDeviceIndex;
    selectedDeviceIndex -= 1;
    std::cout << std::endl;
    
    // Create default context and command queue
    defaultContext = cl::Context(devices[selectedDeviceIndex]);
    defaultQueue = cl::CommandQueue(defaultContext, devices[selectedDeviceIndex]);
    
    // Read experiment file
    std::cout << "Reading Experiment6b.cl...";
    programFile = std::ifstream("Experiment6b.cl");
    if (!programFile.is_open()) {
        std::cout << " file not found." << std::endl;
        return 1;
    } else {
        std::cout << " done." << std::endl;
    }
    
    // Compile and create kernels
    std::cout << "Compiling Experiment6b.cl...";
    try {
        programCode = std::string(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));
        defaultSource = cl::Program::Sources(1, std::make_pair(programCode.c_str(), programCode.length() + 1));
        defaultProgram = cl::Program(defaultContext, defaultSource);
        defaultProgram.build(defaultContext.getInfo<CL_CONTEXT_DEVICES>());
        defaultProgram.createKernels(&kernels);
        std::cout << " done." << std::endl;
    } catch (cl::Error e) {
        std::cout << " failed. ";
        if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
            std::cout << " Could not build program." << std::endl;
            std::cout << "Build Log:" << std::endl;
            std::cout << defaultProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultContext.getInfo<CL_CONTEXT_DEVICES>()[0]) << std::endl;
            std::cout << "(End of build log)";
            hasError = 2;
        } else {
            std::cout << " Unknown OpenCL error occurred.";
            hasError = 10;
        }
        std:: cout << std::endl;
    }
    std::cout << std::endl;
    
    // Read file
    std::cout << "Enter the name of the file containing the image to blur: ";
    std::cin >> inputFileName;
    inputImage = read_BMP_RGB_to_RGBA(inputFileName, &imageWidth, &imageHeight);
    if (inputImage == NULL) {
        return 20;
    }
    
    // Print the size (debug only)
    std::cout << "Image loaded with width " << imageWidth << "px and height " << imageHeight << "px" << std::endl << std::endl;
    
    // Set the size of the output
    outputImage = new unsigned char[imageWidth * imageHeight * 4];
    
    // Prepare buffers
    std::cout << "Preparing buffers...";
    inputImageBuffer = cl::Image2D(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, cl::ImageFormat(CL_RGBA, CL_UNORM_INT8), imageWidth, imageHeight, 0, (void*)inputImage);
    outputImageBuffer = cl::Image2D(defaultContext, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, cl::ImageFormat(CL_RGBA, CL_UNORM_INT8), imageWidth, imageHeight, 0, (void*)outputImage);
    std::cout << " done." << std::endl;
    
    // Enqueue kernel
    std::cout << "Enqueuing kernels now!" << std::endl << std::endl;
    kernels[0].setArg(0, inputImageBuffer);
    kernels[0].setArg(1, outputImageBuffer);
    hasError = defaultQueue.enqueueNDRangeKernel(kernels[0], cl::NDRange(0, 0), cl::NDRange(imageWidth, imageHeight));
    
    defaultQueue.finish();
    
    // Read image
    origin[0] = origin[1] = origin[2] = 0;
    region[0] = imageWidth;
    region[1] = imageHeight;
    region[2] = 1;
    defaultQueue.enqueueReadImage(outputImageBuffer, CL_TRUE, origin, region, 0, 0, outputImage);
    
    // Write file
    std::cout << "Enter the name of the file to save the image to: ";
    std::cin >> outputFileName;
    outputHasError = write_BMP_RGBA_to_RGB(outputFileName, outputImage, imageWidth, imageHeight);
    if (!outputHasError) {
        return 20;
    } else {
        std::cout << "File saved successfully." << std::endl;
    }
    
    // Cleanup
    free(inputImage);
    free(outputImage);
    
    // Pause and wait for input before exiting on Windows
#ifdef _WIN32
    std::cout << std::endl;
    if (hasError != 0) {
        std::cout << "Program completed with one or more errors. Press any key to exit.";
    } else {
        std::cout << "Program completed with no errors. Press any key to exit.";
    }
    std::cin.ignore();
#endif
    
    return hasError;
    
}
