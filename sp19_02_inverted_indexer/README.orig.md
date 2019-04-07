# Hadoop Word Count Example (Cloudlab)

## Instructions
### Setting up
1. On your local machine, run `./copy.sh`. This will copy over all needed files
to the Cloudlab instance of your choice. Copy the files over to **all**
machines in the cluster.
2. `ssh` into that Cloudlab instance.
3. Go into the newly created `~/project_thing` directory (created by
`copy.sh`).
4. Run `sudo ./patch.sh` on all machines in the cluster (yes, this is why you
have to copy to all machines).

### Launching the experiment
1. On the namenode, run `./setup.sh`. It'll make a directory named `tmp_dir`
on the HDFS, and copy the `data/100-0.txt` file into it.
2. Run `./experiment.sh`. It will run the MapReduce on that file.
3. Check `/tmp_dir/out.txt` for results.

Easy.
