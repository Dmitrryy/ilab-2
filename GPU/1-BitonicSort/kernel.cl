/*************************************************************************************************
 *
 *   kernel.cl
 *
 *   Created by dmitry
 *   10.03.2021
 *
 ***/


#ifdef DEBUG
#define DEBUG_ACTION(act) { act }

#else
#define DEBUG_ACTION(act) {}

#endif //DEBUG


/// a kernel that performs only one substage of the bitonic sorting algorithm.
/// That is, only one comparison and one swap for two items.
/// \param input_ptr   - array to sort
/// \param stage       - number of stage
/// \param passOfStage - number of pass
/// \return void
__kernel void bitonic_sort_kernel_default(__global DATA_TYPE *input_ptr,
                                          const uint stage,
                                          const uint passOfStage)
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


    DEBUG_ACTION(printf("id %d, blockId %d, pDist %d, width %d, dir %d, blockDir %d, leftId %d, rightId %d\n",
                        threadId, blockId, pairDistance, blockWidth, sameDirection,
                        sameDirectionBlockWidth, leftId, rightId););

    const DATA_TYPE greater = max(right_elem, left_elem);
    const DATA_TYPE lesser  = min(right_elem, left_elem);

    input_ptr[leftId]  = sameDirection ? greater : lesser;
    input_ptr[rightId] = sameDirection ? lesser : greater;
}



/// unlike the first core, it copies its area to local memory and
/// performs several passes or even stages on it.
/// \param input_ptr   - array to sort
/// \param stage_start - number of first stage
/// \param stage_end   - number of last stage
/// \param pass_start  - number of first pass
/// \return void
__kernel void bitonic_sort_kernel_local(__global DATA_TYPE *input_ptr,
                                        const uint stage_start,
                                        const uint stage_end,
                                        const uint pass_start)
{
    const uint threadId   = get_global_id(0);
    const uint local_id   = get_local_id(0);
    const uint groupSize  = get_local_size(0);
    const uint bankOffset = get_group_id(0) * groupSize * 2;


    // local memory array initialization
    // GROUP_SIZE is defined as a macro at kernel compile time using the -D flag
    __local int local_data[GROUP_SIZE * 2];

    local_data[local_id] = input_ptr[bankOffset + local_id];
    local_data[local_id + groupSize] = input_ptr[bankOffset + local_id + groupSize];

    // ends local memory initialization
    barrier(CLK_LOCAL_MEM_FENCE);


    uint stage_pass = pass_start;
    for(uint stage = stage_start; stage < stage_end; ++stage) {
        for(/*nop*/; stage_pass < stage + 1; ++stage_pass) {

            const uint pairDistance = 1 << (stage - stage_pass);

            const uint blockWidth   = 2 * pairDistance;
            const uint blockId      = threadId / (blockWidth / 2);

            const uint leftId  = blockId * blockWidth + threadId % (blockWidth / 2) - bankOffset;
            const uint rightId = leftId + pairDistance;

            const int left_elem = local_data[leftId];
            const int right_elem = local_data[rightId];

            const uint sameDirectionBlockWidth = blockId >> stage_pass;
            const uint sameDirection = sameDirectionBlockWidth % 2;


            DEBUG_ACTION(printf("stage %d, pass %d, id %d, localId %d, blockId %d, pDist %d, width %d, dir %d, blockDir %d, leftId %d, rightId %d\n",
                                stage, stage_pass, threadId, local_id, blockId,
                    pairDistance, blockWidth, sameDirection, sameDirectionBlockWidth, leftId, rightId););

            const DATA_TYPE greater = max(right_elem, left_elem);
            const DATA_TYPE lesser  = min(right_elem, left_elem);

            local_data[leftId]  = sameDirection ? greater : lesser;
            local_data[rightId] = sameDirection ? lesser : greater;


            barrier(CLK_LOCAL_MEM_FENCE);
        }
        stage_pass = 0;
    }

    // dump the result back into global memory
    input_ptr[bankOffset + local_id] = local_data[local_id];
    input_ptr[bankOffset + local_id + groupSize] = local_data[local_id + groupSize];
}