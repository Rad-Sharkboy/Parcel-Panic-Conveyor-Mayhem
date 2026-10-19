# How much work each operation takes

**n** means the number of items. **c** means the allocated array capacity.

- **O(1):** roughly the same work, even with more items.
- **O(log n):** follows a path through the heap's tree levels.
- **O(n) / O(c):** may visit every item / allocated slot.

| Operation | Usual work | Important exception |
|---|---|---|
| Queue enqueue, dequeue, peek | O(1) | A full/empty check is also O(1). |
| Stack push, pop | O(1) | Clearing visits all n nodes: O(n). |
| Heap peek | O(1) | Reads the root. |
| Heap insert / remove root | O(log n) | An insertion that grows the array takes O(n). |
| Hash find, update, erase | Expected O(1) | Many collisions can require O(c) probing. |
| Hash insert | Expected O(1) over many insertions | Resizing usually takes O(c); extreme collisions can make it O(c + n²). |

The heap swaps entries up or down. The hash table tries the next bucket after a collision. At about 70% used/deleted slots, it grows and inserts the live records again.

Copying or clearing arrays takes work proportional to the allocated slots, except heap clear, which only resets its count. Stack copying/clearing visits its nodes. Storage is O(c) for arrays and O(n) for stack nodes.

Gameplay also loops through the grid and parcel records. Hash lookups happen inside some of those loops, so a complete game step is not O(1). The current levels contain at most 60 parcels.
