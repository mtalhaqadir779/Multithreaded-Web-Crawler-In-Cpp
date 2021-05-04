**Read Me**
1.	Put at least one html file in html docs. You can use the HTML files provided in seeds folder
2.	Use make command to compile the code.

**To Compile**
•	Either use Makefile  OR
•	g++ -o crawler crawler.cpp -lpthread -L/usr/local/lib/ -lcurlpp -lcurl -I/usr/local/include

**To Run**
•	./crawler


**Info for Understanding**

**Headers**
1.	iostream: standard input-output stream
2.	fstream: library allows us to work with files (Require both <iostream> and <fstream> headers)
3.	cstdio: header file that contains all of the old C functions to print stuff and write to files (printf(), fprintf(), fopen() etc). iostream contains all of the C++ streams        to do that same thing.
4.	cstdlib: general purpose standard library with functions involving memory allocation, process control, conversions, and others
5.	cstring: C++ <cstring> header file declares a set of functions to work with C style string (null terminated byte strings).
6.	string: To use strings, you must include an additional header file in the source code
7.	list: Header to use linked lists. List containers are implemented as doubly linked lists
8.	dirent: dirent structure, defined in the dirent.h file, is used for directory access operations.
9.	pthread: for multithreading programming
10.	unistd: unistd.h is the name of the Header file that provides access to the POSIX Operating System API
11.	curllpp: cURLpp is a C++wrapper of libcurl, so cURLpp needs libcurl to be installed already.

 
**Pthread_create()**
https://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_create.html
