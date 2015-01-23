#include <iostream>
#include <vector>
#include <fstream>
#include <thread>
#include <algorithm>
#include "timer.h"
#include "complex.h"
#include "ppm.h"

#define MAXITERS 512
#define ITERATIONS 5
#define ROWS 256
#define COLS 256

std::vector<int> mandelbrotData;
std::vector<int> threadingData[ITERATIONS];
std::vector<std::thread> threadpool(ITERATIONS);
std::ofstream data;


struct pixel{

};

int mbrotIters(Complex c){
	int i=0;
	Complex z;
	z.real=0;
	z.imag=0;
	
	while(i<MAXITERS){
		z=z*z+c;	
		if((z.real*z.real+z.imag*z.imag)>4)break;
		i++;
	}
	return i;
}

void mbrot(Complex c1, Complex c2, int startRows, int endRows, int cols, std::vector<int> mData){
	int i,j;
	Complex cx;
	int iters;

	for(i=startRows;i<endRows;++i){
		for(j=0;j<cols;++j){
		    cx.real = c1.real+(j*(c2.real-c1.real)/COLS);
		    cx.imag = c1.imag+(i*(c2.imag-c1.imag)/ROWS);
		    iters = mbrotIters(cx);
		    if(iters==MAXITERS)mData.push_back(0);
		    else mData.push_back(1);
		}
		mData.push_back(2);
	}
}

void threadStuff(Complex c1, Complex c2, int threads){
	int cols = COLS;
	int chunk = ROWS/threads;
	int excess = ROWS - chunk*threads;
	int location = 0, endLocation = chunk;
	//std::vector<int> data = std::vector<int>();
	
	std::generate(threadpool.begin(),threadpool.end(),[c1,c2,location,endLocation,cols,data]
		(){return std::thread(mbrot,c1,c2,location,endLocation,cols,data);});
	
	std::cout << "done" << std::endl;	
	for(int i = 1; i <= threads; i++){
		std::vector<int> data = std::vector<int>();
		threadpool[i] = std::thread(mbrot,c1,c2,location,endLocation,cols,data);
			location = endLocation;
			endLocation += chunk;
			if(i == threads)
				endLocation += excess;
			threadingData[i] = data;
	}
	for(auto&& t:threadpool){t.join();}
}

void generate(int threads){
	mandelbrotData = std::vector<int>();
	Complex c1;
	Complex c2;
	c1.real=-1.5;
	c1.imag=1.0;
	c2.real=0.5;
	c2.imag=-1.0;

	auto duration = timeFunc([c1,c2,threads](){threadStuff(c1,c2,threads);});
	data << duration.count() << "," << threads << std::endl;
	for(int i = 0; i < threads; i++)
		mandelbrotData.insert( mandelbrotData.end(), 
			threadingData[i].begin(), threadingData[i].end() );
}

void nums(int nums){std::cout << "One" << nums << std::endl;}

int main(){
	data.open("Values.csv");
	data << "Time Taken:, Threads:\n";
	for(int i = 1; i < ITERATIONS; i++){
		generate(i);
		mandelbrotData.clear();
	}
	generate(ITERATIONS);
	data.close();
	
	auto writer = new ppm("Mandelbrot.ppm");
	writer->sizeInit(ROWS,COLS);
	for(int i = 0; i < mandelbrotData.size(); i++){
		if(mandelbrotData[i] == 0)
			writer->writeBlack();
		else if(mandelbrotData[i] == 1)
			writer->writeWhite();
		else if(mandelbrotData[i] == 2)
			writer->writeNewline();

	}
	return 0;

}