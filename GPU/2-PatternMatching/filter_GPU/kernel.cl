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
    float8 source = NON_SYMBOL_CODE;

    // determine the possible number of bytes to read
    // (in case of the end of the buffer)
    const size_t permissible = min(7lu, buffer_size - pos);

    switch(permissible) {
        case 7:  source.s6 = pkt_buffer[pos + 6];
        case 6:  source.s5 = pkt_buffer[pos + 5];
        case 5:  source.s4 = pkt_buffer[pos + 4];
        case 4:  source.s3 = pkt_buffer[pos + 3];
        case 3:  source.s2 = pkt_buffer[pos + 2];
        case 2:  source.s1 = pkt_buffer[pos + 1];
        case 1:  source.s0 = pkt_buffer[pos];
            break;

        default:
            break;
    }

    *word0 = source.xy;
    *word1 = source.yz;

    *tag0 = (float4)(source.s2, source.s3, source.s4, source.s5);
    *tag1 = (float4)(source.s3, source.s4, source.s5, source.s6);
}




__kernel void signature_match(__global char*      pkt_buffer
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

    // Read 2 words from the packet buffer and create tag.
    get_words(pkt_buffer, buffer_size, index0, &word0, &word1, &tag0, &tag1);

    // to get the desired effect, you first need to convert float to char.
    // Direct conversion from float to unsigned char will give zero
    const int2 coord0 = convert_int2(convert_uchar2(convert_char2(word0)));
    const int2 coord1 = convert_int2(convert_uchar2(convert_char2(word1)));
    // Use the word values to index the virus pattern table.
    const float4 h0 = read_imagef(g_table, coord0.yx);
    const float4 h1 = read_imagef(g_table, coord1.yx);

    // Check if all 4 of the bytes match.
    const bool res0 = all(h0 == tag0);
    const bool res1 = all(h1 == tag1);

    res_buffer[index0] = (res0) ? word0 : (float2)(nons);
    res_buffer[index1] = (res1) ? word1 : (float2)(nons);
}