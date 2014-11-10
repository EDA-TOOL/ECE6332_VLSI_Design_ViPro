#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include "userInput.h"

using namespace std;

class parser {
	public:
		parser();
		~parser();
		string openFile(char* file);
		int parseFile(char* file);
		void consumeComments(string& str);
		void consumeNewLines(string& str);
		int parseToken(string& str);
		int parseUserToken(string& str);

		//EX: SWEEP SAoffset 0.05 0.3 0.05 ./sweep.txt
		int parseSWEEPToken(string& str);
		int parseOPTIMIZEToken(string& str);
		int parseKNOBS(string& str);
		string checkCmd(string str);
		userInput getInputHandle();

	private:
		string   inputFile;
		userInput input_handle;
};


#endif
