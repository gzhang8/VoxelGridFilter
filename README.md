# VoxelGridFilter

So first before we start on trying to have our own standalone voxel filter we need to use it in order to understand its capabilities. The instructions for downloading it is outdated. I'm currently using Ubuntu 16.04 trying to download the latest pcl library, version 18. Instead of using the instructions from the pcl website, use the instruction from

https://larrylisky.com/2016/11/03/point-cloud-library-on-ubuntu-16-04-lts/ 

to run the voxel_grid create the cpp and cmake file.

http://pointclouds.org/documentation/tutorials/voxel_grid.php

As a reference you should have 

-voxel grid folder
	-voxel_grid.cpp
	-CMakefiles.txt


create a make folder and run cmake then make in it.

-voxel grid folder
	-voxel_grid.cpp
	-CMakefiles.txt
	-make
		-(make file stuff)

NOTE: just as a reference, if you download all the files but did not install it unto your computer, there is a roundabout way which is on the cmake file, make sure to add (below) somewhere. Otherwise your computer won't be able to locate pcl's cmake file if you haven't downloaded pcl using sudo get install update.

	set(PCL_DIR "/home/[[user]/pcl/release/")

Now before we start on our implementation of a standalone voxelgrid filter, we need to understand how their voxel filter works. First is data representation. For all intents and purposes I'm going to store my data like below. This will account for all information needed.

x y z normal_x normal_y normal_z rgb

<Approach 1>

1. Central grid


Say that the leaf is <1,1,1> so the center of this portion is <.5,.5,.5>. We find all the points +-.5 x , +-.5 y, +-.5 z, condense it into 1 point located at the center by finding the average color (and normal somehow). The loop interval would be

int leaf_x = 1; int leaf_y = 1; int leaf_y = 1; 
