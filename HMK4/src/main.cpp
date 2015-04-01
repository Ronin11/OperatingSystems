#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <string>
#include <sys/wait.h>
#include <string>
#include <chrono>
#include <fcntl.h>
#include "terminal.h"

void child_execute(std::vector<std::string> data){
		std::vector<char*> arg;
		for(int i = 0; i < data.size() - 1; i++)
			arg.push_back(strdup(data[i].c_str()));
		arg.push_back(nullptr); //terminate with nullptr
		execvp(arg[0], arg.data());
}

//Send the data command to execvp
void execute(std::vector<std::string> data, std::vector<std::string> *history, std::chrono::microseconds *time){
	auto start = std::chrono::system_clock::now();
	auto pid = fork();
	if(pid < 0)
		std::cerr << "Could not fork " << std::endl;

	if (pid == 0)
		child_execute(data);

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

//Print out history
void print_history(std::vector<std::string> *history){
	for(int i = 0; i < history->size(); i++ )
		std::cout << 0 - (i-history->size()) << ": " << history->operator[](i) << std::endl;
}

//Execute history
void exec_history(std::vector<std::string> data, std::vector<std::string> &history, std::chrono::microseconds *time){
	if(std::stoi(data[1]) <= history.size() && std::stoi(data[1]) > 0)
		execute(parse(history[0 - (std::stoi(data[1]) - history.size())]), &history, time);

	else
		std::cout << "History Query is Out of Bounds" << std::endl;
}

//Print out time
void ptime(std::chrono::microseconds *time){
	int secs, millis, micros;
	secs = time->count()/1000000;
	millis = time->count()/1000 - secs*1000;
	micros = time->count() - millis*1000 - secs*1000000;
	std::cout << "Total Child Processes Time Taken: " << secs << " Seconds, " << millis << " Milliseconds, and " << micros << " Microseconds" << std::endl;

}

void redirection_recursion(std::vector<std::string> data, int output[2]){
	int input_new[2];
	pipe(input_new);
	std::vector<std::string> data_args = data;
	int def_out = dup(1);
	int def_in = dup(0);

	if(switchVal(data) != 4){

		close(input_new[1]);
		close(input_new[0]);
		auto pid = fork();
		if(pid < 0)
			std::cerr << "Could not fork" << std::endl;
		if(pid == 0){
			dup2(output[0], 0);
			dup2(output[1], 1);	
			child_execute(data);
			dup2(def_out, 1);
			dup2(def_in, 0);
		}
		waitpid(pid, nullptr, 0);
		close(output[1]);
		close(output[0]);
		exit(0);
	}

	else
	for(int i = data.size() - 1; i >= 0; i--){
			if(data[i] == "<"){
				auto pid = fork();
				if(pid < 0)
					std::cerr << "Could not fork" << std::endl;
				if(pid == 0){
						close(input_new[1]);
						auto file = open(data[i+1].c_str(), O_RDONLY, S_IRUSR);
						dup2(file, input_new[0]);
						dup2(output[1], input_new[1]);
						data_args.erase(data_args.begin()+i+1);
						data_args.erase(data_args.begin()+i);
						redirection_recursion(data_args, input_new);		
						close(file);
						close(input_new[0]);	
					}
				waitpid(pid, nullptr, 0);
				exit(0);
			}

			else if(data[i] == ">"){
				auto pid = fork();
				if(pid < 0)
					std::cerr << "Could not fork" << std::endl;
				if(pid == 0){
						close(input_new[0]);
						auto file = open(data[i+1].c_str(), O_RDWR | O_TRUNC | O_CREAT, S_IWUSR);
						dup2(file, input_new[1]);		
						data_args.erase(data_args.begin()+i+1);
						data_args.erase(data_args.begin()+i);
						redirection_recursion(data_args, input_new);		
						close(file);
						close(input_new[1]);
				}
				waitpid(pid, nullptr, 0);
				exit(0);
			}

			else if(data[i] == "|"){
				auto pid = fork();
				if(pid < 0)
					std::cerr << "Could not fork" << std::endl;
				if(pid == 0){
						close(input_new[1]);
						close(output[0]);
						dup2(output[1], 1);
						dup2(input_new[0], 0);
						data_args.erase(data_args.begin()+i+1);
						data_args.erase(data_args.begin()+i);
						redirection_recursion(data_args, input_new);
						auto pid = fork();
						if(pid < 0)
							std::cerr << "Could not fork" << std::endl;
						if(pid == 0){
							std::vector<std::string> temp;
							temp.push_back(data[i+1]);
							child_execute(temp);	
							close(input_new[0]);
							close(output[1]);
							dup2(def_out, 1);
							dup2(def_in, 0);
						}
					waitpid(pid, nullptr, 0);
				}
				waitpid(pid, nullptr, 0);
				exit(0);
			}
		}
}

void redirection(std::vector<std::string> data, std::vector<std::string> *history, std::chrono::microseconds *time){
	auto start = std::chrono::system_clock::now();
	auto pid = fork();
	if(pid < 0){
		std::cerr << "Could not fork " << std::endl;
	}
	if (pid == 0){
		int output[2];
		pipe(output);
		dup2(dup(0),output[0]);
		dup2(dup(1),output[1]);
		redirection_recursion(data, output);
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

		switch(switchVal(data)){
		//Quit
		case(0):
		{
			exit(0);
			break;
		};
		//History
		case(1):
		{
			print_history(&history);
			break;
		};
		//History exec
		case(2):
		{
			exec_history(data,history,&time);
			break;
		}
		//PTime
		case(3):
		{
			ptime(&time);
			break;
		};
		case(4):
		{
			redirection(data,&history,&time);
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
