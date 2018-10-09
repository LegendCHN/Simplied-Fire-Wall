# Simplied-Fire-Wall
In my design, I will have four trees representing inbound/tcp, inbound/udp, outbound/tcp, outbound/udp.
Each tree node contains two important values: 1.the node's port range 2. the node's valid ip range (contained in a set<unsigned int,unsigned int>, IP address is converted to unsigned int). Below is an example of one tree using only 1-8 ports.

							[1-8]
			[1-4]							[5-8]
	[1-2] 			[3-4]			[5-6]			[7-8]
[1-1]	[2-2]	[3-3]	[4-4]	[5-5]	[6-6]	[7-7]	[8-8]

Initially, I was considering using 4 unordered_map where each key represents one port. However, if the port range of one rule is 1-65535, I would add IP range for each key which is time-consuming and redundant. Therefore, I came up with above tree structure to reduce the time complexity. Even if the port number is from 1-65535, the space is still not much. In general, if there are N ports in total, the space complexity is around 4*O(2N-1). 

How do I add ip range to one node? If one rule comes in with port range 1-8, I can simply add that IP range to the root node. However, if it does not fit the node's port range, for exmaple rule's port range is now 4-7. I will need to go to the left node and right node respectively. At the end, I will add the range to port node [4-4], port node [5-6] and port node [7,7]. 

Once the tree is properly set up with each node containing its corresponding valid address. I can test it with one packet. For exmaple if the coming packing is with port number 6, I will first examine the [1-8] node's IP range. If allows the ip range, the accept_packet function simply returns true. Otherwise, I will go right and see whether node [5-8] allows that ip address. So on and so forth until I come to the node [6-6]. If that node still can not accpet the coming ip address. The function returns false. This results in a worst time complexity 
O(logN) which is travese until the leaf node. If in average every port has m valid ip ranges contained in the set, my algorithm goes through the entire set which results in a time complexity O(m) (This could be optimized using binary search).

Other optimizations:
1. For one port node, the valid ip address it contains might have overlap. Therefore, each time a new ip range is added into the set, I will have to traverse the set and see whethere is range I can merge with. However, all the merge will happen during the contructing stage. Therefore, the runtime complexity during the function accept_packet will not be affected.

Optimizations not achieved:
I can use binary search to find out whether the packet's ip address is valid when travese the ip range set. It can boost the time complexity from O(m) to O(logm).

Testing:
Due to time constraint, I only tested my algorithm with 4 test csv. First csv is the one from pdf; second csv is from port 1-65535 and ip range 0.0.0.0-255.255.255; third csv is empty; fourth csv is a case when merge can happen. My code passes all the cases above. However, this is definitely not enough. Ideally, I would generate more test cases using scripts which covers more cases.


Team Choices:
I am most interested in the platform team and data team. I have a good understanding in things like API, db, os and networking.


	