#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define __CL_ENABLE_EXCEPTIONS

// Import standard C++ libaries
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

// Import OpenCL headers
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// CAUTION: This code is provided AS-IS and should be considered an archive for reference purposes only.
// The code may contain errors or bugs that may not be fixed.

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
    
    std::vector<cl_int> vec1(32);
    std::vector<cl_int> vec2(16);
    std::vector<cl_int> results(128);
    
    cl::Buffer buf1;
    cl::Buffer buf2;
    cl::Buffer buf3;
    
    void* map1;
    
    std::cout << "Initializing data...";
    
    // Use a new seed based on current time
    srand(time(NULL));
    
    // Fill vec1 with random numbers between 10 and 20 (inclusive)
    for (int i = 0; i < 32; i++) {
        vec1[i] = rand() % 11 + 10;
    }
    
    // Fill vec2 first half with random numbers between 2 to 9 (inclusive)
    for (int i = 0; i < 8; i++) {
        vec2[i] = i + 2;
    }
    
    // Fill vec2 second half with random numbers between -9 to -2 (inclusive)
    for (int i = 8; i < 16; i++) {
        vec2[i] = i -17;
    }
    
    std::cout << " done." << std::endl << std::endl;
    
    // Print data
    std::cout << "Data: " << std::endl;
    std::cout << "vec1: ";
    for (auto it = vec1.cbegin(); it != vec1.cend(); it++) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
    std::cout << "vec2: ";
    for (auto it = vec2.cbegin(); it != vec2.cend(); it++) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl << std::endl;
    
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
    
    // Prepare buffers
    std::cout << "Preparing buffers...";
    buf1 = cl::Buffer(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * vec1.size(), &vec1[0]);
    buf2 = cl::Buffer(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * vec2.size(), &vec2[0]);
    buf3 = cl::Buffer(defaultContext, CL_MEM_WRITE_ONLY, sizeof(cl_int) * 32);
    std::cout << " done." << std::endl;
    
    // Read experiment file
    std::cout << "Reading Experiment4.cl...";
    programFile = std::ifstream("Task1.cl");
    if (!programFile.is_open()) {
        std::cout << " file not found." << std::endl;
        return 1;
    } else {
        std::cout << " done." << std::endl;
    }
    
    // Compile and create kernels
    std::cout << "Compiling Experiment4.cl...";
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
            std::cout << " Could not build program.";
            hasError = 2;
        } else {
            std::cout << " Unknown OpenCL error occurred.";
            hasError = 10;
        }
        std:: cout << std::endl;
    }
    std::cout << std::endl;
    
    // Enqueue kernel
    std::cout << "Enqueuing kernels now! Output will follow:" << std::endl << std::endl;
    kernels[0].setArg(0, buf1);
    kernels[0].setArg(1, buf2);
    kernels[0].setArg(2, buf3);
    hasError = defaultQueue.enqueueNDRangeKernel(kernels[0], 0, vec1.size() / 8);
    
    defaultQueue.finish();
    
    // Read the result
    std::cout << "Reading result from memory...";
    map1 = defaultQueue.enqueueMapBuffer(buf3, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * 32);
    memcpy(&results[0], map1, sizeof(cl_int) * results.size());
    defaultQueue.enqueueUnmapMemObject(buf3, map1);
    std::cout << " done." << std::endl << std::endl;
    
    // Print final output
    std::cout << "Output: " << std::endl << std::endl;
    for (int i = 0; i < 4; i++) {
        int x = i * 32;
        std::cout << "Work Item " << i << std::endl;
        std::cout << "v       :";
        for (int j = x; j < (x + 8); j++) {
            std::cout << " " << std::setw(3) << results[j];
        }
        std::cout << std::endl;
        std::cout << "v1      :";
        for (int j = x + 8; j < (x + 16); j++) {
            std::cout << " " << std::setw(3) << results[j];
        }
        std::cout << std::endl;
        std::cout << "v2      :";
        for (int j = x + 16; j < (x + 24); j++) {
            std::cout << " " << std::setw(3) << results[j];
        }
        std::cout << std::endl;
        std::cout << "results :";
        for (int j = x + 24; j < (x + 32); j++) {
            std::cout << " " << std::setw(3) << results[j];
        }
        std::cout << std::endl << std::endl;
    }
    
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
