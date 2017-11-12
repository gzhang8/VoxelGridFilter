# VoxelGridFilter

Standalone voxelgrid from the very beginning

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

x y z normal_x normal_y normal_z rgb


Below will be a basic implementation that I will try in order just get the avg of x, y, z and rgb in order to understand the process.

<Approach 1>

1. Spatial Hash 

The purpose of having a hash is to completely avoid actual plotting of the points. We have a list of voxel points and their properties. We can hash all the points that belong to a voxel space and do the computation directly as it enters the hash.  

Say that the leaf is <1,1,1>, that means for the first block we need to find all points x < 1 , y < 1, z < 1, condense it into 1 point located at the avg and finding average color. We can plot it all into a map and have the finding avg operation done when the points are inserted. Now in order for this to work we would need a perfect hashing function that won't have any unwanted collision with 96 bits (xyz are three floats). Mathematically speaking, given that our hash function can only have operations done to 64 bits this is not possible. However, we can test different hashing operations and catch all unwanted collision and test success rate of each

Name: test01.pcd
Method: Prime Number combination

	long long int hash = (((((31 + xHash) * yHash) + 37) * 83) + zHash) * 131;

Expected Count of Points: 1817
Actual Count of Points: 1615
Duration: 12 Seconds
Analysis: Points in the middle cross section are missing and somehow ending up at the center of the image.

Name: test02.pcd
Method: Prime Number xor

	long long int hash = ((xHash * 73856093) ^ (yHash * 19349669) ^ (zHash * 83492791)); //prime number hashing but without specifying size of hash

Description:Hashing based on http://graphics.stanford.edu/~niessner/papers/2013/4hashing/niessner2013hashing.pdf where Niesser hashed exploiting the fact that many space on the graph is empty. Unlike his method, I never specified the size of Hash because I need perfect hashing but I can change this later
Expected Count of Points: 1817
Actual Count of Points: 1629
Duration: 
Analysis: Points appear in the center but no missing cross section like the method above 

Name: N/A
Method: Golden Ratio 

		long long xy = (xHash << 32) | yHash;
		xy ^= zHash + 0x9e3779b97f4a7c15 + (xy << 6) + (xy >> 2);

Description:Hashing based on the boost library. Doesn't work sing original equation accounts for combining already hashed values. 
Expected Count of Points: 1817
Actual Count of Points: 367
Ananlysis: N/A
