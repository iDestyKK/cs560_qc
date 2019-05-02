# K-V Software Performance Comparison

## Synopsis
A paper that compares various k-v storage software performance. From "Research
Problem Lists.docx": "Measure the performance of k-v storage software, e.g.,
Redis, vs. SQL database?".

The idea behind this is to try to treat several of these storage structures as
the same, and then push incrementally more data to them and measure
performance. Performance here can be more than time, but also I/O writes and
reads.

## Format
A C program named `gen_input` is used to generate random, shuffled, unique keys
for each of the database programs to read in and insert into their respective
databases.

All executables are made in a way to where they print the following format
string (or some other similar variant):
```
Inserted ??? keys in ???ms
```

This allows us to easily grab runtimes without any kind of shell scripting
magic. It also allows us to time exactly a specific portion of each program
without overhead of other parts affecting it.

## What's planned
### Database Storage Systems
| | SQL | Radis | ??? |
| ---: | :---: | :---: | :---: |
| Node.js - Single Query | ✅ | ❌ | ❓ |
| Node.js - Multi-Query | ❌ | ❌ | ❓ |
| Command Line - Single Query | ✅ | ❌ | ❓ |
| Command Line - Multi-Query | ❌ | ❌ | ❓ |

#### Notes
* ***Single Query** refers to inserting all keys in a single query. **Multi-Query** refers to inserting one key per query.*
* *Each test has two variants: **Node.js**, being a widely used server-side Javascript interface, as well as the **Command Line** applications compiled in C and distributed via official Arch-Linux repositories. Performance between the two is measured for a potential speed advantage one may have over another.*

### Tests Completed
| | SQL | Radis | ??? |
| ---: | :---: | :---: | :---: |
| HDD | ✅ | ❌ | ❓ |
| SSD | ❌ | ❌ | ❓ |

### Procedure
```diff
+ Incremental Data Generation (To chuck at each program)
- csv2graph (Optional. Could just use Excel).
```

There will be more added as the project progresses. Stay tuned.
