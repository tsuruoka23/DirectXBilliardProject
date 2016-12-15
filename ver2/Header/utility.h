#pragma once

#include "stdafx.h"
#include <consoleapi.h>


class ConsoleViewer{
private:
	FILE* stream;

	// �W���o�͂��R���\�[���ɕ\�������悤�ɐݒ�
	bool initConsole() {
		errno_t err = freopen_s(&stream, "CONOUT$", "w", stdout);
		//errno_t err = freopen_s(&stream, "out.txt", "w", stdout);

		if (err != 0) {
			fprintf(stdout, "error on freopen\n");
			return false;
		}

		return true;
	}

public:
	ConsoleViewer() {
		AllocConsole();
		initConsole();
	}
	~ConsoleViewer() {
		if(stream != NULL) fclose(stream);
		FreeConsole();
	}

	void print(std::string str) {
		std::cout << str << std::endl;
	}

	void log(std::string str) {
		std::cout << str << std::endl;
	}

};

