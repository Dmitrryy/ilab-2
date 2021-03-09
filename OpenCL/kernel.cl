
#ifdef DEBUG
#define DEBUG_ACTION(act) { act }

#else
#define DEBUG_ACTION(act) {}

#endif //DEBUG



__kernel void bitonic_sort_kernel_default(__global DATA_TYPE *input_ptr, const uint stage, const uint passOfStage)
{
    const uint threadId = get_global_id(0);

    const uint pairDistance = 1 << (stage - passOfStage);
    const uint blockWidth   = 2 * pairDistance;
    const uint blockId      = threadId / (blockWidth / 2);

    const uint leftId  = blockId * blockWidth + threadId % (blockWidth / 2);
    const uint rightId = leftId + pairDistance;

    const int left_elem = input_ptr[leftId];
    const int right_elem = input_ptr[rightId];

    const uint sameDirectionBlockWidth = blockId >> passOfStage;
    const uint sameDirection = sameDirectionBlockWidth % 2;


    DEBUG_ACTION(printf("id %d, blockId %d, pDist %d, width %d, dir %d, blockDir %d, leftId %d, rightId %d\n", threadId, blockId,
           pairDistance, blockWidth, sameDirection, sameDirectionBlockWidth, leftId, rightId););

    const DATA_TYPE greater = max(right_elem, left_elem);
    const DATA_TYPE lesser  = min(right_elem, left_elem);

    input_ptr[leftId]  = sameDirection ? greater : lesser;
    input_ptr[rightId] = sameDirection ? lesser : greater;
}




__kernel void bitonic_sort_kernel_loop(__global DATA_TYPE *input_ptr, const uint nStages)
{
    const uint threadId = get_global_id(0);

    for (uint stage = 0; stage < nStages; stage++)
    {
        for (uint passOfStage = 0; passOfStage < stage + 1; passOfStage++)
        {
            const uint pairDistance = 1 << (stage - passOfStage);
            const uint blockWidth   = 2 * pairDistance;
            const uint blockId      = threadId / (blockWidth / 2);

            const uint leftId  = blockId * blockWidth + threadId % (blockWidth / 2);
            const uint rightId = leftId + pairDistance;

            const int left_elem = input_ptr[leftId];
            const int right_elem = input_ptr[rightId];

            const uint sameDirectionBlockWidth = blockId >> passOfStage;
            const uint sameDirection = sameDirectionBlockWidth % 2;

            DEBUG_ACTION(printf("id %d, blockId %d, pDist %d, width %d, dir %d, blockDir %d, leftId %d, rightId %d\n", threadId, blockId,
                                pairDistance, blockWidth, sameDirection, sameDirectionBlockWidth, leftId, rightId););

            const DATA_TYPE greater = max(right_elem, left_elem);
            const DATA_TYPE lesser  = min(right_elem, left_elem);

            input_ptr[leftId] = sameDirection ? greater : lesser;
            input_ptr[rightId] = sameDirection ? lesser : greater;


            barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
        }
    }

}




__kernel void bitonic_sort_compare_kernel(__global const DATA_TYPE *input_ptr, __global bool *output_ptr, const uint stage, const uint passOfStage)
{
    const uint threadId = get_global_id(0);
    const uint grwId = get_group_id(0);
    //printf("%d\n", grwId);

    const uint pairDistance = 1 << (stage - passOfStage);
    const uint blockWidth   = 2 * pairDistance;
    const uint blockId      = threadId / (blockWidth / 2);

    const uint leftId  = blockId * blockWidth + threadId % (blockWidth / 2);
    const uint rightId = leftId + pairDistance;

    const uint sameDirectionBlockWidth = blockId >> passOfStage;
    const uint sameDirection = sameDirectionBlockWidth % 2;


    DEBUG_ACTION(printf("id %d, blockId %d, pDist %d, width %d, dir %d, blockDir %d, leftId %d, rightId %d\n", threadId, blockId,
                        pairDistance, blockWidth, sameDirection, sameDirectionBlockWidth, leftId, rightId););

    if((sameDirection && input_ptr[leftId] < input_ptr[rightId])
        || (!sameDirection && input_ptr[leftId] > input_ptr[rightId]))
    {
        output_ptr[threadId] = true;
    }
    else
    {
        output_ptr[threadId] = false;
    }
}


__kernel void bitonac_sort_swap_kernel(__global DATA_TYPE *input_ptr, __global const bool *needSwap, const uint stage, const uint passOfStage)
{
    const uint threadId = get_global_id(0);

    if (!needSwap[threadId]) {
        /* nop */
    }
    else
    {
        const uint pairDistance = 1 << (stage - passOfStage);
        const uint blockWidth   = 2 * pairDistance;
        const uint blockId      = threadId / (blockWidth / 2);

        const uint leftId  = blockId * blockWidth + threadId % (blockWidth / 2);
        const uint rightId = leftId + pairDistance;

        DATA_TYPE tmp = input_ptr[leftId];
        input_ptr[leftId] = input_ptr[rightId];
        input_ptr[rightId] = tmp;
    }
}