#ifndef COMPLEX_H
#define COMPLEX_H

struct Complex {
	double real;
	double imag;
};

Complex mult(Complex a, Complex b){
	Complex c;
	c.real = a.real*b.real - a.imag*b.imag;
	c.imag = b.real*a.imag + a.real*b.imag;
	return c;
}

Complex operator*(Complex a, Complex b){
	return mult(a,b);
}

Complex add(Complex a, Complex b){
	Complex c;
	c.real = a.real+b.real;
	c.imag = a.imag+b.imag;
	return c;
}

Complex operator+(Complex a, Complex b){
	return add(a,b);
}

#endif