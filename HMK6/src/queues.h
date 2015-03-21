#ifndef QUEUES_H
#define QUEUES_H

#include <vector>
#include "event.h"
#include "task.h"

class event_queue{
private:
	std::vector<event> queue;
public:
	event pop(){
		event e = queue.back();
		queue.pop_back();
		return e;
	}
	event peek(){
		return queue.back();
	}
	void push(task t, float time){
		t.set_start_time(time);
		float task_time = time;
		for(int i = 0; i < t.get_instructions().size()-1; i++){
			float burst_time = t.get_instructions().operator[](i).get_duration();
			task_time += burst_time;
			if(t.get_instructions().operator[](i).get_device() == 0){
					event e = event(task_time,&t);
					push(e);
				}
			else if(t.get_instructions().operator[](i).get_device() > 0){
					event e = event(task_time,
						t.get_instructions().operator[](i).get_device(),
						&t);
					push(e);
				}
			}
			event_type type = task_done;
			push(event(type,task_time+1,&t));
	}
	float get_time_stack(float time){
		if(queue.size()<1)
			return 0;
		return queue[queue.size()-1].get_time()+time;
	}
	void push(event e){queue.insert(queue.begin(), e);}
	int size(){return queue.size();}
	std::vector<event>::iterator begin(){return queue.begin();};
	std::vector<event>::iterator end(){return queue.end();};

};

#endif