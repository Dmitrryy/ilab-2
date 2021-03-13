# OpenCL


## Levels

### LVL1 (done):
Bitonic sort on GPU.

####source:
* [статья на codeRoad](https://coderoad.ru/26804153/OpenCL-%D0%BA%D0%BE%D0%BD%D1%86%D0%B5%D0%BF%D1%86%D0%B8%D1%8F-%D1%80%D0%B0%D0%B1%D0%BE%D1%87%D0%B5%D0%B9-%D0%B3%D1%80%D1%83%D0%BF%D0%BF%D1%8B)
* [An Introduction to the OpenCL Programming Model](https://cims.nyu.edu/~schlacht/OpenCLModel.pdf)

### LVL2 Pattern matching:

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

[1] https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-35-fast-virus-signature-matching-gpu