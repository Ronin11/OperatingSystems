#include <iostream>
#include <random>
#include <algorithm>
#include "queues.h"

int const runtime = 1000;

bool add_task(float freq){
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> norm(0.0, 1.0);
	if(freq >= norm(mt))
		return true;
	else
		return false;
}

void average(std::vector<float> data){
	float average = std::accumulate(data.begin(), data.end(),0.0)/data.size();
	std::cout << "\tAverage: " <<  average << std::endl;
}
void minimum(std::vector<float> data){
	float minimum = data[0];
	for(int i = 0; i < data.size(); i++)
		if(minimum > data[i])
			minimum = data[i];
	std::cout << "\tMinimum: " <<  minimum << std::endl;
}
void maximum(std::vector<float> data){
	float maximum = data[0];
	for(int i = 0; i < data.size(); i++)
		if(maximum < data[i])
			maximum = data[i];
	std::cout << "\tMaximum: " <<  maximum << std::endl;
}
void stddev(std::vector<float> data){
	float stddev;
	float mean = std::accumulate(data.begin(), data.end(),0)/data.size();
	float squared = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
	stddev = std::sqrt(squared/data.size() - mean * mean);
	std::cout << "\tStdDev: " <<  stddev << std::endl;
}
void print(std::vector<float> data){
	average(data);
	minimum(data);
	maximum(data);
	stddev(data);
}
void print_stats(std::vector<float> latency, std::vector<float> response, std::vector<float> usage, std::vector<float> throughput){
	std::cout << "Latency: " << std::endl;
	print(latency);
	std::cout << "Response Time: " << std::endl;
	print(response);
	std::cout << "Utilization: "  << std::endl;
	print(usage);
	std::cout << "Throughput: " << std::endl;
	print(throughput);
	exit(0);
}

int main(int argc, char* argv[]){
	int cpus = 1;
	int ios = 1;
	float context_cost = 0.0;
	float mix = 0.5;
	float freq = 0.5;

	float time = 0.0;
	float  quantum = 100;

	int scheduler = 0;

	event_queue readyqueue;
	event_queue executing;
	std::vector<task> tasks;
	std::vector<bool> available_devices;
	std::vector<float> latency;
	std::vector<float> response;
	std::vector<float> usage;
	std::vector<float> throughput;
	
	//Change stuff from command line
	for(int i = 0; i < argc; i++){
		std::string s = argv[i];
		if(s == "-f")
			freq = std::stof(argv[i+1]);
		else if(s == "-c")
			cpus = std::stoi(argv[i+1]);
		else if(s == "-i")
			ios = std::stoi(argv[i+1]);
		else if(s == "-s")
			context_cost = std::stof(argv[i+1]);
		else if(s == "-t")
			mix = std::stof(argv[i+1]);
		else if(s == "-q")
			quantum = std::stof(argv[i+1]);
		else if(s == "-p")
			scheduler = std::stoi(argv[i+1]);
	}

	const int maxcpus = cpus;
	int resources = cpus+ios;

	float responsetime = 0;
	//Create the used devices list.
	for(int i = 0; i < ios+1; i++){
		available_devices.push_back(true);
	}
		while(true){//FIFO Scheduler with modifications for the different schedulers
			int used_devices = 0;
			for(int i = 1; i < available_devices.size()-1; i++)
				if(!available_devices[i])
					used_devices++;
			usage.push_back((float)(used_devices+cpus)/(float)(maxcpus+available_devices.size())+0.1);
			std::sort(executing.begin(), executing.end());
			if(executing.size() > 0){
			event e = executing.pop();
				cpus++;
				time = e.get_time();
				if(time >= runtime)
					print_stats(latency, response, usage, throughput);
				else if(e.get_type() == ioburst_type && responsetime == 0){
					available_devices[e.get_device()] = true;
					responsetime = time;
				}
				else if(e.get_type() == ioburst_type)
					available_devices[e.get_device()] = true;
				else if(e.get_type() == task_done){
					e.get_task()->set_end_time(time);
					e.get_task()->set_response_time(responsetime);
					latency.push_back(e.get_task()->get_start_time()-e.get_task()->get_end_time());
					response.push_back(responsetime);
					throughput.push_back((e.get_task()->get_start_time()-e.get_task()->get_end_time())/runtime);
					responsetime = 0;
				}
			}
			if(readyqueue.size() > 0){
			if(cpus > 0 && available_devices[readyqueue.peek().get_device()]){
				cpus--;
				if(readyqueue.peek().get_device() > 0 )
					available_devices[readyqueue.peek().get_device()] = false;
				executing.push(readyqueue.pop());
				
			}
		}
			
			if(add_task(freq)){
				task t = task(ios,mix);
				readyqueue.sort(scheduler); //Do the sort for the SJF schedulers
				readyqueue.push(t, readyqueue.get_time_stack(time), quantum, scheduler);
			}
		}

	return 0;
}