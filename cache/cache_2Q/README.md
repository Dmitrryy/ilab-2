# cache - 2Q

## algorithm

The cache is divided into three parts:

* In - FIFO incoming cache where new elements are placed.
* Out - FIFO outgoing cache, which moves elements that were pushed out of In and Hot.
* Hot - LRU cache for items that are out out.

The strategy of evictions from the cache:

Elements requested from 'In' do not move anywhere. Elements displaced from 'In' are moved to 'Out' box.
Elements requested from 'Out' - fall into the 'Hot' box. Removed from 'Out' (not used) - are deleted.

## bags

* memory leak for arrays. (as a solution, you can do a wrapper in structures)
  