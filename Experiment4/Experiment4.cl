#pragma OPENCL EXTENSION cl_amd_printf : enable
#pragma OPENCL EXTENSION cl_intel_printf : enable

__kernel void copy(__global int4* input1, __global int* input2, __global int* output) {
    
    int id = get_global_id(0);
    int8 v = (int8)(input1[id * 2], input1[(id * 2) + 1]);
    int8 v1 = vload8(0, input2);
    int8 v2 = vload8(1, input2);
    int8 results = (int8)(0);
    
    results = select(v2, v1, v > 17);
    
    if (all(results == v2)) {
        results = select((int8)((int4)v1.s0123, (int4)v2.s0123), (int8)results, (int8)(0));
    }
    
    // Disabled, for debugging only
    // printf("Work Item %d\nv       : %3v8hld\nv1      : %3v8hld\nv2      : %3v8hld\nresults : %3v8hld\n", id, v, v1, v2, results);
    
    vstore16((int16)(v, v1), id * 2, output);
    vstore16((int16)(v2, results), (id * 2) + 1, output);
    
}
