#pragma OPENCL EXTENSION cl_amd_printf : enable
#pragma OPENCL EXTENSION cl_intel_printf : enable

__kernel void shiftEncrypt(__global char4* plainText, __global char4* cipherText, int shift) {
    
    int id = get_global_id(0);
    char4 ptBlock = (char4)(plainText[id]);
    char4 ctBlock = (char4)(plainText[id]);
    char4 shiftDiff = (char4)(shift);
    char4 vecA = (char4)(65);
    char4 vecZ = (char4)(90);
    char4 vec26 = (char4)(26);
    
    ctBlock = (ptBlock - vecA + shiftDiff + vec26) % vec26 + vecA;
    ctBlock = select(ptBlock, ctBlock, ptBlock >= vecA && ptBlock <= vecZ);
    
    // Disabled, for debugging only
    // printf("Block %-3d : PT %v4hhd CT %v4hhd\n", id, ptBlock, ctBlock);
    
    cipherText[id] = ctBlock;
    
}
