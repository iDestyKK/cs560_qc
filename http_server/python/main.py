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
from globals import *
from helper import *
from response import response

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

# -----------------------------------------------------------------------------
# Server Code                                                              {{{1
# -----------------------------------------------------------------------------

#
# webpage_response
#
# Description:
#     Generates a response given urls.
#

def webpage_response(urls, config):
	# Append the path file
	for url in urls:
		ourl = url;
		url = unquote(url);

		# If it's a "/" and there is a "index.html", add it.
		if url == "" or url[-1:] == "/":
			if os.path.exists(config['page_root'] + "/" + url + "index.html"):
				url += "index.html";

		# Prepare the default HTTP header
		page_res = response();
		page_res.set_status(200);
		page_res.set_header("Content-Type", determine_MIME(url) + "; charset=UTF-8");
		page_res.set_header("Content-Encoding", "UTF-8");

		# Find the path and append the file to http_response
		try:
			page_res.set_content_from_file(
				config['page_root'] + "/" + url
			);

		except FileNotFoundError:
			# Change the HTTP response to a 404
			page_res.set_status(404);

		except IsADirectoryError:
			# Add location... only if there isn't a "/"
			if (url != "" and url[-1] != "/"):
				page_res.set_status(308);
				page_res.set_header("Location", os.path.basename(url) + "/");
			else:
				# Generate the webpage (directory listing) on-the-fly.
				page_res.set_content(
					generate_directory_listing(url, config)
				);

	# Return the response
	return page_res;

#
# parse_post
#
# Description:
#     Parses POST data.
#

#def parse_post(buffer):

#
# generate_directory_listing
#
# Description:
#     Will generate a valid HTML document containing data listing of a specific
#     directory.
#

def generate_directory_listing(path, config):
	# Generate a response that prints a webpage
	html = "<html>\n";

	# Head
	html += "<head>\n"
	html += "<title>Index of /" + path + "</title>\n"
	html += "</head>\n";

	# Body
	html += "<body>\n";
	html += "<h1>Index of /" + path + "</h1>\n";
	html += "<table>\n";

	# Columns
	html += "<tr>\n";
	html += "<th valign = \"top\"></th>\n";
	html += "<th>Name</th>\n";
	html += "<th>Last modified</th>\n";
	html += "<th>Size</th>\n";
	html += "<th>Description</th>\n";
	html += "</tr>\n";

	# The horizontal line
	html += "<tr><th colspan = \"5\"><hr></th></tr>\n";

	# Parent Directory Link
	html += "<tr>\n<td>";
	html += "<img src = \"" + ICON['back'] + "\" alt = \"icon\"/>"
	html += "</td>\n<td>\n<a href = \"../\">";
	html += "Parent Directory</a>\n<td></td>\n<td style = \"text-align: right\">-</td>\n";
	html += "<td></td>\n</tr>\n";

	# Add in each and every file
	dirpath = config['page_root'] + "/" + path + "/";

	for file in os.listdir(dirpath):
		fstat = os.stat(dirpath + "/" + file);
		fname = os.fsdecode(file);
		html += "<tr>\n";

		if os.path.isdir(dirpath + "/" + file):
			# Is Directory
			html += "<td><img src = \"" + ICON['folder'] + "\" alt = \"icon\"/></td>\n";
			html += "<td>\n";
			html += "<a href = \"./" + fname + "/\">" + fname + "/</a>\n";
		else:
			# Is file
			html += "<td><img src = \"" + ICON['generic'] + "\" alt = \"icon\"/></td>\n";
			html += "<td>\n";
			html += "<a href = \"./" + fname + "\">" + fname + "</a>\n";

		html += "</td>\n";
		html += "<td>" + datetime.fromtimestamp(fstat.st_mtime).strftime('%Y-%m-%d %H:%M') + "</td>\n";

		# Size isn't shown if it's a directory
		if os.path.isdir(dirpath + "/" + file):
			html += "<td style = \"text-align: right;\">-</td>\n";
		else:
			html += "<td style = \"text-align: right;\">" + friendly_size(fstat.st_size) + "</td>\n";

		html += "</tr>\n";

	# Another horizontal line
	html += "<tr><th colspan = \"5\"><hr></th></tr>\n";

	# Close. We are done here.
	html += "</table>\n";
	html += "</body>\n";
	html += "</html>\n";

	return html;

#
# read_from_client
#
# Description:
#     Threaded function that will read data from a connection, interpret it,
#     generate an appropriate response, and return. This function is called
#     per request.
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
	page_buf = response();

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

		# Generate a response.
		page_buf = webpage_response(refs, config);

	# ---------------------------------------------------------------------
	# POST data parsing                                                {{{2
	# ---------------------------------------------------------------------

	if is_post:
		print("[POST] RECEIVED")

		req_text = buffer.decode("utf-8");
		refs = re.findall("POST \/([^?]*?)[ ?#].*HTTP", req_text);

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
			#sys.stdout.write("\rDownloading: " + str(total) + "/" + str(content_length) + "\n");

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

		# Construct the response
		page_buf = webpage_response(refs, config);

	# Send "page_buf". Since it's guaranteed to be generated.
	client_connection.sendall(page_buf.generate_buffer());
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

		threading.Thread(
			target = read_from_client,
			args = (
				config,
				client_connection,
				client_address
			)
		).start();

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
