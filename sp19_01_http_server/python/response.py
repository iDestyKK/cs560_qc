#
# HTTP Server - Response class
#
# Description:
#     Response header for constructing proper HTTP responses to give back to
#     clients.
#
# Authors:
#     Clara Nguyen & Rach Offutt
#

# -----------------------------------------------------------------------------
# Global Status Codes                                                      {{{1
# -----------------------------------------------------------------------------

STATUS_CODE = {
	200: "OK",
	308: "Permanent Redirect",
	404: "Not Found"
};

# -----------------------------------------------------------------------------
# Response Class                                                           {{{1
# -----------------------------------------------------------------------------

class response:
	# Constructor
	def __init__(self):
		# Create the header
		self.header = {};
		self.status = 200;
		self.content = bytes('', 'UTF-8');

	#
	# response.set_status
	#
	# Description:
	#     Sets the status code for the current request.
	#

	def set_status(self, val):
		# Set the status code
		if val in STATUS_CODE:
			self.status = val;
		else:
			print("[Error] Invalid Response Value!");

	#
	# response.set_header
	#
	# Description:
	#     Sets a header value. If the value already exists, it is overwritten.
	#     The data put here is printed at the very top of the request when it
	#     generated and sent to the client.
	#

	def set_header(self, key, value):
		self.header[key] = value;

	#
	# response.set_content
	#
	# Description:
	#     Sets the content buffer to "data"
	#

	def set_content(self, data):
		self.content = data.encode();

	#
	# response.set_content_from_file
	#
	# Description:
	#     Imports all bytes from a file into the response content field.
	#

	def set_content_from_file(self, path):
		try:
			with open(path, 'rb') as fp:
				self.content += fp.read();
		except Exception as e:
			raise;

	#
	# response.construct_header
	#
	# Description:
	#     Generates the header string for an HTTP response.
	#

	def construct_header(self):
		# First line
		txt  = "HTTP/1.1 " + str(self.status) + " " + STATUS_CODE[self.status];
		txt += "\r\n";

		# Insert all header values
		for key in self.header:
			txt += key + ": " + self.header[key] + "\r\n";

		return txt;

	#
	# response.generate_buffer
	#
	# Description:
	#     Generates the final response to be sent to the client. This is not a
	#     string. It's returned as a UTF-8 buffer.
	#

	def generate_buffer(self):
		ret = bytes('', "UTF-8");

		# Convert the text header into a buffer
		ret += (self.construct_header() + "\r\n").encode();

		# Add the content to the buffer
		ret += self.content;

		return ret;
