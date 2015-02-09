/*
 * terminal.h
 *
 *  Created on: Jan 28, 2015
 *      Author: root
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include<sstream>
#include<vector>
#include<algorithm>

 struct{
 	std::string command;
 	std::string output;
 	std::string input;

 }pipeData;

std::vector<std::string> parse(std::string input){
	std::istringstream iss(input);
	std::vector<std::string> args;
	do{
		std::string s;
		iss >> s;
		args.push_back(s);
	} while(iss);
	return args;

}

int switchVal(std::vector<std::string> input){

std::string s = input[0];
std::transform(s.begin(), s.end(), s.begin(), ::tolower);
for(int i = 0; i < input.size(); i++){
		if(input[i] == "<" || input[i] == ">" || input[i] == "|")
			return 4;
	}
if(s == "quit" || s == "q" || s == "exit")
	return 0;
else if(s == "history")
	return 1;
else if(s == "^")
	return 2;
else if(s == "ptime")
	return 3;
else
	return 6;

}

#endif /* TERMINAL_H_ */
