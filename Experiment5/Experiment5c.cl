#pragma OPENCL EXTENSION cl_amd_printf : enable
#pragma OPENCL EXTENSION cl_intel_printf : enable

__kernel void replaceDecryct(__global char16* cipherText, __global char16* plainText, __global char16* key) {
    
    int id = get_global_id(0);
    uchar16 ctBlockU = (uchar16)(cipherText[id]);
    char16 ctBlock = (char16)(cipherText[id]);
    char16 ptBlock = (char16)(0);
    char16 vecA = (char16)('A');
    char16 vecZ = (char16)('Z');
    
    ctBlockU = ctBlockU - vecA;
    ptBlock = (char16)shuffle2((char16)key[0], (char16)key[1], ctBlockU);
    
    plainText[id] = select(ctBlock, ptBlock, ctBlock >= vecA && ctBlock <= vecZ);
    
    // Disabled, for debugging only
    // printf("%3d CT : %3v16hhd\n    PT : %3v16hhd\n    RS : %3v16hhd\n", id, ctBlock, ptBlock, plainText[id]);
    
}

__kernel void shiftDecrypt(__global char4* cipherText, __global char4* plainText, int shift) {
    
    int id = get_global_id(0);
    char4 ctBlock = (char4)(cipherText[id]);
    char4 ptBlock = (char4)(cipherText[id]);
    char4 shiftDiff = (char4)(shift);
    char4 vecA = (char4)(65);
    char4 vecZ = (char4)(90);
    char4 vec26 = (char4)(26);
    
    ptBlock = (ctBlock - vecA - shiftDiff + vec26) % vec26 + vecA;
    ptBlock = select(ctBlock, ptBlock, ctBlock >= vecA && ctBlock <= vecZ);
    
    // Disabled, for debugging only
    // printf("Block %-3d : PT %v4hhd CT %v4hhd\n", id, ptBlock, ctBlock);
    
    plainText[id] = ptBlock;
    
}
