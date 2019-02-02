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

	# Bind the socket to a port
	listen_socket.bind((HOST, PORT));

	# Listen for incoming connections
	listen_socket.listen(1);
	print('[LOG] Server Up');

	while True:
		client_connection, client_address = listen_socket.accept();
		request = client_connection.recv(1024);

		# Prepare the default HTTP header
		http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

		# Decode the request
		req_text = request.decode("utf-8");

		# Extract the referer (URLs)
		refs = re.findall("GET /(.*?) HTTP", req_text);

		# Append the path file
		for url in refs:
			# Log the GET request
			print("[GET] " + url);

			# Find the path and append the file to http_response
			try:
				with open(config['page_root'] + "/" + url, 'r') as fp:
					http_response += fp.read().replace('\n', '')
			except FileNotFoundError:
				# Change the HTTP response to a 404
				http_response = "HTTP/1.1 404 Not Found\r\n\r\n";
		
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
