#include <iostream> 
#include <string> 
#include <sstream>
#include <fstream>
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

int HashError = 0;

struct node {

	int count;
	//Note that the coordinates below are the avg of this voxel space
	float x;
	float y;
	float z;
	int color;

	node() {}

	/*Without color*/

	node(float xx, float yy, float zz) : x(xx), y(yy), z(zz) { count = 1; }

	void addCoord(float new_x, float new_y, float new_z) {
		count++;
		avgCoord(new_x, new_y, new_z);
	}

	/*With color*/

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

struct voxelHash {

	//<x, y, z> represents bucket that the coordinates will be in.
	//for example if leaf is <.3,.3,.3> then the range for <0,0,0> is all coord where < 0<=x<.3, 0<=y<.3, 0<=z<.3 >
	unordered_map<long long int, node> vHash;
	float leaf_x;
	float leaf_y;
	float leaf_z;

	voxelHash() {}

	voxelHash(float lx, float ly, float lz) : leaf_x(lx), leaf_y(ly), leaf_z(lz) {}

	//we want some sort of spatial hashing. Doesn't work right now
	long long int hash(float x, float y, float z) {
		//This will show which bucket each coordinate should be in 
		//Ex leaf is <.3,.3,.3>, coord is <.1,.2,.2>, it should go to hash equivalent of <0,0,0>
		int xHash = floor(x / leaf_x);
		int yHash = floor(y / leaf_y);
		int zHash = floor(z / leaf_z);

		long long int hash = (((((31 + xHash) * yHash) + 37) * 83) + zHash) * 131;


		return hash;

	}

	/*no color*/
	//will improve
	void addCoord(vector<float> val) {
		addCoord(val[0], val[1], val[2]);

	}

	void addCoord(float x, float y, float z) {
		long long key = hash(x, y, z);
		if (vHash.count(key)) {
			vHash[key].addCoord(x, y, z);

		}
		else {
			node temp(x, y, z);
			vHash.insert(pair<long long, node>(key, temp));
		}
	}

	/*with color*/

	void addCoord(float x, float y, float z, int rgb) {
		long long int key = hash(x, y, z);
		if (vHash.count(key)) {
			vHash[key].addCoord(x, y, z, rgb);

		}
		else {
			node temp(x, y, z, rgb);
			vHash.insert(pair<long long, node>(key, temp));
		}
	}

	void exportText() {

	}

};


int main()
{
	voxelHash temp(.1, .1, .1);
	ifstream file("test.pcd");
	string inLine;
	string element;


	if (file.is_open())
	{
		while (getline(file, inLine)) { // gets current line
			vector<float> val;
			istringstream iss(inLine);
			while (getline(iss, element, ' ')) //gets space delimited value from line
			{
				val.push_back(stof(element));
			}
			temp.addCoord(val);

		}



	}

	return 0;
}

