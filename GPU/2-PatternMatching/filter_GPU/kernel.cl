/*************************************************************************************************
 *
 *   kernel.cl
 *
 *   Created by dmitry
 *   18.03.2021
 *
 ***/


#ifdef DEBUG
#define DEBUG_ACTION(act) { act }

#else
#define DEBUG_ACTION(act) {}

#endif //DEBUG


///
///
void get_words(__global char* pkt_buffer
                , const size_t buffer_size
                , const size_t pos
                , float2 *word0
                , float2 *word1
                , float4 *tag0
                , float4 *tag1)
{
    float data[7] = { NON_SYMBOL_CODE, NON_SYMBOL_CODE,NON_SYMBOL_CODE
                        , NON_SYMBOL_CODE, NON_SYMBOL_CODE, NON_SYMBOL_CODE
                        , NON_SYMBOL_CODE };
    for (size_t i = 0; pos + i < buffer_size && i < 7; ++i)
    {
        data[i] = pkt_buffer[pos + i];
    }
    *word0 = (float2)(data[0], data[1]);
    *word1 = (float2)(data[1], data[2]);

    *tag0 = (float4)(data[2], data[3], data[4], data[5]);
    *tag1 = (float4)(data[3], data[4], data[5], data[6]);
}




__kernel void image_test(__global char*      pkt_buffer
                         , __global float2*  res_buffer
                         , const uint        buffer_size
                         , __read_only       image2d_t g_table)
{
    const size_t threadId = get_global_id(0);

    const size_t index0 = threadId * 2;
    const size_t index1 = index0 + 1;

    const float nons = NON_SYMBOL_CODE;

    float2 word0 = nons, word1 = nons;
    float4  tag0 = nons,  tag1 = nons;

    get_words(pkt_buffer, buffer_size, index0, &word0, &word1, &tag0, &tag1);


    const float4 h0 = read_imagef(g_table, (int2)(word0.y + 128, word0.x + 128));
    const float4 h1 = read_imagef(g_table, (int2)(word1.y + 128, word1.x + 128));

    const bool res0 = all(h0 == tag0);
    const bool res1 = all(h1 == tag1);


    res_buffer[index0] = (res0) ? word0 : (float2)(nons);
    res_buffer[index1] = (res1) ? word1 : (float2)(nons);
}