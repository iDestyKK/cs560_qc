Clara Nguyen and Rachel Offutt <br>
CS560 - MapReduce on Hadoop <br>
Dr. Cao <br>
4/5/2019 <br>

Design Choices <br> <br>
    -Language Choice <br>
    We chose to implement this project in Python because data parsing, especially word parsing, is exponentially easier in Python and one of our team members has extensive experience in working with big data and text parsing in Python. <br><br>
    -Configuration <br>
    We created a script file to run the mapReduce functions to allow for ease of use for the user. The user provides a list of input arguments in the form of file names they would like to run on. <br><br>
    -Setup of Output File<br>
    The output file is set up in terms of word, then file it appears in, then the lines it appears on. We structured our output file this way to make querying easier.<br><br>
    -Stop Words<br>
    To parse our data, we see if there are over 1000 instances of a word and if the word is five letters or less. The reasoning behind our design decision is because when using the complete works of Shakespeare, we opened up the total count of words, and found that generally words that appeared more than 1000 times were likely to be stop words, and the 5 character cutoff is to ensure that the majority of the names are not cut off as well. <br><br><br>
	
2. Running Instructions <br>
	NOTE:  YOU MUST HAVE PYTHON3 ENABLED IN ORDER TO RUN THE QUERYING SCRIPT. PLEASE TYPE THE FOLLOWING COMMAND: <br>
			source /opt/rh/python33/enable <br><br>
			
	Step by step running: <br>
		-Open up a terminal in Linux with the project downloaded. <br>
		-Change your working directory to /mapreduce/ <br>
        -Type the following command: sh execute.sh <list of files to run on> <br>
        -After MapReduce finishes, the querying script will run. You can now enter in a specific word or list of words to search for. (See the following section for screenshots of code working and example use cases in document.)
