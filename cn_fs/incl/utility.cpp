#include "utility.hpp"

namespace cn_fs {
	namespace util {
		/*
		 * cn_fs::util::log_error
		 *
		 * Logs an error. Dumps to stderr and a log file.
		 */

		void log_error(const char* fmt, ...) {
			va_list args;
			va_start(args, fmt);
			vfprintf(stderr, fmt, args);
			va_end(args);
		}

		/*
		 * cn_fs::util::explode
		 *
		 * Explodes a string "cmd" into arguments that are valid for a shell.
		 * The results are stored in "dest".
		 */

		void explode(vector<string>& dest, const char* cmd, const char* esc) {
			size_t cpos, il, ir, len, esc_len;
			bool in_quotes;
			string tmp;

			esc_len = sizeof(esc);

			//Clean the destination vector
			dest.clear();

			//Set up the buffer scan
			cpos = 0;
			in_quotes = false;
			len = strlen(cmd);
			il = 0;

			for (; il < len; il++) {
				//Skip blanks, newlines, and carriage returns
				if (__on_escape(cmd[il], esc, len))
					continue;

				//We are at a string. Let's parse it.
				tmp = "";

				//Take caution with quotations.
				in_quotes = (cmd[il] == '"');

				//Loop through until we reach a termination character.
				if (in_quotes) {
					for (ir = il + 1; ir < len; ir++) {
						//Copy the string until " is found without \"
						if (cmd[ir] == '"' && cmd[ir - 1] != '\\')
							break;

						tmp += cmd[ir];
					}
				}
				else {
					for (ir = il; ir < len; ir++) {
						//Copy the string until an escape character is found
						if (__on_escape(cmd[ir], esc, len))
							break;

						tmp += cmd[ir];
					}
				}

				//Parsing Done.
				dest.push_back(tmp);

				//Slide the left over.
				il = ir;
			}
		}

		/*
		 * cn_fs::util::__on_escape
		 *
		 * Returns "true" on a character that the shell should escape,
		 * excluding quotation marks.
		 */

		bool __on_escape(char c, const char* esc, size_t len) {
			unsigned int i = 0;

			for (; i < len; i++) {
				if (c == esc[i])
					return true;
			}
			return false;
			//return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
		}
	}
}
