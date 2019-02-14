# HTTP Server (Programming Assignment 1 - Spring 2019)

## Synopsis
Make a webserver that can load up webpages in the web browser. That's pretty
nifty.

## Language Choice
We are allowed to use any language we want. Since this has been done before
(...multiple times), there are multiple implementations here. Whether they are
done or not depends on the colour of the line:
```diff
- python (WIP)
- Node.js (From CN_GTAOSM. Needs rewriting but it works.)
- C++ (May not implement)
```

## Feature Breakdown
Separated by implementation. Think of it as a "TODO" list.

| | Python | Node.js | C++ |
| ---: | :---: | :---: | :---: |
| Code Base | ✅ | ✅ | ❌ |
| JSON Server Config | ✅ | ❌ | ❌ |
| Static HTML Loading | ✅ | ✅ | ❌ |
| Directory Listing | ✅ | ❌ | ❌ |
| Other types of files (img, etc) | ❌ | ❌ | ❌ |
| File Uploads | ✅ | ❌ | ❌ |

## Verified Operating System Support

| | Python | Node.js | C++ |
| ---: | :---: | :---: | :---: |
| Windows | ❓ | ❓ | ❓ |
| Mac OS X | ❓ | ❓ | ❓ |
| Linux | ✅ | ❓ | ❓ |

## Writeup
The goal is to implement a basic HTTP server that supports directory listing,
static HTML files, user file uploads. The server needs to run on a Linux server
environment, such as those supported in our lab machines. Specifically, your
web server needs to support the following features:
1. HTTP requests with query and header parsing
2. HTML page navigation
3. Static file transport allowing users to submit a file to the server side

### Design
Generally, the implementation of a HTTP server is based on socket programming
in this assignment. First, the code needs to allocate a server socket, bind it
to a port, and then listen for incoming connections. Next, the server accepts
an incoming client connection and parse the input data stream into a HTTP
request. Based on the request's parameters, it then forms a response and sends
it back to the client. In a loop, the server perform steps 2 and 3 for as long
as the server is running.

To observe what a web browser such as Firefox or chrome would send to a HTTP
server, you can use netcat to simulate a server. For instance, run the fake
server with **nc -l -p 10010** and then use your web browser to go to
http://hostname:10010. You should then see all HTTP request and headers the web
browser sends to the HTTP server. For example, you may see something like
following (using Chrome as the browser):
```
Connection: keep-alive
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36
Upgrade-Insecure-Requests: 1
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8
Accept-Encoding: gzip, deflate, br
Accept-Language: en-US;,en;q=0.9
```

In general, a valid HTTP response looks like this:
```
HTTP/1.0 200 OK
Content-Type: text/html
<html>
...
</html>
```

The first line contains the HTTP status code for the request. The second line
instructs the client (i.e. web browser) what type of data to expect (i.e.
mime-type). Each of these lines should be ended with `\r\n`. Additionally, it
is important that after the Content-Type line you include a blank line
consisting of only `\r\n`. Most web clients will expect this blank line before
parsing for the actual content.

Once you have implemented a webserver and verified it is working by accessing
web pages using a web browser such as Firefox or Chrome, and by using command
line tools such as curl or wget. For testing needs, we have provided three HTML
pages as samples, namely, index.html, page1.html, and page2.html for your
needs. If you place these three pages in a home directory, your server will be
able to serve the index.html page by default. This page contains links that
link to the second and third page, which you can use as testing benchmarks for
your webserver.

This programming assignment does not specify any programming language you
choose. You may choose any mainstream programming language such as Java,
Python, or C to implement the requirements. You do, however, are required to
provide detailed testing results in the form of screenshots on how to run your
code, what results you obtain, and how you tested your results to know that
they are correct.

We assume that if there is a page named "index.html" in your home folder, then
it will be served as the default page. Besides static HTML pages, you need to
study the HTTP protocol, such as the one available here:

https://www.w3.org/Protocols/HTTP/1.1/rfc2616bis/draft-lafon-rfc2616bis-03.html

So that you will only allow users to submit a basic HTTP form. This is usually
done by using a SUBMIT button on a web page, and the web server will record the
results into a local file. The details on the implementation will depend on
your design choices. You need to document your design choices in detail, and
provide necessary screenshots to illustrate your results.

__Note that, prepackaged HTTP server classes, such as Python http.server class,
should not be used for this project. Usually these classes provide much more
functions than what is required here, and a project that directly calls their
APIs will not be given project credits.__

**References:**

* Java socket programming guide: https://docs.oracle.com/javase/tutorial/networking/sockets/index.html
* Python socket programming guide: https://docs.python.org/3/library/socket.html
* HTTP reference: https://www.w3.org/Protocols/
