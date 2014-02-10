#ifndef CX_IOSTREAM_H
#define CX_IOSTREAM_H

#include <cstdio>
#include <string>
/*
class cx_iostream{
public:
	cx_iostream(){
		this->_iostream = nullptr;
	}

	cx_iostream(FILE *stream){
		this->_iostream = stream;
	}

	// write directly to stream
	size_t write(const void *data, const size_t &size, const int &count){
		return fwrite(data, size, count, _iostream);
	}

	bool open(const std::string &filename, const std::string &mode){
		this->_iostream = fopen(filename.c_str(), mode.c_str());
		return (this->_iostream != nullptr);
	}

private:
	FILE *_iostream;
};*/

#endif