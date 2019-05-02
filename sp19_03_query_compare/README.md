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
```diff
- SQL
- Radis
```

### Procedure
```diff
+ Incremental Data Generation (To chuck at each program)
- csv2graph (Optional. Could just use Excel).
```

There will be more added as the project progresses. Stay tuned.
