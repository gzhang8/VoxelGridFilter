//#include <iostream> 
//#include <string> 
//#include <sstream>
//#include <fstream>
//#include <fstream> 
//#include <iomanip> 
//#include <stdlib.h> 
//#include <bitset> 
//#include <vector> 
//#include <math.h> 
//#include <algorithm> 
//#include <unordered_map>
////#include "stdafx.h" //for Visual Studios 
//
//using namespace std;
//
//int HashError = 0;
//
//struct node {
//
//	int count;
//	//Note that the coordinates below are the avg of this voxel space
//	float x;
//	float y;
//	float z;
//	int RGB;
//	float x_normal;
//	float y_normal;
//	float z_normal;
//	
//
//	node() {}
//
//	/********************/
//
//	/*XYZ*/
//
//	node(float xx, float yy, float zz) : x(xx), y(yy), z(zz) { count = 1; }
//
//	void addCoord(float new_x, float new_y, float new_z) {
//		count++;
//		avgCoord(new_x, new_y, new_z);
//	}
//
//	/*XYZ RGB*/
//
//	node(float xx, float yy, float zz, int rgb) : x(xx), y(yy), z(zz), RGB(rgb) { count = 1; }
//
//	void addCoord(float new_x, float new_y, float new_z, int new_color) {
//		count++;
//		avgCoord(new_x, new_y, new_z);
//		avgColor(new_color);
//	}
//
//
//	/*XYZ RGB XYZ_Normal*/
//
//	node(float xx, float yy, float zz, int rgb, float xn, float yn, float zn) : x(xx), y(yy), z(zz), RGB(rgb), x_normal(xn), y_normal(yn), z_normal(zn) { count = 1; }
//
//	void addCoord(float new_x, float new_y, float new_z, int new_color, float new_xn, float new_yn, float new_zn) {
//		count++;
//		avgCoord(new_x, new_y, new_z);
//		avgColor(new_color);
//		avgNormal(new_xn, new_yn, new_zn);
//	}
//
//
//	/********************/
//
//	void avgCoord(float new_x, float new_y, float new_z) {
//
//		//series of average
//		//we are taking the avg of all coordinates in this voxel space to plot the new single coordinate in this space
//		x = x + (new_x - x) / count;
//		y = y + (new_y - y) / count;
//		z = z + (new_z - z) / count;
//	}
//
//	void avgColor(int new_color) {
//
//
//		//int red = sqrt(pow((RGB >> 16), 2) + (pow((new_color >> 16), 2) - pow((RGB >> 16), 2)) / count); // no need to shift left because first 8 int digit are 0s. 
//		//int blue = sqrt(pow((RGB << 16 >> 24), 2) + (pow((new_color << 16 >> 24), 2) - pow((RGB << 16 >> 24), 2)) / count);
//		//int green = sqrt(pow((RGB << 24 >> 24), 2) + (pow((new_color << 24 >> 24), 2) - pow((RGB << 24 >> 24), 2)) / count);
//
//		//RGB = (red<<16) | (blue<<8) | green ;
//
//		int a = ((RGB >> 16) + ((new_color >> 16) - (RGB >> 16)) / count) << 16; // no need to shift left because first 8 int digit are 0s. 
//		a += ((RGB << 16 >> 24) + ((new_color << 16 >> 24) - (RGB << 16 >> 24)) / count) << 8;
//		a += ((RGB << 24 >> 24) + ((new_color << 24 >> 24) - (RGB << 24 >> 24)) / count);
//		RGB = a;
//		
//	}
//
//	void avgNormal(float new_xn, float new_yn, float new_zn) {
//
//		x_normal = x + (new_xn - x_normal) / count;
//		y_normal = y + (new_yn - y_normal) / count;
//		z_normal = z + (new_zn - z_normal) / count;
//	}
//
//
//};
//
//struct voxelHash {
//
//	//<x, y, z> represents bucket that the coordinates will be in.
//	//for example if leaf is <.3,.3,.3> then the range for <0,0,0> is all coord where < 0<=x<.3, 0<=y<.3, 0<=z<.3 >
//	unordered_map<long long int, unordered_map<int,node>> vHash;
//	float leaf_x;
//	float leaf_y;
//	float leaf_z;
//
//	vector<string> format;
//
//	bool isPCD = false;
//
//	voxelHash() {}
//
//	voxelHash(float lx, float ly, float lz) : leaf_x(lx), leaf_y(ly), leaf_z(lz) {}
//
//
//	void addCoord(vector<float> val) {
//
//		if(val.size() == 3) addCoord(val[0], val[1], val[2]);						//x y z
//		else if (val.size() == 4) addCoord(val[0], val[1], val[2], (int)val[3]);	//x y z rgb
//		else addCoord(val[0], val[1], val[2], (int)val[3], val[4], val[5], val[6]);	//x y z rgb x_normal, y_normal, z_normal
//
//	}
//
//	/*XYZ*/
//
//	void addCoord(float x, float y, float z) {
//		int xHash = floor(x / leaf_x);
//		int yHash = floor(y / leaf_y);
//		int zHash = floor(z / leaf_z);
//
//		long long int xyHash = xHash << 16 | yHash;
//
//		if (vHash[xyHash].count(zHash)) {
//			vHash[xyHash][zHash].addCoord(x, y, z);
//		}
//
//		else {
//			node temp(x, y, z);
//			vHash[xyHash].insert(pair<int, node>(zHash, temp));
//		}
//	}
//
//	/*XYZ RGB*/
//
//	void addCoord(float x, float y, float z, int rgb) {
//		int xHash = floor(x / leaf_x);
//		int yHash = floor(y / leaf_y);
//		int zHash = floor(z / leaf_z);
//
//		long long int xyHash = xHash << 16 | yHash;
//
//		if (vHash[xyHash].count(zHash)) {
//			vHash[xyHash][zHash].addCoord(x, y, z, rgb);
//		}
//
//		else {
//			node temp(x, y, z,rgb);
//			vHash[xyHash].insert(pair<int, node>(zHash, temp));
//		}
//	}
//
//	/*XYZ RGB XYZ_Normal*/
//
//	void addCoord(float x, float y, float z, int rgb, float xn, float yn, float zn) {
//		int xHash = floor(x / leaf_x);
//		int yHash = floor(y / leaf_y);
//		int zHash = floor(z / leaf_z);
//
//		long long int xyHash = xHash << 16 | yHash;
//
//		if (vHash[xyHash].count(zHash)) {
//			vHash[xyHash][zHash].addCoord(x, y, z, rgb, xn, yn, zn);
//		}
//
//		else {
//			node temp(x, y, z, rgb, xn, yn, zn);
//			vHash[xyHash].insert(pair<int, node>(zHash, temp));
//		}
//	}
//	void exportText() {
//
//
//
//		ofstream outFile("double_hash_output.pcd");
//		if (outFile.is_open()) {
//
//			//output file format
//			if (isPCD) {
//				int size = 0;
//				for (auto j : vHash) {size += j.second.size();}
//
//				for (int i = 0; i < 11; i++) {
//					if (i == 6) {
//						outFile << "WIDTH " + to_string(size) + '\n';
//					}
//					else if (i == 9) {
//						outFile << "POINTS " + to_string(size) + '\n';
//					
//					}
//					else {
//						outFile << format[i];
//					}
//
//				}
//			}
//			
//
//			for (auto j : vHash) {
//				for (auto i : j.second) {
//					//outFile << to_string(i.second.x) + " " + to_string(i.second.y) + " " + to_string(i.second.z) + "\n";
//					outFile << to_string(i.second.x) + " " + to_string(i.second.y) + " " + to_string(i.second.z) + " " + to_string(i.second.RGB) + " " + to_string(i.second.x_normal) + " " + to_string(i.second.y_normal) + " " + to_string(i.second.z_normal) + " "+ "0" + "\n"; //the 0 is for curvature for right now
//				}
//			}
//			
//			outFile.close();
//		}
//	}
//};
//
//
//int main()
//{
//	voxelHash temp(.01, .01, .01);
//	ifstream file("test.pcd");
//	string inLine;
//	string element;
//
//
//	if (file.is_open())
//	{
//		//copy the pcd file format
//		getline(file, inLine);
//		if (inLine.substr(0, 6) == "# .PCD") {
//			temp.isPCD = true;
//			temp.format.push_back(inLine + '\n');
//			for (int i = 0; i < 10; i++) {
//				getline(file, inLine);
//				temp.format.push_back(inLine + '\n');
//			}
//		}
//		else { goto skip; }
//
//		while (getline(file, inLine)) { // gets current line
//skip:
//			vector<float> val;
//			istringstream iss(inLine);
//			while (getline(iss, element, ' ')) //gets space delimited value from line
//			{
//				val.push_back(stof(element));
//			}
//			val.pop_back();//the file has curvature which I don't need
//			temp.addCoord(val);
//
//		}
//
//	}
//
//	temp.exportText();
//
//	return 0;
//}
