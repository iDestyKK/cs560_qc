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

## Analysis 1: MySQL vs. Redis
### Insertions (Writes)
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
SELECT * FROM test WHERE t_key IN (key1, key2, key3, ...);
```

For Redis, the exact query look like this:
```bash
Redis> mget key1 key2 key3
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

### Conclusions
| | MySQL (HDD) | MySQL (SSD) | Redis (HDD) | Redis (SSD) |
| ---: | :---: | :---: | :---: | :---: |
| Insertion (via Command Line) | 2.1234 | 0.7448 | 0.0518 | 0.0497 |
| Find (via Command Line) | 0.2314 | 0.2294 | 0.0326 | 0.0356 |
| Insertion (via Node.JS) | 2.5508 | 1.0336 | 0.2500 | 0.2370 |
| Find (via Node.JS) | 0.3922 | 0.3798 | 0.2314 | 0.2228 |

**Table 1:** Performance of MySQL and Redis on HDD and SSDs with 100000 keys in all queries.

Overall, Redis outperforms MySQL in a simple key-value pair insertion and finding of any size, with MySQL showing a larger performance penalty as the key count grows. When comparing command line to an external wrapper such as with software like Node.JS, performance in every possible field took a hit, regardless of whether the hardware was with an SSD or HDD. This appears to be the case with any number of keys as well, but only for MySQL insertions.
