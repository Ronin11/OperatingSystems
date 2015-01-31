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




#endif /* TERMINAL_H_ */
