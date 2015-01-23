#include <iostream>
#include <vector>
#include <fstream>
#include <thread>
#include <algorithm>
#include "timer.h"
#include "complex.h"
#include "ppm.h"

#define MAXITERS 512
#define ITERATIONS 4
#define ROWS 4096
#define COLS 4096


struct pixel{
	bool solved = false;
	int value;
};

std::vector<std::vector<pixel>> mandelbrotData;
std::vector<std::thread> threadpool(ITERATIONS);
std::ofstream data;

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

void mbrot(Complex c1, Complex c2){
	int i,j;
	Complex cx;
	int iters;

	for(i=0;i<ROWS;++i){
		for(j=0;j<COLS;++j){
			if(!mandelbrotData[i][j].solved){
			    mandelbrotData[i][j].solved = true;
			    cx.real = c1.real+(j*(c2.real-c1.real)/COLS);
			    cx.imag = c1.imag+(i*(c2.imag-c1.imag)/ROWS);
			    iters = mbrotIters(cx);
			    if(iters==MAXITERS)
			    	mandelbrotData[i][j].value = 0;
			    else
					mandelbrotData[i][j].value = 1;
			}
		}
	}
}

void threadStuff(Complex c1, Complex c2, int threads){
	
	std::generate(threadpool.begin(),threadpool.end(),[c1,c2]
		(){return std::thread(mbrot,c1,c2);});

	for(auto&& t:threadpool){t.join();}
}

void generate(int threads){
	mandelbrotData = std::vector<std::vector<pixel>>(ROWS, std::vector<pixel>(COLS));
	Complex c1;
	Complex c2;
	c1.real=-1.5;
	c1.imag=1.0;
	c2.real=0.5;
	c2.imag=-1.0;

	auto duration = timeFunc([c1,c2,threads](){threadStuff(c1,c2,threads);});
	data << duration.count() << "," << threads << std::endl;
	
}

int main(){
	data.open("Values.csv");
	data << "Time Taken:, Threads:\n";
	//for(int i = 1; i < ITERATIONS; i++){
	//	generate(i);
	//	mandelbrotData.clear();
	//}
	generate(ITERATIONS);
	data.close();
	
	auto writer = new ppm("Mandelbrot.ppm");
	writer->sizeInit(ROWS,COLS);
	for(int i = 0; i < ROWS; i++){
		for(int j = 0; j < COLS; j++){
			if(mandelbrotData[i][j].value == 0)
				writer->writeBlack();
			else if(mandelbrotData[i][j].value == 1)
				writer->writeWhite();
		}
			writer->writeNewline();
	}
	return 0;

}