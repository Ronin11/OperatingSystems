#ifndef PPM_H
#define PPM_H

#include <fstream>
#include <string>

class ppm{
private:
	std::ofstream file;

public:
	ppm(std::string name){
		file.open(name);
		file << "P3" << std::endl;
	}
	void sizeInit(int rows, int cols){
		file << rows << " " << cols << " " << "255" << std::endl;
	}
	~ppm(){file.close();}
	void writeBlack(){file << "0 0 0\t";}
	void writeWhite(){file << "255 255 255\t";}
	void writeNewline(){file << std::endl;}

};


#endif