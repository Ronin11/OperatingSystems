#ifndef EVENT_H
#define EVENT_H

#include <vector>
#include "task.h"

enum event_type{
	sim_done,
	ioburst_type,
	cpuburst_type,
	task_done
};
class event{
private:
	task * t;
	float time;
	int device = 0;
	event_type type;
public:
	event(float time, task *t){
		type = cpuburst_type;
		this->time = time;
		this->device = 0;
		this->t = t;
	}
	event(float time, int device, task *t){
		type = ioburst_type;
		this->time = time;
		this->device = device;
		this->t = t;
	}
	//Simulation done event && task done event
	event(event_type e, float time, task *t){
		type = e;
		time = time;
		device = 0;
		this->t = t;
	}
	int get_device(){return device;}
	float get_time(){return time;}
	event_type get_type(){return type;}
	task * get_task(){return t;}
	bool operator < (const event& e) const{
        return (time < e.time);
	}
};
#endif