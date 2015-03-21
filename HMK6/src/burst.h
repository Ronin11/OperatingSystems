#ifndef BURST_H
#define BURST_H

#include <random>

float generate_time(float mix){
	std::random_device rd;
	std::mt19937 mt(rd());
	//This will change depending on the mix variable in main
	std::normal_distribution<> norm(10*mix, 1);
	return norm(mt);
}
int generate_int(int max){
   	std::random_device rd;
   	std::mt19937 gen(rd());
   	std::uniform_int_distribution<> dis(1, max);
	int val = dis(gen);
	return val;
}

class burst{
protected:
	int device;
	float time_left;
	float duration;
public:
	float get_duration(){return duration;}
	float get_time_left(){return time_left;}
	int get_device(){return device;}
};

class cpuburst : public burst{
public:
	cpuburst(float mix){
		device = 0;
		duration = generate_time(mix);
		time_left = duration;
	}
};

class ioburst : public burst{

public:
	ioburst(float mix, int max_devices){
		duration = generate_time(mix);
		time_left = duration;
		device = generate_int(max_devices);
	}
};

#endif