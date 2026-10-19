# Time and storage costs

`n` means entries in a structure, `c` means hash-table capacity, `h` means heap capacity, `q` means queue capacity, and `g` means grid cells (187 here). Stack depth is the number of saved changes.

- **O(1):** the work does not grow with the number of entries.
- **O(log n):** follow a path through the heap's tree levels.
- **O(n):** work grows with the number of entries, such as copying an array.
- **O(n²):** a bad case where many entries each need a long search.
- **Expected** means normal hashing behavior; **amortized** spreads occasional resizing work across many operations. Neither promises the same cost on every call.

## Data structures

| Operation | Usual cost | Worst case | Storage |
|---|---|---|---|
| Queue enqueue, dequeue, peek | O(1) | O(1), including full/empty rejection | O(q) array |
| Queue clear | O(q) | O(q) | O(1) extra |
| Heap peek | O(1) | O(1) | O(h) retained array |
| Heap insert | O(log n) amortized | O(n) when the full array grows | O(h), plus replacement array during growth |
| Heap extract-min | O(log n) | O(log n) | O(1) extra |
| Hash find, update, erase | Expected O(1) | O(c) | O(c) table |
| Hash insert | Expected amortized O(1) | O(c + n²) if growth causes many collisions | O(c), plus replacement array during rehash |
| Hash rehash | Expected O(c + n), normally O(c) | O(c + n²) | O(c) replacement array |
| Hash clear/reset | O(c) for the cleared/new capacity | O(c) | Clear reuses storage; reset replaces it |
| Stack push/pop | O(1) | O(1), excluding memory-allocation cost | O(depth) nodes |
| Stack clear | O(depth) | O(depth) | O(1) extra |

The queue never resizes: it rejects an insertion when full. The heap doubles its array when needed. Equal deadlines are ordered by arrival sequence. Heap `clear()` takes O(1) because it only resets the count; its allocated capacity can remain larger than its current size.

Hashing starts at `id % capacity`. Linear probing checks following buckets when a collision occurs. A tombstone keeps a deleted bucket in the search chain. Updating an existing ID does not change the count. Before a new insertion would make occupied-plus-deleted slots reach 70%, the table grows and reinserts the records.

Rehashing also scans and initializes arrays, hence the capacity term `c`. In a bad collision pattern, reinserting `n` records can require O(n²) probing. Expected constant lookup time is not a worst-case guarantee.

## Game operations

| Operation | Usual cost | Worst-case detail |
|---|---|---|
| Conveyor movement | Expected O(g), with normal hash lookups | O(g × c) if each lookup scans the table; O(g) extra snapshot storage |
| Deadline check | Expected O(c) | O(c + f × c), where `f` is the number of expired records looked up again to apply failure |
| Dispatch update | Expected O(c + log n) when loading one valid heap entry | May remove all `n` stale heap entries: O(n log n), plus their hash lookups and the deadline scan |
| Express bonus | Expected O(c) | O(c + a × c), where `a` queued/on-belt records each need another lookup |

Deadline checks, dispatch and express bonuses use O(1) extra storage. A dispatch update may combine the deadline scan, stale heap removals and an express bonus; the hash lookups listed above must also be counted in its worst case.

An express delivery extends deadlines only for queued/on-belt parcels. It leaves already-ready heap priorities unchanged. The shipped levels create at most 60 records, so simple registry scans are sufficient here; a separate deadline-event system would add complexity without much benefit at this size.

## Clearing, copying and freeing memory

Queue, heap and hash-table `clear()` keep their arrays for reuse. `HashTable::reset()` replaces its array; the F3 sample uses this to return to seven buckets. Stack clear deletes its nodes.

All four owning classes free their storage and make deep copies:

| Copy | Time and new storage |
|---|---|
| Queue | O(q) |
| Heap | O(h): constructing the allocated array initializes its entries, even though only `n` live entries are copied |
| Hash table | O(c) |
| Stack | O(depth) |

Assignment also frees the object's old storage; for a stack this takes O(old depth). These are manual arrays and linked nodes, with no prebuilt containers replacing the required structures.
