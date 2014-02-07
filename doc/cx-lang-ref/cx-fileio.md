---
layout: page
title: "Cx-fileio"
date: 2014-31-01 11:35
comments: true
sharing: true
footer: true
---

File access relies on standard members of the file basic object types. Below is the list of standard object members that provide basic file IO.

## Access

### bool file::open(char *filename, char *mode)
(function) opens a file

Opens a file indicated by `filename` and returns a Boolean indicating success or failure. `mode` is used to determine the file access mode.

#### Parameters
`filename` file name to associate the file stream to
`mode` character string determining file access mode

<table>
<tr>
<th> File access <br />mode string
</th>
<th> Meaning
</th>
<th> Explanation
</th>
<th> Action if file <br /> already exists
</th>
<th> Action if file <br /> does not exist
</th></tr>
<tr>
<td> <code>"r"</code>
</td>
<td> read
</td>
<td> Open a file for reading
</td>
<td> read from start
</td>
<td> failure to open
</td></tr>
<tr>
<td> <code>"w"</code>
</td>
<td> write
</td>
<td> Create a file for writing
</td>
<td> destroy contents
</td>
<td> create new
</td></tr>
<tr>
<td> <code>"a"</code>
</td>
<td> append
</td>
<td> Append to a file
</td>
<td> write to end
</td>
<td> create new
</td></tr>
<tr>
<td> <code>"r+"</code>
</td>
<td> read extended
</td>
<td> Open a file for read/write
</td>
<td> read from start
</td>
<td> error
</td></tr>
<tr>
<td> <code>"w+"</code>
</td>
<td> write extended
</td>
<td> Create a file for read/write
</td>
<td> destroy contents
</td>
<td> create new
</td></tr>
<tr>
<td> <code>"a+"</code>
</td>
<td> append extended
</td>
<td> Open a file for read/write
</td>
<td> write to end
</td>
<td> create new
</td></tr>
<tr>
<td colspan="5"> File access mode flag <code>"b"</code> can optionally be specified to open a file in binary mode. This flag has effect only on Windows systems. <br /> On the append file access modes, data is written to the end of the file regardless of the current position of the file position indicator.
</td></tr>
</table>

#### Return value
Boolean `true` on success, `false` on failure

#### Example
``` cpp
int main(){
	file output;
	
	// if open fails, print error
	if(!output.open("filename", "w")){
		perror("open");
	} else {
		output.puts("Hello, File!");
		output.close();
	}

	return 0;
}
```
   
### bool file::reopen(char *filename, char *mode)
(function) open an existing stream with a different filename

Reassigns an existing file stream to a different file identified by `filename` using specified mode. `mode` is used to determine the new file access mode. 

#### Parameters
`filename` file name to associate the file stream to
`mode` character string determining file access mode

<table>
<tr>
<th> File access <br />mode string
</th>
<th> Meaning
</th>
<th> Explanation
</th>
<th> Action if file <br /> already exists
</th>
<th> Action if file <br /> does not exist
</th></tr>
<tr>
<td> <code>"r"</code>
</td>
<td> read
</td>
<td> Open a file for reading
</td>
<td> read from start
</td>
<td> failure to open
</td></tr>
<tr>
<td> <code>"w"</code>
</td>
<td> write
</td>
<td> Create a file for writing
</td>
<td> destroy contents
</td>
<td> create new
</td></tr>
<tr>
<td> <code>"a"</code>
</td>
<td> append
</td>
<td> Append to a file
</td>
<td> write to end
</td>
<td> create new
</td></tr>
<tr>
<td> <code>"r+"</code>
</td>
<td> read extended
</td>
<td> Open a file for read/write
</td>
<td> read from start
</td>
<td> error
</td></tr>
<tr>
<td> <code>"w+"</code>
</td>
<td> write extended
</td>
<td> Create a file for read/write
</td>
<td> destroy contents
</td>
<td> create new
</td></tr>
<tr>
<td> <code>"a+"</code>
</td>
<td> append extended
</td>
<td> Open a file for read/write
</td>
<td> write to end
</td>
<td> create new
</td></tr>
<tr>
<td colspan="5"> File access mode flag <code>"b"</code> can optionally be specified to open a file in binary mode. This flag has effect only on Windows systems. <br /> On the append file access modes, data is written to the end of the file regardless of the current position of the file position indicator.
</td></tr>
</table>

#### Return value
Boolean `true` on success, `false` on failure

#### Example
``` cpp
int main(){
	file output;
	
	// if open fails, print error
	if(!output.open("filename", "w")){
		perror("open");
	} else {
		output.close();
		if(!output.reopen("newfilename", "w"))){
			perror("reopen");
		} else {
			output.puts("Hello, NewFile!");
			output.close();
		}
	}

	return 0;
}
```
 
### bool file::close(void)
Closes the given file stream. Any unwritten buffered data are flushed to the OS. Any unread buffered data are discarded. 

Whether or not the operation succeeds, the stream is no longer associated with a file, and the buffer allocated by file::setbuf or file::setvbuf, if any, is also disassociated and deallocated if automatic allocation was used.

### int file::flush(void)
 synchronizes an output stream with the actual file 
(function)   

### int file::wide(int mode)
 switches a file stream between wide character I/O and narrow character I/O 
(function)

If mode > 0, attempts to make stream wide-oriented. If mode < 0, attempts to make stream byte-oriented. If mode==0, only queries the current orientation of the stream. 

 Parameters

stream  -  pointer to the C I/O stream to modify or query  
mode  -  integer value greater than zero to set the stream wide, less than zero to set the stream narrow, or zero to query only  

 Return value

An integer greater than zero if the stream is wide-oriented after this call, less than zero if the stream is byte-oriented after this call, and zero if the stream has no orientation.    
 
### void file::setbuf(char *buffer)
 sets the buffer for a file stream 
(function)

Sets the internal buffer to use for I/O operations performed on the C stream stream. 

If buffer is not null, equivalent to std::setvbuf(stream, buffer, _IOFBF, BUFSIZ) 

If buffer is null, equivalent to std::setvbuf(stream, NULL, _IONBF, 0), which turns off buffering. 

 Parameters

stream  -  the file stream to set the buffer to.  
buffer  -  pointer to a buffer for the stream to use. If NULL is supplied, the buffering is turned off. If not null, must be able to hold at least BUFSIZ characters  

 Return value

(none) 
   
 
### int file::setvbuf(char *buffer, int mode, int size)
 sets the buffer and its size for a file stream 
(function)   

## Direct input/output 

file::read
 reads from a file 
(function)   

file::write
 writes to a file 
(function)  


## Unformatted input/output 
### Narrow character 


file::getc
 gets a character from a file stream 
(function)   
 


file::gets
 gets a character string from a file stream 
(function)   
 


file::putc
 writes a character to a file stream 
(function)   
 


file::puts
 writes a character string to a file stream 
(function)   
 


file::getchar
 reads a character from stdin 
(function)   
 


char *gets(void)
 reads a character string from stdin 
(function)   
 


char putchar(char c)
 writes a character to stdout 
(function)   
 


int puts(char *str)
 writes a character string to stdout 
(function)   
 


file::ungetc
 puts a character back into a file stream 
(function)   

Wide character 
 
 


file::getwc
 gets a wide character from a file stream 
(function)   
 


file::getws
 gets a wide string from a file stream 
(function)   
 


file::putwc
 writes a wide character to a file stream 
(function)   
 


file::putws
 writes a wide string to a file stream 
(function)   
 


wchar getwchar(void)
 reads a wide character from stdin 
(function)   
 


wchar putwchar(wchar wc)
 writes a wide character to stdout 
(function)   
 


file::ungetwc
 puts a wide character back into a file stream 
(function)  
