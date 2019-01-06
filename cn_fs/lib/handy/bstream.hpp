/*
 * bstream - Buffered Streams
 *
 * Description:
 *     A custom interface for dealing with streams. Can probably be used like
 *     ifstream and ofstream.
 *
 * Author:
 *     Clara Nguyen (@iDestyKK)
 */

#ifndef __HANDY_BSTREAM__
#define __HANDY_BSTREAM__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace handy {
	typedef unsigned char BST_BYTE;

	/*
	 * FLAG DECLARATION
	 *
	 * These flags are essential for telling the bstream how to operate:
	 *
	 *   (  1) BST_READ  - Allows the stream to accept input.
	 *
	 *   (  2) BST_WRITE - Allows the stream to write output.
	 *
	 *   (  4) BST_ZERO  - Any new space allocated will be memset to 0. If
	 *                     disabled, it can give a performance boost. Your
	 *                     platform may set to 0 via realloc anyways.
	 *
	 *   (  8) BST_ERROR - Prevents segmentation faults when dealing with
	 *                     invalid data. Instead, the "append" functions will
	 *                     return a 0 in the event that they failed to run
	 *                     properly.
	 *
	 *   (128) BST_RISK  - Disables checks for a gain in performance. Assumes
	 *                     you know what you're doing.
	 */

	enum BST_FLAG {
		BST_READ    = 1,
		BST_WRITE   = 2,
		BST_ZERO    = 4,
		BST_ERROR   = 8,
		BST_RISK    = 128,
		BST_DEFAULT = 7
	};
	BST_FLAG operator|(BST_FLAG, BST_FLAG);

	#define __BST_FLAG_SET(fg) \
		(flags & fg)

	class bstream {
		public:
			bstream(BST_FLAG = BST_DEFAULT);
			bstream(size_t, BST_FLAG = BST_DEFAULT);
			bstream(int, BST_FLAG = BST_DEFAULT);
			~bstream();

			//Modifications
			int  append_file  (const char*);     /* Import a file */
			int  append_string(const char*);     /* Import a string */
			int  append_buffer(const bstream&);  /* Import a bstream */
			void clear();                        /* Clears the buffer */
			void resize(size_t);                 /* Resize the buffer */

			//Get Functions
			const size_t& size() const;   /* Size of buffer stream */
			const size_t& length() const; /* Ditto */

			//Get raw data in stream
			const BST_BYTE* data() const;
			      BST_BYTE* data();

			//Get raw data as any type
			template <typename T>
			T* data() {
				return reinterpret_cast<T*>(bytes);
			}
			
			template <typename T>
			const T* data() const {
				return reinterpret_cast<T*>(bytes);
			}

			//Get functions that get bytes
			const BST_BYTE& at(size_t) const;
			      BST_BYTE& at(size_t);

			//Get functions that get any type
			template <typename T>
			T& at(size_t i) {
				//TODO: Range check
				return *reinterpret_cast<T*>(&bytes[i]);
			}

			template <typename T>
			const T& at(size_t i) const {
				//TODO: Range check
				return *reinterpret_cast<T*>(&bytes[i]);
			}

			//Operator overloads
			BST_BYTE& operator[](size_t);
			const BST_BYTE& operator[](size_t) const;

		private:
			BST_BYTE *bytes;
			size_t    pos, sz;
			int       flags;

			//Private Functions
			inline void setup();
	};
}

#endif
