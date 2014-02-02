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

### bool open(char *filename, char *mode)
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
   
### bool reopen(char *filename, char *mode)
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
 
## bool close(void)
Closes the given file stream. Any unwritten buffered data are flushed to the OS. Any unread buffered data are discarded. 

Whether or not the operation succeeds, the stream is no longer associated with a file, and the buffer allocated by std::setbuf or std::setvbuf, if any, is also disassociated and deallocated if automatic allocation was used.


flush
 synchronizes an output stream with the actual file 
(function)   

wide
 switches a file stream between wide character I/O and narrow character I/O 
(function)   
 
setbuf
 sets the buffer for a file stream 
(function)   
 
setvbuf
 sets the buffer and its size for a file stream 
(function)   

## Direct input/output 

read
 reads from a file 
(function)   

write
 writes to a file 
(function)  


## Unformatted input/output 
### Narrow character 


getc
 gets a character from a file stream 
(function)   
 


gets
 gets a character string from a file stream 
(function)   
 


putc
 writes a character to a file stream 
(function)   
 


puts
 writes a character string to a file stream 
(function)   
 


getchar
 reads a character from stdin 
(function)   
 


gets
 reads a character string from stdin 
(function)   
 


putchar
 writes a character to stdout 
(function)   
 


puts
 writes a character string to stdout 
(function)   
 


ungetc
 puts a character back into a file stream 
(function)   

Wide character 
 
 


getwc
 gets a wide character from a file stream 
(function)   
 


getws
 gets a wide string from a file stream 
(function)   
 


putwc
 writes a wide character to a file stream 
(function)   
 


putws
 writes a wide string to a file stream 
(function)   
 


getwchar
 reads a wide character from stdin 
(function)   
 


putwchar
 writes a wide character to stdout 
(function)   
 


ungetwc
 puts a wide character back into a file stream 
(function)  
