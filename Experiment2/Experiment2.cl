__kernel void task2(float value,
					__global unsigned char *copied,
					__global unsigned int *integers) 
{
	printf("Value = %.2f\n\n", value);
}
