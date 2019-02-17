#
# COSC 560 (Spring 2019) - Programming Assignment 1 / HTTP Server
#
# Description:
#     It does stuff. I'm going to hate this...
#
# Authors:
#     Clara Nguyen & Rach Offutt
#

# -----------------------------------------------------------------------------
# Python Imports                                                           {{{1
# -----------------------------------------------------------------------------

import socket
import sys
import re
import json
import threading
import os
from datetime import datetime
from urllib.parse import unquote

# -----------------------------------------------------------------------------
# Globals                                                                  {{{1
# -----------------------------------------------------------------------------

# This is so we can put in the right data to the HTTP Header when sending the
# browser data.
MIME = {
	# Web Page files
	""     : "text/html",
	"html" : "text/html",
	"js"   : "text/javascript",
	"css"  : "text/css",
	"txt"  : "text/plain",
	"json" : "application/json",

	# Images
	"gif"  : "image/gif",
	"jpeg" : "image/jpeg",
	"jpg"  : "image/jpeg",
	"png"  : "image/png",
	"svg"  : "image/svg+xml",
	"ico"  : "image/x-icon",

	# Multimedia
	"wav"  : "audio/wave",
	"mp3"  : "audio/mpeg",
	"flac" : "audio/flac",
	"webm" : "audio/webm",
	"ogg"  : "audio/ogg",

	# Default value. Just raw bytes
	"other": "application/octet-stream"
}

# -----------------------------------------------------------------------------
# Helper Functions                                                         {{{1
# -----------------------------------------------------------------------------

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


# -----------------------------------------------------------------------------
# Server Code                                                              {{{1
# -----------------------------------------------------------------------------

#
# buffer_fill
#
# Description:
#     Reads from the socket until the data is exhausted... Probably
#

def buffer_fill(connection):
	buffer = bytes('', 'UTF-8');

	request = connection.recv(1024);
	buffer += request;

	#while True:
	#	request = connection.recv(1024);
	#	if not request:
	#		break;
	#	buffer += request;

	return buffer;

#
# client class
#
# Description:
#     FUCK
#

def read_from_client(config, client_connection, client_address):
	# Construct the buffer
	buffer = buffer_fill(client_connection);

	# Empty HTTP Header
	http_response = "";

	# Buffer to contain the HTML response
	response_buffer = bytes('', 'UTF-8');

	# Buffer to contain the final data sent to client (including response)
	content_buffer = bytes('', 'UTF-8');

	# Before we break anything, we need to know what type of request this
	# is... GET or POST?
	is_get  = False;
	is_post = False;

	# Get the first 3 bytes
	type_chk = buffer[:3];
	type_chks = type_chk.decode("utf-8");

	if (type_chks == "GET"):
		is_get = True;
	elif (type_chks == "POS" and buffer[:4].decode("utf-8") == "POST"):
		is_post = True;

	# ---------------------------------------------------------------------
	# GET data parsing                                                 {{{2
	# ---------------------------------------------------------------------

	if is_get:
		# Decode the request
		req_text = buffer.decode("utf-8");

		# Extract the referer (URLs)
		refs = re.findall("GET \/([^?]*?)[ ?#].*HTTP", req_text);

		# Append the path file
		for url in refs:
			ourl = url;
			url = unquote(url);

			# If it's a "/" and there is a "index.html", add it.
			if url == "" or url[-1:] == "/":
				if os.path.exists(config['page_root'] + "/" + url + "index.html"):
					url += "index.html";

			# Log the GET request
			print("[GET] " + url);

			# Prepare the default HTTP header
			http_response  = "HTTP/1.1 200 OK\r\n";
			http_response += "Content-Type: ";
			http_response += determine_MIME(url);
			http_response += "; charset=UTF-8\r\n";
			http_response += "Content-Encoding: UTF-8\r\n";

			# Find the path and append the file to http_response
			try:
				with open(config['page_root'] + "/" + url, 'rb') as fp:
					# Add the newline
					http_response += "\r\n";

					content_buffer = fp.read();

			except FileNotFoundError:
				# Change the HTTP response to a 404
				http_response = "HTTP/1.1 404 Not Found\r\n\r\n";

			except IsADirectoryError:
				# Add location... only if there isn't a "/"
				if (url != "" and url[-1] != "/"):
					http_response = "HTTP/1.1 308 Permanent Redirect\r\n"
					http_response += "Location: " + os.path.basename(url) + "/\r\n";
					http_response += "\r\n\r\n";
				else:
					print(http_response);

					# Add the newline
					http_response += "\r\n";

					# Generate a response that prints a webpage
					http_response += "<html>\n";

					# Head
					http_response += "<head>\n"
					http_response += "<title>Index of /" + url + "</title>\n"
					http_response += "</head>\n";

					# Body
					http_response += "<body>\n";
					http_response += "<h1>Index of /" + url + "</h1>\n";
					http_response += "<table>\n";

					# Columns
					http_response += "<tr>\n";
					http_response += "<th valign = \"top\"></th>\n";
					http_response += "<th>Name</th>\n";
					http_response += "<th>Last modified</th>\n";
					http_response += "<th>Size</th>\n";
					http_response += "<th>Description</th>\n";
					http_response += "</tr>\n";

					# The horizontal line
					http_response += "<tr><th colspan = \"5\"><hr></th></tr>\n";

					# Parent Directory Link
					icon = {
						"back": "/icons/back.gif",
						"folder": "/icons/folder.gif",
						"file": "/icons/file.gif"
					};

					# TODO: Add icons
					http_response += "<tr>\n<td></td>\n<td>\n<a href = \"../\">";
					http_response += "Parent Directory</a>\n<td></td>\n<td style = \"text-align: right\">-</td>\n";
					http_response += "<td></td>\n</tr>\n";

					# Add in each and every file
					dirpath = config['page_root'] + "/" + url + "/";

					for file in os.listdir(dirpath):
						fstat = os.stat(dirpath + "/" + file);
						fname = os.fsdecode(file);
						http_response += "<tr>\n";
						http_response += "<td></td>\n";
						http_response += "<td>\n";

						# Directories put a "/" at the end.
						if os.path.isdir(dirpath + "/" + file):
							http_response += "<a href = \"./" + fname + "/\">" + fname + "/</a>\n";
						else:
							http_response += "<a href = \"./" + fname + "\">" + fname + "</a>\n";

						http_response += "</td>\n";
						http_response += "<td>" + datetime.fromtimestamp(fstat.st_mtime).strftime('%Y-%m-%d %H:%M') + "</td>\n";

						# Size isn't shown if it's a directory
						if os.path.isdir(dirpath + "/" + file):
							http_response += "<td style = \"text-align: right;\">-</td>\n";
						else:
							http_response += "<td style = \"text-align: right;\">" + friendly_size(fstat.st_size) + "</td>\n";

						http_response += "</tr>\n";

					# Another horizontal line
					http_response += "<tr><th colspan = \"5\"><hr></th></tr>\n";

					# Close. We are done here.
					http_response += "</table>\n";
					http_response += "</body>\n";
					http_response += "</html>\n";

	# ---------------------------------------------------------------------
	# POST data parsing                                                {{{2
	# ---------------------------------------------------------------------

	if is_post:
		print("[POST] RECEIVED")

		# Looks like we're doing this completely manually.
		# Scan for the \r\n\r\n
		i = 0
		while True:
			substr_parse = buffer[i:i + 4].decode("utf-8");
			if (substr_parse == "\r\n\r\n"):
				break;
			i += 1;

		# Extract header
		req_text = buffer[:i].decode("utf-8");

		# Get Content Length
		cl_scan = re.findall("Content-Length: (.*?)\r\n", req_text);
		content_length = 0;

		for num in cl_scan:
			content_length = int(num);

		total = 0
		buffer = bytes('', 'UTF-8');

		print("Receiving " + str(content_length) + " bytes...");

		# Behold
		while (total < content_length):
			request = client_connection.recv(content_length, socket.MSG_WAITALL);
			#request = client_connection.recv(1024);

			#print(request);

			if not request:
				break;

			buffer += request;
			total += len(request);
			print(len(request));
			sys.stdout.write("\rDownloading: " + str(total) + "/" + str(content_length) + "\n");

			#if len(request) < 1024:
			#	break;

		sys.stdout.write("\n");

		print("yes")

		# It's going to have some header information. Let's extract it.
		i = 0;
		while True:
			substr_parse = buffer[i:i + 4].decode("utf-8");
			if (substr_parse == "\r\n\r\n"):
				break;
			i += 1;
			if i >= content_length - 4:
				break;

		# Let's also get rid of the final line
		j = content_length - 4;
		while True:
			substr_parse = buffer[j:j + 2].decode("utf-8");
			if (substr_parse == "\r\n"):
				break;
			j -= 1;
			if (j <= 0):
				break;

		if (i < content_length):
			header = buffer[:i].decode("utf-8");
			print(header);

			# Extract filename
			fns = re.findall("filename=\"(.*?)\"", header);

			if fns:
				fn = fns[0];
			else:
				fn = "upload.dat";

			# Write the buffer to a file.
			with open(config['uploads'] + "/" + fn, 'wb') as fp:
				fp.write(buffer[i + 4:j])

	response_buffer = http_response.encode();
	response_buffer += content_buffer;
	
	# Send the response and close the connection
	client_connection.sendall(response_buffer);
	client_connection.close();

#
# server_run
#
# Description:
#     Takes a configuration JSON object and starts up a listener server. This
#     function should be the very last function called, as it will not break
#     until the server is taken down.
#
#     You can access the server in a web browser at http://host:port/
#

def server_run(config):
	# Configure the server
	HOST = config['host'];
	PORT = config['port'];

	# Allocate a server socket
	listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
	listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1);
	listen_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1);

	# Bind the socket to a port
	listen_socket.bind((HOST, PORT));

	# Listen for incoming connections
	listen_socket.listen(10);
	print('[LOG] Server Up');

	while True:
		client_connection, client_address = listen_socket.accept();

		threading.Thread(target = read_from_client, args = (config, client_connection, client_address)).start();

# -----------------------------------------------------------------------------
# Main Function                                                            {{{1
# -----------------------------------------------------------------------------

#
# main
#
# Description:
#     Entry for the program. Sets up the server and prepares any other data
#     that may be needed for the server to run.
#

def main(argc, argv):
	# Read from the JSON config file
	config = config_open('config.json');

	# Execute the server
	server_run(config);


# -----------------------------------------------------------------------------
# C-like Main Function Wrapper                                             {{{1
# -----------------------------------------------------------------------------

# This is just to give our program a C-like feel.

# Call Main
sys.exit(
	main(len(sys.argv), sys.argv)
)
