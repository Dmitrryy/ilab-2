
#ifdef DEBUG
#define DEBUG_ACTION(act) { act }

#else
#define DEBUG_ACTION(act) {}

#endif //DEBUG

__kernel void bitonic_sort_kernel(__global DATA_TYPE *input_ptr, const uint stage, const uint passOfStage)
{
    const uint threadId = get_global_id(0);

    const uint pairDistance = pow(2.0, stage - passOfStage);
    const uint blockWidth   = 2 * pairDistance;
    const uint blockId      = threadId / (blockWidth / 2);

    const uint leftId  = blockId * blockWidth + threadId % (blockWidth / 2);
    const uint rightId = leftId + pairDistance;

    const uint sameDirectionBlockWidth = blockId >> passOfStage;
    const uint sameDirection = sameDirectionBlockWidth % 2;


    DEBUG_ACTION(printf("id %d, blockId %d, pDist %d, width %d, dir %d, blockDir %d, leftId %d, rightId %d\n", threadId, blockId,
           pairDistance, blockWidth, sameDirection, sameDirectionBlockWidth, leftId, rightId););

    const DATA_TYPE greater = max(input_ptr[rightId], input_ptr[leftId]);
    const DATA_TYPE lesser = min(input_ptr[rightId], input_ptr[leftId]);

    input_ptr[leftId] = sameDirection ? greater : lesser;
    input_ptr[rightId] = sameDirection ? lesser : greater;
}



__kernel void bitonic_sort_kernel2(__global DATA_TYPE *input_ptr, const uint nStages)
{
const uint threadId = get_global_id(0);

    for (uint stage = 0; stage < nStages; stage++)
    {
        for (uint passOfStage = 0; passOfStage < stage + 1; passOfStage++)
        {
const uint pairDistance = pow(2.0, stage - passOfStage);
const uint blockWidth   = 2 * pairDistance;
const uint blockId      = threadId / (blockWidth / 2);

const uint leftId  = blockId * blockWidth + threadId % (blockWidth / 2);
const uint rightId = leftId + pairDistance;

const uint sameDirectionBlockWidth = blockId >> passOfStage;
const uint sameDirection = sameDirectionBlockWidth % 2;

//DEBUG_ACTION(printf("id %d, blockId %d, pDist %d, width %d, dir %d, blockDir %d, leftId %d, rightId %d\n", threadId, blockId,
//                    pairDistance, blockWidth, sameDirection, sameDirectionBlockWidth, leftId, rightId););

const DATA_TYPE greater = max(input_ptr[rightId], input_ptr[leftId]);
const DATA_TYPE lesser = min(input_ptr[rightId], input_ptr[leftId]);

input_ptr[leftId] = sameDirection ? greater : lesser;
input_ptr[rightId] = sameDirection ? lesser : greater;

barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
        }
    }

}