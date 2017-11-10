#include <iostream> 
#include <string> 
#include <fstream> 
#include <iomanip> 
#include <stdlib.h> 
#include <bitset> 
#include <vector> 
#include <math.h> 
#include <algorithm> 
#include <unordered_map>
//#include "stdafx.h" //for Visual Studios 

using namespace std; 

struct node {

	int count;
	//Note that the coordinates below are the avg of this voxel space
	float x;
	float y;
	float z;
	int color;

	node(float xx, float yy, float zz, int RGB) : x(xx), y(yy), z(zz), color(RGB) { count = 1; }

	void addCoord(float new_x, float new_y, float new_z, int new_color) {
		count++;
		avgCoord(new_x, new_y, new_z);
		avgColor(new_color);
	}

	void avgCoord(float new_x, float new_y, float new_z) {

		//series of average
		//we are taking the avg of all coordinates in this voxel space to plot the new single coordinate in this space
		x = x + (new_x - x) / count;
		y = y + (new_y - y) / count;
		z = z + (new_z - z) / count;

	}

	void avgColor(int new_color) {
		int a = ((color >> 16) + ((new_color >> 16) - (color >> 16)) / count) << 16; // no need to shift left because first 8 int digit are 0s. 
		a += ((color << 16 >> 24) + ((new_color << 16 >> 24) - (color << 16 >> 24)) / count) << 8;
		a += ((color << 24 >> 24) + ((new_color << 24 >> 24) - (color << 24 >> 24)) / count);
		color = a;
	}



};


int main()
{
	//tuple<short x, short y, short z> represents bucket that the coordinates will be in.
	//for example if leaf is <.3,.3,.3> then the range for tuple <0,0,0> is all coord where < 0<=x<.3, 0<=y<.3, 0<=z<.3 >
	unordered_map<tuple<short, short, short>, node>;




	

	return 0;
}