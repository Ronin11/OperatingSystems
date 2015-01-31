#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <string>
#include <sys/wait.h>
#include <string>
#include <chrono>
#include "terminal.h"

//Send the data command to execvp
void execute(std::vector<std::string> data, std::vector<std::string> *history, std::chrono::microseconds *time){
	auto start = std::chrono::system_clock::now();
	auto pid = fork();
	if(pid < 0){
		std::cerr << "Could not fork " << std::endl;
	}
	if (pid ==0){
		std::vector<char*> arg;
		for(int i = 0; i < data.size() - 1; i++)
			arg.push_back(strdup(data[i].c_str()));
		arg.push_back(nullptr); //terminate with nullptr
		execvp(arg[0], arg.data());
	}
	//Add Command to History
	std::string temp = data[0];
	for(int i = 1; i < data.size(); i++)
		temp.append(" " + data[i]);
	if(!temp.empty())
		history->push_back(temp);
	//wait for pid to finish
	waitpid(pid, nullptr, 0);
	auto stop = std::chrono::system_clock::now();
	*time += stop - start;
}

int main(int argc, char **argv) {
	while(true){
		std::cout << "RoninShell->>> ";
		std::string line = "stuff";
		std::getline(std::cin,line);
		std::vector<std::string> data = parse(line);
		static std::vector<std::string> history;
		static std::chrono::microseconds time;

		switch(switchVal(data[0])){
		//Quit
		case(0):
		{
			exit(0);
			break;
		};
		//History
		case(1):
		{
			for(int i = 0; i < history.size(); i++ )
				std::cout << 0 - (i-history.size()) << ": " << history[i] << std::endl;
			break;
		};
		//History exec
		case(2):
		{
			if(std::stoi(data[1]) <= history.size() && std::stoi(data[1]) > 0)
				execute(parse(history[0 - (std::stoi(data[1]) - history.size())]), &history, &time);

			else
				std::cout << "History Query is Out of Bounds" << std::endl;
			break;
		}
		//PTime
		case(3):
		{
			int secs, millis, micros;
			secs = time.count()/1000000;
			millis = time.count()/1000 - secs*1000;
			micros = time.count() - millis*1000 - secs*1000000;
			std::cout << "Total Child Processes Time Taken: " << secs << " Seconds, " << millis << " Milliseconds, and " << micros << " Microseconds" << std::endl;

			break;
		};
		//Regular Command
		default:
		{
			execute(data,&history,&time);
		};
		}
	}
}
