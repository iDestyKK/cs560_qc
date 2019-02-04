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
	listen_socket.listen(1);
	print('[LOG] Server Up');

	while True:
		client_connection, client_address = listen_socket.accept();

		# Construct the buffer
		buffer = buffer_fill(client_connection);

		# Prepare the default HTTP header
		http_response  = "HTTP/1.1 200 OK\r\n";
		http_response += "Content-Type: text/html; charset=UTF-8\r\n";
		http_response += "Content-Encoding: UTF-8\r\n";
		http_response += "\r\n";

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
				# Log the GET request
				print("[GET] " + url);

				# Find the path and append the file to http_response
				try:
					with open(config['page_root'] + "/" + url, 'r') as fp:
						http_response += fp.read(); #.replace('\n', '')
				except FileNotFoundError:
					# Change the HTTP response to a 404
					http_response = "HTTP/1.1 404 Not Found\r\n\r\n";

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

			total = 0 #len(buffer);
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

		
		# Send the response and close the connection
		client_connection.sendall(http_response.encode());
		client_connection.close();

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
