__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void lumos(read_only image2d_t input, write_only image2d_t output) {
    
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    float4 originalPixel = read_imagef(input, sampler, coord);
    float r = originalPixel.s0;
    float g = originalPixel.s1;
    float b = originalPixel.s2;
    float luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);
    float4 luminatedPixel = (float4){luminance, luminance, luminance, 1.0};
    
    write_imagef(output, coord, luminatedPixel);

}
