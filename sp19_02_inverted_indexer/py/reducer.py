#!/usr/bin/python

import sys;
import json;

current_word = None;
current_offset = 0;
word = None;

my_words = {};

for line in sys.stdin:
	line = line.strip();

	word, fname, offset = line.split('\t', 2);

	if (word in my_words):
		if (fname in my_words[word]):
			my_words[word][fname] += " " + str(offset);
		else:
			my_words[word].update({ fname: str(offset) });
	else:
		my_words[word] = { fname: str(offset) };

	offset = int(offset);

	if (current_word == word):
		current_offset += offset;
	else:
		#if (current_word != None):
		#	# We are done here. Write the results.
		#	print("{}\t{}\t{}".format(current_word, fname, current_offset));

		# Setup for the new word
		current_offset = offset;
		current_word  = word;

# Were we on the last word? Output that too.
#if (current_word == word):
#	print("{}\t{}\t{}".format(current_word, fname, current_offset));

for word in my_words:
	for filename in my_words[word]:
		# If the string is less than 6 characters and occurs more than 1000 times, omit
		if (len(word) < 6 and len(my_words[word][filename].split()) > 1000):
			#sys.stderr.write("{}\t{}\t{}\n".format(word, filename, len(my_words[word][filename].split()))); #, my_words[word][filename]));
			continue;
		else:
			print("{}\t{}\t{}".format(word, filename, my_words[word][filename]));

