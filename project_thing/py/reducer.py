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

#pp = pprint.PrettyPrinter(indent=4);

#pp.pprint(my_words);

#fout = json.dumps(my_words);

#print(fout);

for word in my_words:
	for filename in my_words[word]:
		print("{}\t{}\t{}".format(word, filename, my_words[word][filename]));
