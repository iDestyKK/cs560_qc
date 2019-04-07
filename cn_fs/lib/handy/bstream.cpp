#include "bstream.hpp"

namespace handy {
	//BST_FLAG Enum Operator Overload
	BST_FLAG operator|(BST_FLAG a, BST_FLAG b) {
		return static_cast<BST_FLAG>(
			static_cast<unsigned int>(a) | static_cast<unsigned int>(b)
		);
	}

	bstream::bstream(BST_FLAG f) {
		//Setup the stream
		setup();
		flags = f;
	}

	bstream::bstream(size_t new_size, BST_FLAG f) {
		//Setup the stream
		setup();
		flags = f;

		//Set up the buffer to have "new_size" bytes.
		resize(new_size);
	}

	bstream::bstream(int new_size, BST_FLAG f) {
		//Setup the stream
		setup();
		flags = f;

		//Set up the buffer to have "new_size" bytes.
		resize(new_size);
	}

	bstream::~bstream() {
		//Clear the stream
		clear();
	}

	//Modifications
	int bstream::append_file(const char* fname) {
		size_t old_size = sz;
		long fsz;
		FILE* fp;

		//Get information on the file
		fp = fopen(fname, "rb");
		if (__BST_FLAG_SET(BST_ERROR) && fp == NULL)
			return 0;

		fseek(fp, 0, SEEK_END);
		fsz = ftell(fp);

		//Configure the file and bstream
		fseek(fp, 0, SEEK_SET);
		resize(old_size + fsz);

		//Copy the file into the stream
		fread(&bytes[old_size], sizeof(BST_BYTE), fsz, fp);

		//We are done.
		fclose(fp);
		return 1;
	}

	int bstream::append_string(const char* str) {
		//We can't append a null string.
		if (__BST_FLAG_SET(BST_ERROR) && str == NULL)
			return 0;

		size_t old_size = sz,
		       slen     = strlen(str);

		resize(old_size + slen);

		//Copy the string in.
		memcpy(&bytes[old_size], &str[0], slen);

		return 1;
	}

	int bstream::append_buffer(const bstream& ref_obj) {
		//We can't append a null buffer
		//This is the only error check that runs even without BST_ERROR
		if (ref_obj.data() == NULL)
			return 0;

		size_t old_size = sz,
		       blen     = ref_obj.size();

		resize(old_size + blen);

		//Copy the buffer data in.
		memcpy(&bytes[old_size], &ref_obj.data()[0], blen);

		return 1;
	}

	void bstream::clear() {
		if (bytes != NULL)
			free(bytes);
		bytes = NULL;
		sz = 0;
	}

	void bstream::resize(size_t new_size) {
		size_t old_size;
		old_size = sz;

		bytes = (BST_BYTE*) realloc(bytes, new_size);

		//Fill the new space with 0's.
		if (__BST_FLAG_SET(BST_ZERO) && new_size > old_size)
			memset(&bytes[old_size], 0, new_size - old_size);

		sz = new_size;
	}

	//Get Functions
	const size_t& bstream::size() const {
		return sz;
	}

	const size_t& bstream::length() const {
		return sz;
	}

	const BST_BYTE* bstream::data() const {
		return bytes;
	}

	BST_BYTE* bstream::data() {
		return bytes;
	}

	BST_BYTE& bstream::at(size_t i) {
		//TODO: Range check
		return bytes[i];
	}

	const BST_BYTE& bstream::at(size_t i) const {
		//TODO: Range check
		return bytes[i];
	}
	
	//Operator Overloads
	BST_BYTE& bstream::operator[](size_t i) {
		return bytes[i];
	}

	const BST_BYTE& bstream::operator[](size_t i) const {
		return bytes[i];
	}

	inline void bstream::setup() {
		bytes = NULL;
		pos = 0;
		sz = 0;
		flags = 0;
	}
}
