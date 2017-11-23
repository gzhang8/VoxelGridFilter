Standalone voxelgrid 
---------------------------------------------------------------

tl;dr: How to use

	pcl_convert_pcd_ascii_binary [input_file.pcd] test.pcd 0
	g++ -std=c++11 doubleHash.cpp -o doubleHash
	./a doubleHash

or for xorHash

	pcl_convert_pcd_ascii_binary [input_file.pcd] test.pcd 0
	g++ -std=c++11 xorHash.cpp -o xorHash
	./a xorHash


---------------------------------------------------------------
From the very beginning

Our goal is to create a voxelgrid function just like the one in the PCL library. Since I'm completely new to linux and pcl every step I do to get to the result will be documented. 

1. Installing PCL version 18 for Ubuntu 16.04

So first before we start on trying to have our own standalone voxel filter we need to use it in order to understand its capabilities. The instructions for downloading it is outdated. I'm currently using Ubuntu 16.04 trying to download the latest pcl library, version 18. Instead of using the instructions from the pcl website, use the instruction from

https://larrylisky.com/2016/11/03/point-cloud-library-on-ubuntu-16-04-lts/ 

to run the voxel_grid create the cpp and cmake file.

http://pointclouds.org/documentation/tutorials/voxel_grid.php

As a reference you should have 

-voxel grid folder
	-voxel_grid.cpp
	-CMakefiles.txt


create a make folder and run 
	cmake .. 
	make

-voxel grid folder
	-voxel_grid.cpp
	-CMakefiles.txt
	-make
		-(make file stuff)

NOTE: just as a reference, if you download all the files but did not install it unto your computer, there is a roundabout way which is on the cmake file, make sure to add (below) somewhere. Otherwise your computer won't be able to locate pcl's cmake file if you haven't downloaded pcl using sudo get install update.

	set(PCL_DIR "/home/[[user]/pcl/release/")



2. Converting .pcd files to something readable

A .pcd file is the standard file format for pcl point clouds. If we open a pcd file as it is or as a txt file, a bunch of unreadable ascii value will show up. In order to convert the file into something that makes sense there is a function built into the pcd library called pcl_convert_pcd_ascii_binary. The output .pcd file will have human readble data that we can use.

	pcl_convert_pcd_ascii_binary <file_in.pcd> <file_out.pcd> 0/1/2 (ascii/binary/binary_compressed) (we're using ascii)



3. Data Representation 

A PCD file can have all kinds of data format. For all simplicity sake, I'm going to only store my data like the format below. This will account for all information needed.

x y z rgb normal_x normal_y normal_z

4. Core Implementation

Below will be a basic implementation that I will try in order just get the avg of x, y, z and rgb in order to understand the process.

<Approach 1: Spatial Hash>

The purpose of having a hash is to completely avoid actual plotting of the points. We have a list of voxel points and their properties. We can hash all the points that belong to a voxel space and do the computation directly as it enters the hash.  

Say that the leaf is <1,1,1>, that means for the first block we need to find all points x < 1 , y < 1, z < 1, condense it into 1 point located at the avg and finding average color. We can plot it all into a map and have the finding avg operation done when the points are inserted. Now in order for this to work we would need a perfect hashing function that won't have any unwanted collision with 96 bits (xyz are three floats). Mathematically speaking, given that our hash function can only have operations done to 64 bits this is not possible. However, we can test different hashing operations and catch all unwanted collision and test success rate of each.

STAGE 1: avg of xyz

For average of xyz, if a voxel falls into a bucket we can find the new avg by using the formula

	coord = coord + (new_coord - coord) / count;



--------------------------------------------------------------------------------------------------------
Method: Prime Number combination

input file: xyz_sample.pcd

output file: xyz_method01.pcd

	long long int hash = (((((31 + xHash) * yHash) + 37) * 83) + zHash) * 131;

Expected Count of Points: 1817

Actual Count of Points: 1615

Current Iteration Flaws: 

1. Points in the middle cross section are missing and somehow ending up at the center of the image.

--------------------------------------------------------------------------------------------------------

Method: Prime Number xor

input file: all_sample.pcd

output file: all_method02.pcd

	long long int hash = ((xHash * 73856093) ^ (yHash * 19349669) ^ (zHash * 83492791)); //prime number hashing but without specifying size of hash

Description:Hashing based on http://graphics.stanford.edu/~niessner/papers/2013/4hashing/niessner2013hashing.pdf (turns out Niessner did something similar to what I'm trying to achieve and he's a lot smarter so I'm probably pull a lot from his paper) where Niesser hashed exploiting the fact that many space on the graph is empty. Unlike his method, I never specified the size of Hash because I need perfect hashing (which I can account for later). However, I used a 64 bit hash rather than Niessner's 32 bit hash to avoid collision better.

Expected Count of Points: 94678

Actual Count of Points: 94676

Current Iteration Flaws:

1. Points appear in the center when not suppose to be but no missing cross section like the method above 
2. Hashed increased to 64 bits so points no longer appear in center

--------------------------------------------------------------------------------------------------------

Method: Double Layered Perfect Hash

output file: all_method03.pcd

input file: all_sample.pcd

Description:Because we have 96 bits to represent the three coordinates of XYZ we can have a Hash object like the following where cooridinates are converted into buckets and hashed by said buckets

					combine xy                  z
	unordered_map<long long int, unordered_map<int,node>> vHash;

Innately this is slower than any of the hashing done eariler but it guarantees a perfect hash because it avoids collisions all together. 

Expected Count of Points: 94678

Actual Count of Points: 94678

Current Iteration Flaws:

1. Slower than conventional hash but really not by much

--------------------------------------------------------------------------------------------------------


Method: Golden Ratio 

output file: N/A

		long long xy = (xHash << 32) | yHash;
		xy ^= zHash + 0x9e3779b97f4a7c15 + (xy << 6) + (xy >> 2);

Description:Hashing based on the boost library. Doesn't work sing original equation accounts for combining already hashed values. 
Expected Count of Points: 1817

Actual Count of Points: 367

Analysis: N/A


STAGE 2: avg of color

NOTE: Ignore below for now, I implemented the squared average and everything came out prominently red and "bloodstained". For now it's just conventional average like coordinates

The problem with taking an average of color is that its not simply just (red + green)/2. We see color in a logarithmicly meaning as intensity increase, brightness plateaus. The correct way to find the avg of 2 color is as follows

	sqrt((red^2+green^2)/2)

The equation for finding the avg of the coordinates won't work in this case, instead we can use 

	new_avg = sqrt(old_avg^2 + (new_val^2+old_avg^2)/count)

Now I'll be completely honest, I got this equation with trial and error so I can't really explain how it works in a series. 