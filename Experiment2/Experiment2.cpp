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
    cl::Kernel kernel;
    cl::Buffer alphabetsRBuffer;
    cl::Buffer uchar52WBuffer;
    cl::Buffer uint256RWBuffer;
    
    std::vector<cl_uchar> alphabets(0);
    std::vector<cl_uint> uintegers(0);
    std::vector<cl_uchar> uchar52WBufferOutput(52);
    std::vector<cl_uint> uint256RWBufferOutput(256);
    
    int selectedPlatformIndex = 0;
    int selectedDeviceIndex = 0;
    int queueHasError = -1;
    int hasError = 0;
    
    void* uchar52WBufferMemory;
    void* uint256RWBufferMemory;
    
    std::ifstream programFile;
    std::string programCode;
    
    // Initialize data (invisible to console)
    for (cl_uchar c = 'z'; c >= 'a'; c--) {
        alphabets.push_back(c);
    }
    for (cl_uchar c = 'Z'; c >= 'A'; c--) {
        alphabets.push_back(c);
    }
    for (cl_uint u = 512; u <= 767; u++) {
        uintegers.push_back(u);
    }
    
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
    std::cout << "Select a platform: ";
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
    std::cout << "Select a device: ";
    std::cin >> selectedDeviceIndex;
    selectedDeviceIndex -= 1;
    std::cout << std::endl;
    
    // Create default context
    std::cout << "Creating default context...";
    defaultContext = cl::Context(devices[selectedDeviceIndex]);
    std::cout << " done." << std::endl;
    
    // Create command queue
    std::cout << "Creating command queue based on default context...";
    defaultQueue = cl::CommandQueue(defaultContext, devices[selectedDeviceIndex]);
    std::cout << " done." << std::endl << std::endl;
    
    // Create buffers
    std::cout << "Creating buffer 1...";
    alphabetsRBuffer = cl::Buffer(defaultContext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * 52, &alphabets[0]);
    std::cout << " done." << std::endl;
    std::cout << "Creating buffer 2...";
    uchar52WBuffer = cl::Buffer(defaultContext, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * 52);
    std::cout << " done." << std::endl;
    std::cout << "Creating buffer 3...";
    uint256RWBuffer = cl::Buffer(defaultContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * 256);
    std::cout << " done." << std::endl;
    
    // Enqueue commands
    std::cout << "Enqueuing job to copy buffer 1 to buffer 2...";
    queueHasError = defaultQueue.enqueueCopyBuffer(alphabetsRBuffer, uchar52WBuffer, 0, 0, sizeof(cl_uchar) * 52);
    if (queueHasError == CL_SUCCESS) {
        std::cout << " done." << std::endl;
    } else {
        std::cout << " failed." << std::endl;
    }
    queueHasError = -1;
    
    std::cout << "Enqueuing job to write contents to buffer 3...";
    queueHasError = defaultQueue.enqueueWriteBuffer(uint256RWBuffer, CL_FALSE, 0, sizeof(cl_uint) * 256, &uintegers[0]);
    if (queueHasError == CL_SUCCESS) {
        std::cout << " done." << std::endl;
    } else {
        std::cout << " failed." << std::endl;
    }
    queueHasError = -1;
    std::cout << std::endl;
    
    defaultQueue.finish();
    
    // Read experiment file
    std::cout << "Reading experiment file...";
    programFile = std::ifstream("Experiment2.cl");
    if (!programFile.is_open()) {
        std::cout << " file not found." << std::endl;
        return 1;
    } else {
        std::cout << " done." << std::endl;
    }
    
    // Load contents of experiment file
    std::cout << "Loading Experiment2.cl...";
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
    
    // Create kernels
    if (hasError == 0) {
        std::cout << "Creating kernels...";
        defaultProgram.createKernels(&kernels);
        kernel = kernels[0];
        std::cout << " done." << std::endl;
    }
    
    std::cout << "Setting arguments for kernel 1...";
    kernel.setArg(0, 34.12f);
    kernel.setArg(1, uchar52WBuffer);
    kernel.setArg(2, uint256RWBuffer);
    std::cout << " done." << std::endl;
    
    // Queue kernel for execution
    std::cout << "Enqueuing kernel 1...";
    queueHasError = defaultQueue.enqueueTask(kernel);
    if (queueHasError == CL_SUCCESS) {
        std::cout << " done." << std::endl << std::endl;
    } else {
        std::cout << " failed." << std::endl << std::endl;
    }
    queueHasError = -1;
    
    defaultQueue.finish();
    
    // Read buffer contents
    std::cout << "Enqueuing job to map memory of buffer 2...";
    uchar52WBufferMemory = defaultQueue.enqueueMapBuffer(uchar52WBuffer, CL_FALSE, CL_MAP_READ, 0, sizeof(cl_uchar) * 52);
    std::cout << " done." << std::endl;
    std::cout << "Enqueuing job to read contents from buffer 3...";
    uint256RWBufferMemory = defaultQueue.enqueueMapBuffer(uint256RWBuffer, CL_FALSE, CL_MAP_READ, 0, sizeof(cl_uint) * 256);
    std::cout << " done." << std::endl;
    
    defaultQueue.finish();
    
    std::cout << "Copying buffer 2 memory...";
    memcpy(&uchar52WBufferOutput[0], uchar52WBufferMemory, sizeof(cl_uchar) * 52);
    std::cout << " done." << std::endl;
    std::cout << "Copying buffer 3 memory...";
    memcpy(&uint256RWBufferOutput[0], uint256RWBufferMemory, sizeof(cl_uint) * 256);
    std::cout << " done." << std::endl;
    
    defaultQueue.enqueueUnmapMemObject(uchar52WBuffer, uchar52WBufferMemory);
    defaultQueue.enqueueUnmapMemObject(uint256RWBuffer, uint256RWBufferMemory);
    
    defaultQueue.finish();
    
    std::cout << std::endl;
    std::cout << "Contents of buffer 2:" << std::endl;
    for (cl_uchar uchar : uchar52WBufferOutput) {
        std::cout << uchar << " ";
    }
    std::cout << std::endl << std::endl;
    std::cout << "Contents of buffer 3:" << std::endl;
    for (cl_uint uint : uint256RWBufferOutput) {
        std::cout << uint << " ";
    }
    std::cout << std::endl << std::endl;
    
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
