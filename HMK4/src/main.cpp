#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <string>
#include <sys/wait.h>
#include <string>
#include "terminal.h"
#include "timer.h"

using namespace std;

int main(int argc, char **argv) {
	char input[1024];
	while(true){
		std::cout << "RoninShell->>> ";
		std::string line = "stuff";
		std::getline(std::cin,line);
		std::vector<std::string> data = parse(line);
		for(std::string s : data)
			std::cout << s << std::endl;
		auto pid = fork();
		if(pid < 0){
			cerr << "Could not fork " << std::endl;
		}
		if (pid ==0){
			std::cout << "child" << std::endl;
			std::vector<char*> arg;
			arg.push_back(strdup(line.c_str())); //need to pass in the first command
			arg.push_back(nullptr); //terminate with nullptr

			execvp("ls", arg.data());
		} else {
			std::cout << "parent " << pid << std::endl;
		}
		//wait for pid to finish
		waitpid(pid, nullptr, 0);
	}
}
