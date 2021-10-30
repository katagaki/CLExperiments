__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__constant float gaussianFactor[49] = {0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036, 0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363, 0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446, 0.002291, 0.023226, 0.092651, 0.146768, 0.092651, 0.023226, 0.002291, 0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446, 0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363, 0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036};

__kernel void blur(read_only image2d_t input, write_only image2d_t output) {
    
    int column = get_global_id(0);
    int row = get_global_id(1);
    int2 coord = (int2)(column, row);
    float4 pixel = (float4)(0.0, 0.0, 0.0, 1.0);
    
    // Disabled, for debugging only
    // printf("Coordinates: %d, %d\n", get_global_id(0), get_global_id(1));
    
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
