# Analysis of the data

## Before we begin
The results shown below will be a result of `bench.sh` with the default settings. Recall that these are the following:
```bash
UNIX> ./bench.sh 1000 100 5
```
This means that `bench.sh` is testing the all of these test cases from 100 key-value pairs all the way up to 100000, incrementing at 100 keys per time. These keys are all unique and they are inserted in a shuffled order (via the classic **Fisher-Yates** shuffling algorithm). Each test was run 5 times and then the results were averaged in an effort to remove noise.

Lastly, it needs to be established that, while MySQL and Redis can achieve similar functionality here, they are very different pieces of software. Redis's interface promotes simplistic **key-value pairing**. MySQL is more table-like, allowing for inserting **entire rows and columns** of data, which can consist of a unique key (Primary Key), as well as several value columns, such as usernames, passwords, email addresses, etc. Because of the complexity of MySQL over Redis, performance varying over several of these tests will lead to obvious results.

Furthermore, Redis performs in memory, only dumping to the disk whenever necessary. MySQL's database is stored entirely on the disk. The reason we chose to compare HDD and SSD performance is to show how drastic of a difference this change in hardware will change insertion (or even find) speed, since one is more disk I/O heavy than the other.

## What was tested
* **Insertion (Writes)** of both **MySQL** and **Redis**
* **Find (Reads)** of both **MySQL** and **Redis**
* **HDD** vs **SSD** performance
* **Command Line** vs **Node.JS** library wrappers

`bench.sh` doesn't export CSVs directly, but `gen_csv.sh` is able to convert results into CSVs. These were imported into Microsoft Excel, which is where the graphs were generated and exported.

## How timing was done
For the command line applications, both **mysql** and **redis-cli** feature their own built in time functions. These are what were used to capture runtimes for both applications.

### mysql
The `mysql` executable allows you to get the milliseconds of every query by passing `-vvv`  (Triple Verbose) as a parameter when running the command.
```bash
MySQL> SAMPLE QUERY COMMAND;
Query OK, 100 rows affected (0.019 sec)
```

### redis-cli
Unfortunately, the `redis-cli` executable does not feature a verbose command that gives the times of every query. However, it does feature a command to output the current UNIX timestamp, as well as microseconds. Therefore, to get the duration, we have to resort to queuing up multiple instructions to force the runtime to print.
```bash
Redis> MULTI
Redis> TIME
Redis> SAMPLE QUERY (MSET/MGET)
Redis> TIME
Redis> EXEC
```
This, along with a combination of UNIX applications such as `tail`, `grep`, `tr`, and `awk` allow us to extract and compute the duration directly. The first call to `TIME` prints out the current time before the query was run, while the second call prints out the current time after the query was run. However, the output may seem to be esoteric at first:
```
1557000068
56341
1557000068
88921
```
The first two lines are the output of the first call to `TIME`, while the last two lines are the output from the second call. As stated earlier, the command prints out the current UNIX timestamp as well as the current microseconds. If we use `tr` to put these onto the same line and then use `awk`, we can extract the duration as shown below:
```bash
UNIX> redis-cli < CMD.TXT | tr '\n' ' ' | '{ a = $1 + ($2 / 1000000); b = $3 + ($4 / 1000000); printf("%.09f\n", b - a); }'
0.032580137
```

***Note:** This is shown in **bench.sh** on line 360.*

## Analysis 1: MySQL vs. Redis
### Insertions (Writes)
Insertions were all done in a single query for both MySQL and Redis. The constraints of how many keys were inserted are defined by `bench.sh` as shown above.

For MySQL, the exact query looks like this:
```bash
MySQL> INSERT into test (t_key, t_value) VALUES (key1, value1), (key2, value2), (key3, value3), ...
```

For Redis, the exact query looks like this:
```bash
Redis> mset key1 value1 key2 value2 key3 value3 ...
```

<p align="center">
  <img src = "./mysql_vs_redis/svg/MySQL%20vs.%20Redis%20(Insert%20-%20Command%20Line%20-%20HDD).svg" width = "520px"/>
  <br/>
  <b>Figure 1:</b> Performance between MySQL and Redis performing insertion on a HDD.
</p>

Knowing that Redis performs in memory as opposed to MySQL makes it much, much faster. At 100000 key-value pairs inserted, Redis performs at **0.0518** seconds, whereas MySQL performs at around **2.1234** seconds. While these results aren't surprising, as Redis is **40.9923x** faster than MySQL, we can attempt to do better by switching to a Solid-State Drive (SSD).

<p align="center">
  <img src = "./mysql_vs_redis/svg/MySQL%20vs.%20Redis%20(Insert%20-%20Command%20Line%20-%20SSD).svg" width = "520px"/>
  <br/>
  <b>Figure 2</b> Performance between MySQL and Redis performing insertion on an SSD.
</p>

On an SSD the results for Redis's insertion remains the same, as its performance is done in memory. However, MySQL's performance improves drastically. At 100000 keys, it performs at **0.7448** seconds. This is a **2.8509x** speedup compared to its HDD test runs, and it is **14.3784x** slower than Redis.

### Finds (Reads)
The test performed here is an find operation of incresingly more keys in a database that already has 100000 keys inserted. The first test consists of a find operation with 100 keys. The second test consists of the same operation but with 200 keys. The test goes on until eventually all 100000 keys are in the query (and inevitably found).

For MySQL, the exact query looks like this:
```bash
MySQL> SELECT * FROM test WHERE t_key IN (key1, key2, key3, ...);
```

For Redis, the exact query look like this:
```bash
Redis> mget key1 key2 key3 ...
```

<p align="center">
  <img src = "./mysql_vs_redis/svg/MySQL%20vs.%20Redis%20(Find%20-%20Command%20Line%20-%20HDD).svg" width = "520px"/>
  <br/>
  <b>Figure 3:</b> Performance between MySQL and Redis performing finds on a HDD.
</p>

Redis's advantage of being in memory, again, allows it to come out ahead of MySQL in terms of reading data. However, there is an interesting spike in performance of MySQL that persists after adding around **23700 or more** keys into the query. This performance also holds true in the SSD performance, which leads us to believe that this bottleneck is software-related in the command line application for MySQL. Thankfully, we can actually test if this holds true by running the same tests in Node.JS rather than the C native apps to see if we get different results.

<p align="center">
  <img src = "./mysql_vs_redis/svg/MySQL%20vs.%20Redis%20(Find%20-%20Node.JS%20-%20HDD).svg" width = "520px"/>
  <br/>
  <b>Figure 4:</b> Performance between MySQL and Redis performing finds on a HDD via Node.JS.
</p>

Both Redis and MySQL suffer and show different results when using wrapper libraries and packages in Node.JS to communicate with the respective database softwares. However, MySQL has another interesting performance spike within the 20000-50000 key range. This confirms the suspicion of the impact in performance being caused by software. 

## Analysis 2: HDD vs. SSD
In our previous analysis, we demonstrated some differences between HDD and SSD performance, mainly for the sake of demonstrating MySQL's penalties while inserting on a HDD.
<p align="center">
  <img src = "./hdd_vs_ssd/svg/MySQL%20(Insert%20-%20Command%20Line%20-%20HDD%20vs.%20SSD).svg" width = "520px"/>
  <br/>
  <b>Figure 5:</b> MySQL write performance between HDD and SSD.
</p>
This is because, unlike Redis, MySQL relies entirely on I/O with the disk to store the database. This has shown to be a massive bottleneck to MySQL in every test performed comparing it to Redis. However, explicitly comparing HDD to SSD directly yields interesting results on its own. When comparing insertions, the only real cases that had a major impact on performance were the insertions of keys into a MySQL database. The find cases for MySQL, as well as both the insertion and find cases for Redis had minimal impact upon switching a HDD for an SSD. Let's look more into it.

Just like **MySQL Insertion**, we have other three cases where SSDs have the potential to speed up performance:
* MySQL Find
* Redis Insertion
* Redis Find.

Let's look into each of them.

### MySQL Find
<p align="center">
  <img src = "./hdd_vs_ssd/svg/MySQL%20(Find%20-%20Command%20Line%20-%20HDD%20vs.%20SSD).svg" width = "520px"/>
  <br/>
  <b>Figure 6:</b> MySQL read performance between HDD and SSD.
</p>

This is an interesting case where HDD and SSD appear to have nearly equal performance. Even more interestingly, both the HDD and SSD have the same penalty at around the **23700th** key being added into the query. This performance penalty also holds for when Node.JS is used instead, where it also has an interesting spike of duration between the 20000-50000 key range.

Part of the reason for this similar performance is because of how MySQL internally stores its database on the file system. Cleverly, MySQL uses the **B-Tree** data structure. What makes this special is that, unlike other Binary Search trees (such as **AVL** and **Red-Black**), B-Trees are not optimized for being used in memory. They were constructed specifically for storage systems, such as discs. Since the internal structure for MySQL is a B-tree, optimal for discs, the nearly equal performance between both HDD and SSD in the find operation isn't a surprise.

### Redis Insert & Find
<p align="center">
  <img src = "./hdd_vs_ssd/svg/Redis%20(Insert%20-%20Command%20Line%20-%20HDD%20vs.%20SSD).svg" width = "520px"/>
  <br/>
  <img src = "./hdd_vs_ssd/svg/Redis%20(Find%20-%20Command%20Line%20-%20HDD%20vs.%20SSD).svg" width = "520px"/>
  <br/>
  <b>Figure 7 & 8:</b> Redis write (top) and read (bottom) performance between HDD and SSD.
</p>

Redis's performance between a HDD and an SSD is irrelevant, as it is in-memory. Interestingly, the find operation yields a faster time for the HDD benchmark as opposed to the SSD benchmark. However, when run on Node.JS, this penalty goes away. It's possible that this is due to other running software or RAM restrictions on the test bench, rather than between HDD and SSD.

## Conclusions
| | MySQL (HDD) | MySQL (SSD) | Redis (HDD) | Redis (SSD) |
| ---: | :---: | :---: | :---: | :---: |
| Insertion (via Command Line) | 2.1234 | 0.7448 | 0.0518 | 0.0497 |
| Find (via Command Line) | 0.2314 | 0.2294 | 0.0326 | 0.0356 |
| Insertion (via Node.JS) | 2.5508 | 1.0336 | 0.2500 | 0.2370 |
| Find (via Node.JS) | 0.3922 | 0.3798 | 0.2314 | 0.2228 |

**Table 1:** Performance of MySQL and Redis on HDD and SSDs with 100000 keys in all queries (in seconds).

Overall, Redis outperforms MySQL in a simple key-value pair insertion and finding of any size, with MySQL showing a larger performance penalty as the key count grows. When comparing command line to an external wrapper such as with software like Node.JS, performance in every possible field took a hit, regardless of whether the hardware was with an SSD or HDD. This appears to be the case with any number of keys as well, but only for MySQL insertions. The differences between SSD and HDD only mattered for MySQL insertion queries, as no notable difference was found between finding a key via MySQL, as well as Redis in its entirety.
