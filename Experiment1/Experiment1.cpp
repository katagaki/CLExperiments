#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define __CL_ENABLE_EXCEPTIONS

// Import standard C++ libaries
#include <fstream>
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
    
    // Get number of platforms
    std::cout << "Detecting platforms...";
    cl::Platform::get(&platforms);
    std::cout << " done." << std::endl << std::endl;
    std::cout << "Number of platforms detected: " << platforms.size() << std::endl << std::endl;
    
    // Show all platforms available to select
    for (int i = 0; i < platforms.size(); i++) {
        std::cout << "Platform " << i + 1 << ": " << platforms[selectedPlatformIndex].getInfo<CL_PLATFORM_VENDOR>() << " " << platforms[selectedPlatformIndex].getInfo<CL_PLATFORM_NAME>() << std::endl;
    }
    std::cout << std::endl;
    
    // Prompt for selection
    std::cout << "Select a platform to view its details: ";
    std::cin >> selectedPlatformIndex;
    selectedPlatformIndex -= 1;
    std::cout << std::endl;
    
    // Get number of devices
    std::cout << "Detecting devices for platform " << selectedPlatformIndex + 1 << "...";
    platforms[selectedPlatformIndex].getDevices(CL_DEVICE_TYPE_ALL, &devices);
    std::cout << " done." << std::endl << std::endl;
    std::cout << "Number of devices detected: " << devices.size() << std::endl << std::endl;
    
    // Show all devices available to select
    for (int i = 0; i < devices.size(); i++) {
        std::cout << "Device " << i + 1 << ": " << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
    }
    std::cout << std::endl;
    
    // Prompt for selection
    std::cout << "Select a device to view its details: ";
    std::cin >> selectedDeviceIndex;
    selectedDeviceIndex -= 1;
    std::cout << std::endl;
    
    // Show details
    std::cout << "About " << devices[selectedDeviceIndex].getInfo<CL_DEVICE_NAME>() << ":" << std::endl;
    std::cout << "Platform: " << platforms[selectedPlatformIndex].getInfo<CL_PLATFORM_VENDOR>() << " " << platforms[selectedPlatformIndex].getInfo<CL_PLATFORM_NAME>() << std::endl;
    if (devices[selectedDeviceIndex].getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_CPU) {
        std::cout << "Type: CPU";
    } else if (devices[selectedDeviceIndex].getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_GPU) {
        std::cout << "Type: GPU";
    } else {
        std::cout << "Type: Unknown";
    }
    std::cout << std::endl;
    std::cout << "Max. Compute Units: " << devices[selectedDeviceIndex].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
    std::cout << "Max. Work Group Size: " << devices[selectedDeviceIndex].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;
    std::cout << "Max. Work Item Dimensions: " << devices[selectedDeviceIndex].getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>() << std::endl;
    std::cout << "Global Memory Size: " << devices[selectedDeviceIndex].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() * 0.000001 << " MB" << std::endl;
    std::cout << "Local Memory Size: " << devices[selectedDeviceIndex].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() * 0.001 << " kB" << std::endl;
    std::cout << std::endl;
    
    // Create default context
    std::cout << "Creating default context...";
    defaultContext = cl::Context(devices[selectedDeviceIndex]);
    std::cout << " done." << std::endl;
    
    // Create command queue
    std::cout << "Creating command queue based on default context...";
    defaultQueue = cl::CommandQueue(defaultContext, devices[selectedDeviceIndex]);
    std::cout << " done." << std::endl << std::endl;
    
    // Read experiment file
    std::cout << "Reading experiment file...";
    programFile = std::ifstream("Experiment1.cl");
    if (!programFile.is_open()) {
        std::cout << " file not found." << std::endl;
        return 1;
    } else {
        std::cout << " done." << std::endl;
    }
    
    // Load contents of experiment file
    std::cout << "Loading Experiment1.cl...";
    programCode = std::string(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));
    std::cout << " done." << std::endl;
    
    // Create OpenCL program source
    std::cout << "Creating OpenCL source...";
    defaultSource = cl::Program::Sources(1, std::make_pair(programCode.c_str(), programCode.length() + 1));
    std::cout << " done." << std::endl;
    
    // Create OpenCL program
    std::cout << "Creating OpenCL program...";
    defaultProgram = cl::Program(defaultContext, defaultSource);
    std::cout << " done." << std::endl;
    
    // Build OpenCL program
    std::cout << "Building OpenCL program...";
    try {
        defaultProgram.build(defaultContext.getInfo<CL_CONTEXT_DEVICES>());
        std::cout << " done." << std::endl;
    } catch (cl::Error e) {
        std::cout << " failed." << std::endl;
        if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
            hasError = 2;
        } else {
            hasError = 10;
        }
    }
    std::cout << std::endl;
    
    // Show build log
    std::cout << "Build Log:" << std::endl;
    std::cout << defaultProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultContext.getInfo<CL_CONTEXT_DEVICES>()[0]) << std::endl;
    std::cout << "(End of build log)" << std::endl << std::endl;
    
    // Create and show kernel information
    if (hasError == 0) {
        std::cout << "Number of kernels discovered: " << defaultProgram.getInfo<CL_PROGRAM_NUM_KERNELS>() << std::endl << std::endl;
        std::cout << "Creating kernels...";
        defaultProgram.createKernels(&kernels);
        std::cout << " done." << std::endl;
        std::cout << "Number of kernels created: " << kernels.size() << std::endl << std::endl;
        for (int i = 0; i < kernels.size(); i++) {
            std::cout << "Kernel " << i + 1 << ": " << kernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>() << std::endl;
        }
        std::cout << std::endl;
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
