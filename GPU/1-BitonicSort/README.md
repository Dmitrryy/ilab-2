# LVL1 (done):

Bitonic sort on GPU.

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

###sources:
* [статья на codeRoad](https://coderoad.ru/26804153/OpenCL-%D0%BA%D0%BE%D0%BD%D1%86%D0%B5%D0%BF%D1%86%D0%B8%D1%8F-%D1%80%D0%B0%D0%B1%D0%BE%D1%87%D0%B5%D0%B9-%D0%B3%D1%80%D1%83%D0%BF%D0%BF%D1%8B)
* [An Introduction to the OpenCL Programming Model](https://cims.nyu.edu/~schlacht/OpenCLModel.pdf)
