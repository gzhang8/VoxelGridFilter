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
	int RGB;
	float x_normal;
	float y_normal;
	float z_normal;
	

	node() {}

	/********************/

	/*XYZ*/

	node(float xx, float yy, float zz) : x(xx), y(yy), z(zz) { count = 1; }

	void addCoord(float new_x, float new_y, float new_z) {
		count++;
		avgCoord(new_x, new_y, new_z);
	}

	/*XYZ RGB*/

	node(float xx, float yy, float zz, int rgb) : x(xx), y(yy), z(zz), RGB(rgb) { count = 1; }

	void addCoord(float new_x, float new_y, float new_z, int new_color) {
		count++;
		avgCoord(new_x, new_y, new_z);
		avgColor(new_color);
	}


	/*XYZ RGB XYZ_Normal*/

	node(float xx, float yy, float zz, int rgb, float xn, float yn, float zn) : x(xx), y(yy), z(zz), RGB(rgb), x_normal(xn), y_normal(yn), z_normal(zn) { count = 1; }

	void addCoord(float new_x, float new_y, float new_z, int new_color, float new_xn, float new_yn, float new_zn) {
		count++;
		avgCoord(new_x, new_y, new_z);
		avgColor(new_color);
		avgNormal(new_xn, new_yn, new_zn);
	}


	/********************/

	void avgCoord(float new_x, float new_y, float new_z) {

		//series of average
		//we are taking the avg of all coordinates in this voxel space to plot the new single coordinate in this space
		x = x + (new_x - x) / count;
		y = y + (new_y - y) / count;
		z = z + (new_z - z) / count;
	}

	void avgColor(int new_color) {

		int a = ((RGB >> 16) + ((new_color >> 16) - (RGB >> 16)) / count) << 16; // no need to shift left because first 8 int digit are 0s. 
		a += ((RGB << 16 >> 24) + ((new_color << 16 >> 24) - (RGB << 16 >> 24)) / count) << 8;
		a += ((RGB << 24 >> 24) + ((new_color << 24 >> 24) - (RGB << 24 >> 24)) / count);
		RGB = a;

	}

	void avgNormal(float new_xn, float new_yn, float new_zn) {

                x_normal = x_normal + (new_xn - x_normal) / count;
                y_normal = y_normal + (new_yn - y_normal) / count;
                z_normal = z_normal + (new_zn - z_normal) / count;
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

	vector<string> format;

	bool isPCD = false;

	voxelHash(float lx, float ly, float lz) : leaf_x(lx), leaf_y(ly), leaf_z(lz) {}


	long long int hash(float x, float y, float z) {
		//This will show which bucket each coordinate should be in 
		//Ex leaf is <.3,.3,.3>, coord is <.1,.2,.2>, it should go to hash equivalent of <0,0,0>
		int xHash = floor(x / leaf_x);
		int yHash = floor(y / leaf_y);
		int zHash = floor(z / leaf_z);

		//long long int hash = (((((31 + xHash) * yHash) + 37) * 83) + zHash) * 131;
		long long int hash = ((xHash * 472882049) ^ (yHash * 19349669) ^ (zHash * 83492791)); //prime number hashing but without specifying size of hash

		return hash;

	}

	/*no color*/

	void addCoord(vector<float> val) {

		if(val.size() == 3) addCoord(val[0], val[1], val[2]);						//x y z
		else if (val.size() == 4) addCoord(val[0], val[1], val[2], (int)val[3]);	//x y z rgb
		else addCoord(val[0], val[1], val[2], (int)val[3], val[4], val[5], val[6]);

	}

	/*XYZ*/

	void addCoord(float x, float y, float z) {
		long long int key = hash(x, y, z);
		if (vHash.count(key)) {
			vHash[key].addCoord(x, y, z);

		}
		else {
			node temp(x, y, z);
			vHash.insert(pair<long long, node>(key, temp));
		}
	}

	/*XYZ RGB*/

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

	/*XYZ RGB XYZ_Normal*/

	void addCoord(float x, float y, float z, int rgb, float xn, float yn, float zn) {
		long long int key = hash(x, y, z);
		if (vHash.count(key)) {
			vHash[key].addCoord(x, y, z, rgb, xn, yn, zn);

		}
		else {
			node temp(x, y, z, rgb,xn, yn, zn);
			vHash.insert(pair<long long, node>(key, temp));
		}
	}

	void exportText() {

		ofstream outFile("xor_hash_output.pcd");
		if (outFile.is_open()) {

			//output file format
			if (isPCD) {
				int size = vHash.size();

				for (int i = 0; i < 11; i++) {
					if (i == 6) {
						outFile << "WIDTH " + to_string(size) + '\n';
					}
					else if (i == 9) {
						outFile << "POINTS " + to_string(size) + '\n';

					}
					else {
						outFile << format[i];
					}
				}
			}


			for (auto i : vHash) {
				//outFile << to_string(i.second.x) + " " + to_string(i.second.y) + " " + to_string(i.second.z) + "\n";
				outFile << to_string(i.second.x) + " " + to_string(i.second.y) + " " + to_string(i.second.z) + " " + to_string(i.second.RGB)+  " " + to_string(i.second.x_normal)+  " " + to_string(i.second.y_normal)+  " " + to_string(i.second.z_normal)+ " 0" + "\n";
			}
			
			outFile.close();

		}
	}
};


int init()
{
	voxelHash temp(.01, .01, .01);
	ifstream file("test.pcd");
	string inLine;
	string element;


	if (file.is_open())
	{
		//copy the pcd file format
		getline(file, inLine);
		if (inLine.substr(0, 6) == "# .PCD") {
			temp.isPCD = true;
			temp.format.push_back(inLine + '\n');
			for (int i = 0; i < 10; i++) {
				getline(file, inLine);
				temp.format.push_back(inLine + '\n');
			}
		}
		else { goto skip; }

		while (getline(file, inLine)) { // gets current line
		skip:
			vector<float> val;
			istringstream iss(inLine);
			while (getline(iss, element, ' ')) //gets space delimited value from line
			{
				val.push_back(stof(element));
			}
			val.pop_back();//the file has curvature which I don't need
			temp.addCoord(val);

		}

	}

	temp.exportText();

	return 0;
}


int main()
{
        float avgTime = 0;
        int j = 5;
        for (int i = 0; i < j; i++) {
                float startTime = (float)clock() / CLOCKS_PER_SEC;

                init();

                float endTime = (float)clock() / CLOCKS_PER_SEC;

                cout << endTime - startTime << endl;

                avgTime += endTime - startTime;
        }
        avgTime /= j;

        cout << "Avg Time = " << avgTime;

        return 0;

}
