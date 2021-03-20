

# LVL2 Pattern matching:

### cmake

* get program which check patter matches (stdin and stdout): \
````  
    $: cmake
    $: ./PatternMatching < someInFile.txt > someOutFile.txt
````
* test generating program: \
````  
    $: cmake
    $: ./gen_tests
````
* program for comparing work efficiency: \
  it is advisable to generate additional tests before calling (program above)
````  
    $: cmake
    $: ./measure
````

#Level description

You get a long string (in the format size, then string) and then a few substrings (in the format number of substrings, then for each length and the substring itself)

For example:    \
11 abracadabra  \
3               \
3 rac           \
5 barrack       \
2 ab

To output, you need to present the number of matches for each substring (numbering from
one in the order of submission)

1 1 \
2 0 \
3 2

The essence of the task is to speed up the search on the GPU and in the same way to
overtake the CPU

In some articles (e.g. [1]), the authors claim that they manage to achieve a performance
gain of up to 30 (!) times

### Sources

- [1] https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-35-fast-virus-signature-matching-gpu

    Code from the article for easy reading:
    > Example 35-1. Cg Code for Reading Bytes from the Packet Buffer and Generating Tags
    > ```c
    >     void get_words(in float4 in_pos
    >                    ,   out float4 w0
    >                    ,   out float4 w1
    >                    ,   out float4 tag0
    >                    ,   out float4 tag1
    >                    ,   const uniform samplerRECT pkt_buffer) 
    >     {   // Read the data from the packet buffer.    
    >         float4 p =  texRECT(pkt_buffer, pos.xy);
    >         float4 p_p, n_p;   
    >         float2 pindex = float2(pos.x-1, pos.y);  
    >         float2 nindex = float2(pos.x+1, pos.y);   
    >         // Check boundary case and perform address wrapping if needed.    
    >         // PKT_MAP_X_SZ is the packet buffer size.    
    >         if (pos.x == 0)     
    >             pindex = float2(PKT_MAP_X_SZ-1, pos.y-1);   
    >         if (pos.x == PKT_MAP_X_SZ-1)    
    >             nindex = float2(0, pos.y+1);   
    >         // Read the second word from the packet buffer.    
    >         float4 p_p = texRECT(pkt_buf, pindex);   
    >         float4 n_p = texRECT(pkt_buf, nindex);   
    >         // Pack data and check for 2-word alignment.    
    >         if (floor(fmod(in_pos.x, 2.0))) {     
    >             w0 = float4(p.w, p.x, 0, 0);     
    >             tag0 = float4(p.z, p_p.w, p_p.x, p.y);     
    >             w1 = float4(n_p. z, p.w, 0, 0);     
    >             tag1 = float4(p.y, p.z, p_p.w, p.x);   
    >         } 
    >         else {    
    >             w0 = float4(p.y, p.z, 0, 0);     
    >             tag0 = float4(p_p.x, p_p.y, p_p.z, p_p.w);     
    >             w1 = float4(p.x, p.y, 0, 0);    
    >             tag1 = float4(p_p.w, p_p.x, p_p.y, p.z); 
    >         } 
    >     }
    > ```
    > 
    > Example 35-2. Cg Code for Virus Signature Matching on the GPU
    > ```c
    >     void signature_match(in float4 fragid : WPOS
    >                          ,   out float4 output_value : COLOR0
    >                          ,   const uniform samplerRECT pkt_buffer
    >                          ,   const uniform samplerRECT g_table) 
    >     {   
    >         float4 word0, word1, tag0, tag1;   
    >         // Read 2 words from the packet buffer and create tag.   
    >         get_words(fragid, word0, word1, tag0, tag1, pkt_buffer);   
    >         // Use the word values to index the virus pattern table.    
    >         float4 h0 = texRECT(g_table, word0.xy * 256);   
    >         float4 h1 = texRECT(g_table, word1.xy * 256);   
    >         // Check if all 4 of the bytes match.    
    >         float all_match0 = all(h0 == tag0);   
    >         float all_match1 = all(h1 == tag1);   
    >         output_value = float4(0, 0, 0, 0);   
    >         // Discard this output if there are no matches.    
    >         if (!(all_match0 || all_match1))     
    >             discard;   
    >         output_value.xy = all_match0 * word0.xy;   
    >         output_value.zw = all_match1 * word1.xy; 
    >     }
    > ```

- [2] [A Massively Parallel Antivirus Engine](https://projects.ics.forth.gr/_publications/gravity_raid10.pdf)