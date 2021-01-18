/*__kernel
void saxpy_kernel(float alpha,
                   __global float *A,
                   __global float *B,
                   __global float *C)
{
   int index = get_global_id(0);
   //printf(\"%f + %f\", A[index], B[index]);
   C[index] = alpha * A[index] + B[index];
}*/

#define DATA_TYPE int

//The bitonic sort kernel does an ascending sort
__kernel
void bitonic_sort_kernel(__global DATA_TYPE * input_ptr,
           const uint stage,
           const uint passOfStage )
{
 uint threadId = get_global_id(0);
 uint pairDistance = 1 << (stage - passOfStage);
 uint blockWidth = 2 * pairDistance;
 uint temp;
 bool compareResult;
 uint leftId = (threadId & (pairDistance -1))
 + (threadId >> (stage - passOfStage) ) * blockWidth;
 uint rightId = leftId + pairDistance;

 DATA_TYPE leftElement, rightElement;
 DATA_TYPE greater, lesser;
 leftElement = input_ptr[leftId];
 rightElement = input_ptr[rightId];

 uint sameDirectionBlockWidth = threadId >> stage;
 uint sameDirection = sameDirectionBlockWidth & 0x1;

 temp = sameDirection?rightId:temp;
 rightId = sameDirection?leftId:rightId;
 leftId = sameDirection?temp:leftId;

 compareResult = (leftElement < rightElement) ;

 greater = compareResult?rightElement:leftElement;
 lesser= compareResult?leftElement:rightElement;

 input_ptr[leftId] = lesser;
 input_ptr[rightId] = greater;
}