#ifndef TASK_H
#define TASK_H

#include <vector>
#include "burst.h"

class task{
private:
	float start_time = 0;	
	float response_time = 0;
	float end_time = 0;
	bool responded = false;
	std::vector<burst> instructions = std::vector<burst>(1);
public:
	task(int max_devices, float mix){
		for(int i = 0; i < generate_int(10); i++){
			instructions.push_back(cpuburst(mix));
			instructions.push_back(ioburst(1-mix, max_devices));
		}
		instructions.push_back(cpuburst(mix));

	}
	void set_start_time(float f){start_time = f;}
	float get_start_time(){return start_time;}
	void set_end_time(float f){end_time = f;}
	float get_end_time(){return end_time;}
	void set_response_time(float f){this->response_time = f;}
	float get_response_time(){return response_time;}
	std::vector<burst> get_instructions(){return instructions;}
};

#endif
