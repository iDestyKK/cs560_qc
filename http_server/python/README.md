# HTTP Server - Python 3 Edition

## Synopsis
We were tasked with writing a webserver. So here is the Python 3 version. There
were quite a few oddities that made this project a bit less than trivial. Those
will be covered here.

## 1. Configuration
The application must be fine-tunable. As such, this webserver has a config JSON
that is named `config.json`. This file is loaded at run-time and contains data
on where essential directories are, as well as host and port values. This is
what it may look like:
```json
{
	"page_root": "./htdocs",
	"uploads": "./uploads",
	"host": "",
	"port": 28961
}
```
The following keys are required:
* **page\_root** - Root directory of all webpages.
* **uploads** - Directory to store uploads.
* **host** - Host address (Leave blank for "localhost")
* **port** - Port number for server

You can access the webserver's pages by going to `http://localhost:28961` (if
you are using the default configuration).

## 2. Python sockets
While we were not allowed to use HTTP.server, we *were* allowed to use Python
sockets, which is the entire reason this project was possible in the first
place. The server allocates a server socket, binds it to a port, and then
listens for incoming connections, just as the assignment states to do. Then,
after accepting incoming requests, it generates a response and returns it back
to the sender.

The server does this indefinitely until killed or until a fatal error occurs.

## 3. The struggle with a single threaded webserver (and solution)
Initially, the socket handled one request at a time. This sufficed for simple
requests. The `socket.recv` function has the server read a fixed number of
bytes (1024 for this build). The server must call this multiple times and
reconstruct the original request from those bytes.

### 3.1. The problem
Modern browsers, such as Google Chrome and Firefox, send multiple requests per
page load. Since the server had a single thread and was calling `socket.recv`
to reconstruct a request at a time, it couldn't handle multiple requests being
sent simultaneously.

The packets being sent were often interlaced between the multiple requests. For
instance, if Google Chrome sent 2 requests of 4096 bytes each, they both are
broken up into 2 groups of 4 requests of 1024 bytes each, which are sent to
the server. But they are sent all in one stream, and the server gets them all
in a first-come, first-serve (queue) way.
```
+--------------------+
|  Request   1    2  |
+--------------------+
|  Recv 1    |       |
|  Recv 2    |       | ____ Would get stuck here anticipating a fragment for
|    .            |  |      the first request.
|    .            |  |
|    .       |       |
|    .            |  |
|    .            |  |
|  Recv 8    |       |
+--------------------+
```
This means that the server could receive a few packets of the first request,
and then a few for the second. The **only** guarantee is the order of the
packets received for each request. So you will always get the first fragment of
the first request before the second.

When a single-threaded server is trying to parse packets for the first request,
it will get stuck at waiting if it gets a fragment from the second request.

### 3.2. The solution
The solution wasn't so obvious, and Google doesn't directly state it either.
But the solution was trivial... make the server multithreaded. This allowed
the requests to be split up on a per-thread basis, meaning that, while the
`socket.recv` function was blocked on some threads, other threads were able to
run it and receive bytes for their respective requests and reconstruct it.

The multithreaded solution also has the positive effect of allowing multiple
users to load pages and other content simultaneously.

## 4. Status Codes
The server communicates by using the HTTP protocol. When responding to requests
from browsers, it will generate content and send it back. But there are a few
status codes that had to be implemented for this server to be functional:

### 4.1. Status Code 200 (OK)
Status code 200 signals that the request was understood correctly and is able
to generate data to send back to the client without any problems.
```
HTTP/1.1 200 OK
Content-Type: text/html; charset=UTF-8
Content-Encoding: UTF-8
```

### 4.2. Status Code 308 (Permanent Redirect)
Status Code 308 signals that the following page will redirect to another page.
This status code comes in handy for forcing the browser to redirect. In our
case, it was used for allowing directory listing. This is covered more in the
next section.
```
HTTP/1.1 308 Permanent Redirect
Location: /
```

### 4.3. Status Code 404 (Not Found)
Status code 404 signals that the request is understood, but the content that
the client wants cannot be found, usually because the content literally doesn't
exist. It is common, in practice, to fake 404 messages for pages that casual
viewers are not supposed to see (GitHub does this for private repos).
```
HTTP/1.1 404 Not Found
```

## 5. Directory Listing
A blatant ripoff (and exact copy) of Apache's directory listing. If a directory
does not feature an `index.html` file, then the server will not throw a 404,
but rather create a webpage on-the-fly and return it instead. This page will
list all files in the current directory, links to all of them, and also a link
to go up a directory.

### 5.1. Issues regarding Directory Listing
As stated in Section 4.2 (Status Code 308), there were a few issues with
directory listing that required extra requests to be sent in order to work.

#### 5.1.1. Telling what is a directory and what isn't
Firstly, if the URL is simply the directory without the final `/`, then all
paths on the page that feature a `..` will associate the directory as one
higher. For instance, consider the URL: `http://localhost:28961/test_dir/test`.
Suppose that `test_dir/test` is a directory.

The link in there to go to the `Parent Directory` is defined as `../`. If it's
correctly defined, it should point to `http://localhost:28961/test_dir`.
However, because there is no final `/` in the URL, the link will point to
`http://localhost:28961/` instead.

#### 5.1.2. The solution
The problem is because the browser thinks that the `test` is a file instead of
a directory. To be fair, we do generate a page on-the-fly, so it makes sense
that the browser makes this assumption. The solution is to simply slap on a `/`
at the end of the URL.

However, we can't just go in and do a `url += '/'`... *yet*. The browser's URL
bar won't update. And even if we force it through Javascript, the links won't
update. What do we do? Return Status Code 308 and tell it to redirect the page
to the same page with the `/` appended on.

The request for the example above would look like this:
```
HTTP/1.1 308 Permanent Redirect
Location: /test_dir/test/
```

#### 5.2.1. Images
Apache's directory listings contain several GIFs that are put to the left side
of each file to help distinguish which is which (as well as `/` after directory
names). Since, at the time in development, MIME types weren't implemented, the
Directory listing was lacking images and only showed text.

#### 5.2.2. The solution
Go to the next section.

## 6. MIME Types
HTML isn't the only type that can be read, and this proved to be a feat that
required changing how the data of a webpage is read into the webserver. Prior,
the file at the specified path was treated as a text file.

When your web browser tries to reach `http://localhost:28961/index.html`, it
sends a GET request which points to `/index.html`. Obviously the file is HTML.
But what happens if we try to reach `http://localhost:28961/img/cheese.jpg`? It
completely flopped.
```bash
[GET] img/cheese.jpg
Exception in thread Thread-24:
Traceback (most recent call last):
  File "/usr/lib/python3.7/threading.py", line 917, in _bootstrap_inner
    self.run()
  File "/usr/lib/python3.7/threading.py", line 865, in run
    self._target(*self._args, **self._kwargs)
  File "main.py", line 149, in read_from_client
    http_response += fp.read(); #.replace('\n', '')
  File "/usr/lib/python3.7/codecs.py", line 322, in decode
    (result, consumed) = self._buffer_decode(data, self.errors, final)
UnicodeDecodeError: 'utf-8' codec can't decode byte 0xff in position 0: invalid start byte
```

The issue lies in the file being read in. The type of the file could be of a
binary type like `jpg`, `mp3`, `wav`, etc. Thus, the read procedure of the
server had to be rewritten to construct the HTTP response as a raw buffer of
bytes.

Whenever that issue was fixed, images loaded flawlessly. As a positive side
effect, MIME types such as `audio/wave`, `audio/ogg`, etc, were also
implemented. Here is the list of supported MIME types:
```python
# Web Page files
     : text/html,
html : text/html,
js   : text/javascript,
css  : text/css,
txt  : text/plain,
json : application/json,

# Images
gif  : image/gif,
jpeg : image/jpeg,
jpg  : image/jpeg,
png  : image/png,
svg  : image/svg+xml,
ico  : image/x-icon,

# Multimedia
wav  : audio/wave,
mp3  : audio/mpeg,
flac : audio/flac,
webm : audio/webm,
ogg  : audio/ogg,

# Default value. Just raw bytes
other: application/octet-stream
```
There is a test page at `page_root/media_test.html` which tests multimedia
types. There is also a test page at `page_root/image.html` which tests image
file types being loaded from cross-domain, and also locally stored. You may
also access these files by typing the paths to them stored on the server
directly.
