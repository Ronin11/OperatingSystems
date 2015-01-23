#include <iostream>
#include <vector>
#include <fstream>
#include "timer.h"
#include "complex.h"
#include "ppm.h"

#define MAXITERS 512
#define ITERATIONS 5

std::vector<int> mandelbrotData;
std::ofstream data;
int ROWS = 256;
int COLS = 256;

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
		    cx.real = c1.real+(j*(c2.real-c1.real)/COLS);
		    cx.imag = c1.imag+(i*(c2.imag-c1.imag)/ROWS);
		    iters = mbrotIters(cx);
		    if(iters==MAXITERS)mandelbrotData.push_back(0);
		    else mandelbrotData.push_back(1);
		}
		mandelbrotData.push_back(2);
	}
}

void generate(){
	mandelbrotData = std::vector<int>();
	Complex c1;
	Complex c2;
	c1.real=-1.5;
	c1.imag=1.0;
	c2.real=0.5;
	c2.imag=-1.0;

	auto duration = timeFunc([c1,c2](){mbrot(c1,c2);});
	data << duration.count() << "," << ROWS << "," << COLS << std::endl;
}

int main(){

	data.open("Values.csv");
	data << "Time Taken:, ROWS, COLS\n";
	for(int i = 0; i < ITERATIONS; i++){
		generate();
		ROWS *= 2;
		COLS *= 2;
		mandelbrotData.clear();
	}
	generate();
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