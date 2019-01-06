#include "shell.hpp"

namespace cn_fs {
	shell::shell() {
		//Set the default shell prompt
		prompt = "sh> ";
	}

	shell::shell(const char* _prompt) {
		//Set a custom shell prompt
		prompt = _prompt;
	}

	/*
	 * cn_fs::shell::command_add
	 *
	 * Adds a valid command to the shell. The command will have a function
	 * pointer that will take a copy of the command string whenever the
	 * "interpret" function is called.
	 */

	void shell::command_add(const char* cmd, int (*func)(vector<string>&)) {
		map<string, int (*)(vector<string>&)>::iterator ii;
		ii = cmd_map.find(cmd);

		//If the command doesn't already exist, add it.
		if (ii == cmd_map.end()) {
			cmd_map.insert(make_pair<string, int (*)(vector<string>&)>(
				cmd,
				func
			));
		}
		else {
			//Already exists? Report it.
			cn_fs::util::log_error(
				"[ERROR] Command %s already exists!\n",
				cmd
			);
		}
	}

	/*
	 * cn_fs::shell::interpret
	 *
	 * Interprets a command and returns a status from that function. This only
	 * accepts commands added via "command_add". On an invalid command, it will
	 * return "1". "0" means a success. The command "cmd" will contain the
	 * entire command, including arguments.
	 */

	int shell::interpret(const char* cmd) {
		if (cmd == NULL || cmd[0] == 0)
			return 1;

		map<string, int (*)(vector<string>&)>::iterator ii;
		vector<string> args;
		cn_fs::util::explode(args, cmd);

		ii = cmd_map.find(args[0]);

		if (ii != cmd_map.end()) {
			//Time to interpret it.
			return ii->second(args);
		}
		else {
			//Assume failure.
			cn_fs::util::log_error(
				"[ERROR] %s: command not found\n",
				args[0].c_str()
			);

			return 1;
		}
	}

	/*
	 * cn_fs::shell::run
	 *
	 * Runs the shell. Returns "0" when exited successfully (always).
	 */

	int shell::run() {
		string cmd;
		printf("%s", prompt.c_str());
		
		while (getline(cin, cmd)) {
			interpret(cmd.c_str());

			printf("\n%s", prompt.c_str());
		}

		return 0;
	}
}
