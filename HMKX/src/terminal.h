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

bool checkForRedirection(std::vector<std::string> input){
	for(int i = 0; i < input.size(); i++){
		if(input[i] == '<' || input[i] == '>' || input[i] == '|')
			return true;
	}
	else
		return false;
}

int switchVal(std::string s){
std::transform(s.begin(), s.end(), s.begin(), ::tolower);
if(s == "quit" || s == "q" || s == "exit")
	return 0;
else if(s == "history")
	return 1;
else if(s == "^")
	return 2;
else if(s == "ptime")
	return 3;
else
	return 5;

}

#endif /* TERMINAL_H_ */