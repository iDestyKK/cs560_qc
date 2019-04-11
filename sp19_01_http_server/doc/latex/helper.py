#
# HTTP Server - Helper Functions
#
# Description:
#     Defines all helper functions which are used throughout the server's life.
#
# Authors:
#     Clara Nguyen & Rach Offutt
#

import os
import json
from globals import *

#
# config_open
#
# Description:
#     Opens a configuration json file and stores it in a variable. This can be
#     used for reading any JSON file for any purpose. Though it's mainly used
#     for reading in the initial "config.json" for the server.
#

def config_open(path):
	# Load the file into a dictionary
	with open(path) as fp:
		json_dat = json.load(fp);

	return json_dat;

#
# friendly_size
#
# Description:
#     Takes "size" and turns it into a friendly value that has less numbers...
#

def friendly_size(size):
	units = [
		"", "K", "M", "G", "T", "P", "E", "Z", "Y"
	];

	cu = 0;

	while (size > 1024):
		cu += 1;
		size /= 1024;

	return str(round(size, 1)) + units[cu];

#
# determine_MIME
#
# Description:
#     From a file path, determine the file type based on the extension.
#

def determine_MIME(path):
	# Make the path lowercase and extract extension
	# Oh... and remove the "." at the beginning too.
	ext = os.path.splitext(path.lower())[1][1:];

	if ext in MIME:
		return MIME[ext];

	# Just tell the webserver to return raw data.
	return MIME['other'];
