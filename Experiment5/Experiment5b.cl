#pragma OPENCL EXTENSION cl_amd_printf : enable
#pragma OPENCL EXTENSION cl_intel_printf : enable

__kernel void replaceEncrypt(__global char16* plainText, __global char16* cipherText, __global char16* key) {
    
    int id = get_global_id(0);
    uchar16 ptBlockU = (uchar16)(plainText[id]);
    char16 ptBlock = (char16)(plainText[id]);
    char16 ctBlock = (char16)(0);
    char16 vecA = (char16)('a');
    char16 vecZ = (char16)('z');
    
    ptBlockU = ptBlockU - vecA;
    ctBlock = (char16)shuffle2((char16)key[0], (char16)key[1], ptBlockU);
    
    cipherText[id] = select(ptBlock, ctBlock, ptBlock >= vecA && ptBlock <= vecZ);
    
    // Disabled, for debugging only
    // printf("%3d PT : %3v16hhd\n    CT : %3v16hhd\n    RS : %3v16hhd\n", id, ptBlock, ctBlock, cipherText[id]);
    
}
