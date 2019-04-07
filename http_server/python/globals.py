#
# HTTP Server - Globals
#
# Description:
#     Defines all global variables that are used throughout the server's life.
#
# Authors:
#     Clara Nguyen & Rach Offutt
#

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
};

# Icons for the directory lister.
ICON = {
	"back"   : "/icons/back.gif",
	"folder" : "/icons/folder.gif",
	"generic": "/icons/generic.gif"
};
