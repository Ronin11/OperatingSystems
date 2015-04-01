#ifndef QUEUES_H
#define QUEUES_H

#include <vector>
#include "event.h"
#include "task.h"

class event_queue{
private:
	std::vector<event> queue;
	struct queuedata{
		int begin;
		int end;
		float tasktime;
	};
public:
	event pop(){
		event e = queue.back();
		queue.pop_back();
		return e;
	}
	event peek(){
		return queue.back();
	}
	void push(task t, float time, float quantum, int scheduler){
		t.set_start_time(time);
		float task_time = time;
		for(int i = 0; i < t.get_instructions().size()-1; i++){
			float burst_time = t.get_instructions().operator[](i).get_duration();
			if(scheduler == 1){ //Break apart the bursts so that they are smaller than the quantum
				if(burst_time > quantum){
					if(t.get_instructions().operator[](i).get_device() == 0){
						while(quantum < burst_time){
							task_time += quantum;
							burst_time -= quantum;
							event e = event(task_time, t.get_instructions().operator[](i).get_memory(), &t);
							push(e);
						}
							task_time += burst_time;
							event e = event(task_time, t.get_instructions().operator[](i).get_memory(), &t);
							push(e);
						}
					else if(t.get_instructions().operator[](i).get_device() > 0){
						while(quantum < burst_time){
							task_time += quantum;
							burst_time -= quantum;
							event e = event(task_time,
								t.get_instructions().operator[](i).get_device(),
								t.get_instructions().operator[](i).get_memory(),
								&t);
							push(e);
						}
							event e = event(task_time,
								t.get_instructions().operator[](i).get_device(),
								t.get_instructions().operator[](i).get_memory(),
								&t);
							push(e);
						}
				}
				else{	
				task_time += burst_time;
					if(t.get_instructions().operator[](i).get_device() == 0){
							event e = event(task_time, t.get_instructions().operator[](i).get_memory(), &t);
							push(e);
						}
					else if(t.get_instructions().operator[](i).get_device() > 0){
							event e = event(task_time,
								t.get_instructions().operator[](i).get_device(),
								t.get_instructions().operator[](i).get_memory(),
								&t);
							push(e);
						}
				}
			}
			else{
			task_time += burst_time;
			t.update_average_burst(burst_time);
				if(t.get_instructions().operator[](i).get_device() == 0){
						event e = event(task_time, t.get_instructions().operator[](i).get_memory(), &t);
						push(e);
					}
				else if(t.get_instructions().operator[](i).get_device() > 0){
						event e = event(task_time,
							t.get_instructions().operator[](i).get_device(),
							t.get_instructions().operator[](i).get_memory(),
							&t);
						push(e);
					}
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

	void sort(int scheduler){ //Ugliest code possible.
		//Shortest job first by simply sorting the ready queue,
		if(queue.size() == 0 || queue.size() == 1 || scheduler == 0 || scheduler == 1) //Dont sort for fifo or round robin
			return;
		std::vector<queuedata> buffer;
		buffer.push_back({0,0,0.0});
		int j = 0;
		for(int i = 0; i < queue.size(); i++)
			if(queue[i].get_type() == task_done){
				buffer[j].end = i;
				if(scheduler == 2)
					for(int k = buffer[j].begin; k < buffer[j].end; k++)
						buffer[j].tasktime += queue[k].get_time(); //Sort by actual time for SJF
				else if(scheduler == 3)
						buffer[j].tasktime = queue[j].get_task()->get_average_burst(); //Sort by average time for Approx SJF
				if(i < queue.size())
					buffer.push_back({i+1,0,0.0});
			}
		std::vector<event> temp;
		float lowest;
		int index;
		while(buffer.size() > 0){
			index = 0;
			lowest = buffer[0].tasktime;
			for(int i = 0; i < buffer.size(); i++){
				if(buffer[i].tasktime < lowest){
					lowest = buffer[i].tasktime;
					index = i;
				}
			}
			for(int i = buffer[index].begin; i <= buffer[index].end; i++){
				if(temp.size() == 0)
					temp.push_back(queue[i]);
				else
					temp.insert(queue.begin(), queue[i]);
			}
			buffer.erase(buffer.begin()+index);
		}
		queue = temp;
	}

};

#endif