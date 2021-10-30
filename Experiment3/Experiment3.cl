__kernel void fillArray(int multiplier, __global int *array) {
    int index = get_global_id(0);
    int number = 1 + (index * multiplier);
    array[index] = number;
}
