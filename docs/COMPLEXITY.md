# Complexity and storage

Let n be stored entries, c allocated hash buckets, and g grid cells (187 by default).

| Operation | Typical / amortized | Worst case | Storage |
|---|---:|---:|---:|
| Circular queue enqueue/dequeue/peek | O(1) | O(1), including rejection | O(capacity) |
| Queue clear | O(capacity) | O(capacity) | no extra allocation |
| Heap peek | O(1) | O(1) | O(n) retained array |
| Heap insert | O(log n) amortized | O(n) when resizing | O(n) |
| Heap extract-min | O(log n) | O(log n) | O(1) extra |
| Hash find/update/erase | expected O(1) at bounded load | O(c) | O(c) |
| Hash insertion | expected amortized O(1) | O(n²) pathological rehash collisions | O(c) plus old array during rehash |
| Hash rehash | expected O(n) | O(n²) under pathological collisions | O(c) |
| Linked stack push/pop | O(1) | O(1), excluding allocator cost | O(depth) |
| Stack clear | O(depth) | O(depth) | O(1) extra |
| Conveyor movement | expected O(g) registry probes | O(g × c) with pathological hash collisions | O(g) snapshots |
| Deadline scan | O(c) | O(c) plus failed-ID hash probes | O(1) extra |
| Dispatch tick | O(c + log n) normally | O(c + n log n) to discard all stale heap entries, plus hash probes | O(1) extra |

The queue is a bounded dynamic allocation, not a resizing container: full admission fails, so its operation bound remains constant. The heap doubles its dynamic array and orders equal deadlines by a monotonically increasing sequence number. The table grows before occupied-plus-tombstone load reaches 70%; modulo hashing makes collisions easy to explain but does not promise constant worst-case behavior. Tombstones preserve probe chains. Duplicate keys update without changing size.

`clear` retains queue/heap/table allocations for reuse (the table clears all bucket states); `HashTable::reset` explicitly replaces its allocation for a repeatable capacity-seven demo. All owning classes disable copying and destroy their owned arrays or linked nodes. There are no ready-made containers substituting for the required structures.

The simulation stores at most 60 parcel records in the shipped levels. Registry scans are intentionally simple and transparent at that scale. An express delivery scans active queued/on-belt records to extend deadlines; already staged heap priorities remain unchanged. A more scalable implementation could maintain separate deadline events, but that would add teaching complexity outside this prototype's scope.
