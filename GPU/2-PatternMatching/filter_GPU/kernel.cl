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


__kernel image_test(__global char* pkt_buffer
                    , __read_only image2d_t g_table