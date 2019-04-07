#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys;
import string;
import re;
import os;

punctuation='\'‘’"“”#$%&\\()*+,/:;<=>?@[]^_`{|}—~!?'

# Get the filename that we are currently reading in
f = os.environ['mapreduce_map_input_file'];

for num, line in enumerate(sys.stdin):
    line = line.strip();
    words = line.split();

    # Grab all "words"
    for word in words:
        # Aggressively remove characters in "punctuation".
        word = word.translate(None, punctuation);
        word.strip();

        # Any other punctuation (- and .) turns into spaces
        word = re.sub('-', ' ', word);
        word = re.sub('\.', ' ', word);

        real_words = word.split();

        for real_word in real_words:
            # Manually filter out useless "words"
            if (real_word == ''):
                continue;

            # Log the word... after it's been lowercased.
            print('{}\t{}\t{}'.format(real_word.lower(), f, num + 1));
