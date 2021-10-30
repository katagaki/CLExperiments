__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__constant float gaussianFactor[49] = {0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036, 0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363, 0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446, 0.002291, 0.023226, 0.092651, 0.146768, 0.092651, 0.023226, 0.002291, 0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446, 0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363, 0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036};

__kernel void lumos(read_only image2d_t input, write_only image2d_t output, float lumosValue) {
    
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    float4 originalPixel = read_imagef(input, sampler, coord);
    float r = originalPixel.s0;
    float g = originalPixel.s1;
    float b = originalPixel.s2;
    float luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);
    float4 finalPixel = (float4)(0.0, 0.0, 0.0, 1.0);
    if (luminance >= lumosValue) {
        finalPixel = (float4){r, g, b, 1.0};
    }
    
    write_imagef(output, coord, finalPixel);
    
}

__kernel void blur(read_only image2d_t input, write_only image2d_t output) {
    
    int column = get_global_id(0);
    int row = get_global_id(1);
    int2 coord = (int2)(column, row);
    float4 pixel = (float4)(0.0, 0.0, 0.0, 1.0);
    
    for (int i = - 3; i <=  3; i++) {
        
        for (int j = - 3; j <= 3; j++) {
            
            int offsetColumn = column + i;
            int offsetRow = row + j;
            int index = (i + 3) + ((j + 3) * 7);
            pixel.s0 += read_imagef(input, sampler, (int2)(offsetColumn, offsetRow)).s0 * gaussianFactor[index];
            pixel.s1 += read_imagef(input, sampler, (int2)(offsetColumn, offsetRow)).s1 * gaussianFactor[index];
            pixel.s2 += read_imagef(input, sampler, (int2)(offsetColumn, offsetRow)).s2 * gaussianFactor[index];
            
        }
        
    }
    
    write_imagef(output, coord, pixel);
    
}

__kernel void merge(read_only image2d_t bloom, read_only image2d_t original, write_only image2d_t output) {
    
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    float4 bloomPixel = read_imagef(bloom, sampler, coord);
    float4 originalPixel = read_imagef(original, sampler, coord);
    float rB = bloomPixel.s0;
    float gB = bloomPixel.s1;
    float bB = bloomPixel.s2;
    float rO = originalPixel.s0;
    float gO = originalPixel.s1;
    float bO = originalPixel.s2;
    float r = rB + rO;
    float g = gB + gO;
    float b = bB + bO;
    if (r > 1.0) {
        r = 1.0;
    }
    if (g > 1.0) {
        g = 1.0;
    }
    if (b > 1.0) {
        b = 1.0;
    }
    float4 finalPixel = (float4)(r, g, b, 1.0);
    
    write_imagef(output, coord, finalPixel);
    
}
