# CS560 - MapReduce on Hadoop
Clara Nguyen and Rachel Offutt

Dr. Cao

4/5/2019

## 1. Design Choices

### Language Choice
We chose to implement this project in Python because data parsing, especially word parsing, is exponentially easier in Python and one of our team members has extensive experience in working with big data and text parsing in Python.

### Configuration
We created a script file to run the mapReduce functions to allow for ease of use for the user. The user provides a list of input arguments in the form of file names they would like to run on.

### Setup of Output File
The output file is set up in terms of word, then file it appears in, then the lines it appears on. We structured our output file this way to make querying easier.

### Stop Words
To parse our data, we see if there are over 1000 instances of a word and if the word is five letters or less. The reasoning behind our design decision is because when using the complete works of Shakespeare, we opened up the total count of words, and found that generally words that appeared more than 1000 times were likely to be stop words, and the 5 character cutoff is to ensure that the majority of the names are not cut off as well.

## 2. Running Instructions
There is a demo of this running on Cloudlab. To see it, visit https://asciinema.org/a/x8wRWlMvEh5Y8aBImklUvLzYZ

Step-by-Step procedure:
1. Set up a Cloudlab instance with the default Hadoop configuration (by `gary`).
2. Have SSH RSA keys set and ready so you can copy over to the Cloudlab instance.
3. On your local machine, open up terminal and go to the directory that contains `copy.sh` from this project.
4. Run `./copy.sh`. It takes 2 arguments:
    1. `username@address`. This is your username and Hadoop cluster address.
    2. Path to private RSA key. This is used to authenticate you to copy files over.
5. SSH into the Hadoop Cluster. The copy script from Step 4 has created a directory in your home directory called `mapreduce`, which stores all of the files in the experiment.
6. Go into `~/mapreduce`. To simply run Hadoop on a series of files, run the following:
    ```bash
    ./execute.sh data/100-0.txt data/test.txt
    ```
    It automates the entire procedure of copying files over to HDFS and doing the Mapreduce for you.
7. The `py/query.py` script will run immediately after completion. Type in a few words!
8. After the script's completion, you will see a `results.txt` file created in the local directory. To run the query script on this file, run the following:
    ```bash
    python3 py/query.py results.txt
    ```
