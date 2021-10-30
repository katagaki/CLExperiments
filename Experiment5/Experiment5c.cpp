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

int main(void) {
    
    std::vector<cl_char> key = {'J', 'R', 'A', 'X', 'V', 'G', 'N', 'P', 'B', 'Z', 'S', 'T', 'L', 'F', 'H', 'Q', 'D', 'U', 'C', 'M', 'O', 'E', 'I', 'K', 'W', 'Y'};
    
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
    
    std::string encryptionType;
    
    std::string ctFileName;
    std::ifstream ctFileStream;
    std::string ctFileContents;
    
    std::string ptFileName;
    std::ofstream ptFileStream;
    
    int shiftValue = 0;
    int padCount = 0;
    
    std::vector<cl_char> plainText;
    std::vector<cl_char> cipherText;
    
    std::string ctString;
    std::string ptString;
    
    cl::Buffer ctBuffer;
    cl::Buffer ptBuffer;
    cl::Buffer keyBuffer;
    
    void* ptBufferMemoryMap;
    
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
    std::cout << "Reading Experiment5c.cl...";
    programFile = std::ifstream("Task2c.cl");
    if (!programFile.is_open()) {
        std::cout << " file not found." << std::endl;
        return 1;
    } else {
        std::cout << " done." << std::endl;
    }
    
    // Compile and create kernels
    std::cout << "Compiling Experiment5c.cl...";
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
    std::cout << "Enter the name of the file containing the ciphertext to decrypt: ";
    std::cin >> ctFileName;
    ctFileStream = std::ifstream(ctFileName);
    if (ctFileStream) {
        std::ostringstream stringStream;
        stringStream << ctFileStream.rdbuf();
        ctFileContents = stringStream.str();
        ctString = ctFileContents;
    } else {
        std::cout << "File not found." << std::endl;
        return 20;
    }
    transform(ctString.begin(), ctString.end(), ctString.begin(), ::toupper);
    std::copy(ctString.begin(), ctString.end(), std::back_inserter(plainText));
    // Ask which type of encryption to decrypt
    
    std::cout << "What is the type of encryption used (U - substitution cipher, S - shift cipher)? ";
    std::cin >> encryptionType;
    transform(encryptionType.begin(), encryptionType.end(), encryptionType.begin(), ::toupper);
    
    if (encryptionType == "U") {
        
        while (plainText.size() % 16 != 0) {
            plainText.push_back(' ');
            padCount++;
        }
        cipherText = std::vector<cl_char>(plainText.size());
        std::cout << std::endl;
        
        // Prepare buffers
        std::cout << "Preparing buffers...";
        ctBuffer = cl::Buffer(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * plainText.size(), &plainText[0]);
        ptBuffer = cl::Buffer(defaultContext, CL_MEM_WRITE_ONLY, sizeof(cl_char) * plainText.size());
        keyBuffer = cl::Buffer(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * key.size(), &key[0]);
        std::cout << " done." << std::endl << std::endl;
        
        // Enqueue kernel
        std::cout << "Enqueuing kernels now!" << std::endl << std::endl;
        kernels[0].setArg(0, ctBuffer);
        kernels[0].setArg(1, ptBuffer);
        kernels[0].setArg(2, keyBuffer);
        hasError = defaultQueue.enqueueNDRangeKernel(kernels[0], 0, plainText.size() / 16);
        
        defaultQueue.finish();
        
        std::cout << std::endl;
        
        // Read the ciphertext memory
        std::cout << "Reading plaintext from memory...";
        ptBufferMemoryMap = defaultQueue.enqueueMapBuffer(ptBuffer, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_char) * plainText.size());
        memcpy(&cipherText[0], ptBufferMemoryMap, sizeof(cl_char) * plainText.size());
        defaultQueue.enqueueUnmapMemObject(ptBuffer, ptBufferMemoryMap);
        std::cout << " done." << std::endl << std::endl;
        
        // Get back the ciphertext in string form
        ptString = std::string(cipherText.begin(), cipherText.end());
        for (int i = 0; i < padCount; i++) {
            ptString.pop_back();
        }
        
        // Display final result
        std::cout << "Final Result:" << std::endl;
        std::cout << " Ciphertext : " << std::endl << ctFileContents << std::endl;
        std::cout << " Plaintext  : " << std::endl << ptString << std::endl;
        
    } else if (encryptionType == "S") {
        
        while (plainText.size() % 4 != 0) {
            plainText.push_back(' ');
            padCount++;
        }
        cipherText = std::vector<cl_char>(plainText.size());
        std::cout << std::endl;
        
        // Ask for the shift value
        std::cout << "Enter the original shift value for the shift cipher (between 1 to 26): ";
        std::cin >> shiftValue;
        if (shiftValue <= 0 || shiftValue >= 27) {
            std::cout << "Invalid shift value of " << shiftValue << " specified!" << std::endl;
            return 30;
        }
        std::cout << std::endl;
        
        // Prepare buffers
        std::cout << "Preparing buffers...";
        ctBuffer = cl::Buffer(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * plainText.size(), &plainText[0]);
        ptBuffer = cl::Buffer(defaultContext, CL_MEM_WRITE_ONLY, sizeof(cl_char) * plainText.size());
        std::cout << " done." << std::endl << std::endl;
        
        // Enqueue kernel
        std::cout << "Enqueuing kernels now!" << std::endl << std::endl;
        kernels[1].setArg(0, ctBuffer);
        kernels[1].setArg(1, ptBuffer);
        kernels[1].setArg(2, shiftValue);
        hasError = defaultQueue.enqueueNDRangeKernel(kernels[1], 0, plainText.size() / 4);
        
        defaultQueue.finish();
        
        std::cout << std::endl;
        
        // Read the ciphertext memory
        std::cout << "Reading plaintext from memory...";
        ptBufferMemoryMap = defaultQueue.enqueueMapBuffer(ptBuffer, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_char) * plainText.size());
        memcpy(&cipherText[0], ptBufferMemoryMap, sizeof(cl_char) * plainText.size());
        defaultQueue.enqueueUnmapMemObject(ptBuffer, ptBufferMemoryMap);
        std::cout << " done." << std::endl << std::endl;
        
        // Get back the ciphertext in string form
        ptString = std::string(cipherText.begin(), cipherText.end());
        for (int i = 0; i < padCount; i++) {
            ptString.pop_back();
        }
        
        // Display final result
        std::cout << "Final Result:" << std::endl;
        std::cout << " Ciphertext : " << std::endl << ctFileContents << std::endl;
        std::cout << " Plaintext  : " << std::endl << ptString << std::endl;
        
    } else {
            std::cout << "Invalid encryption type specified." << std::endl;
            return 20;
    }
    
    // Write file
    std::cout << "Enter the name of the file to save the plaintext to: ";
    std::cin >> ptFileName;
    ptFileStream = std::ofstream(ptFileName);
    if (ptFileStream) {
        ptFileStream << ptString;
        ptFileStream.close();
        std::cout << "Plaintext saved to " << ptFileName << " successfully." << std::endl;
    } else {
        std::cout << "Unable to save the plaintext." << std::endl;
        return 20;
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
