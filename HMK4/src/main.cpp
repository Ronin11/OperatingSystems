#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <string>
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv) {
/*
 std::cout << std::endl;

 for(int i =0 ; i < argc;++i){
	 std::cout << argv[i] << std::endl;
 }
*/
	std::string line = "stuff";
	int status;
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
	return 0;
}
