CustomMemoryManager
===================

My own implementation of a Custom Memory Manager, that allocates ONLY equal sized blocks.

We are going to do bookkeeping in order to maintain the data.

We will store the bookmark header and the bookmarks from left -> right and data from right -> left [of Buffer].

The bookmark will hold 2 information, 
1. is the block empty
2. pointer to actual data block

The header contains general buffer and allocation information.

when we want to add data, we will create a bookmark from [NewBufferSize = BufferSize - sizeof(Header)], then we add the data at [buffer + (BufferSize - sizeof(T)].
we keep adding in this fashion when there are no empty blocks.

Removing a block, would simply mean setting the empty flag to true and updating the header [all of which is o(1)].
there is a little caveat here though, we are looping through the bookmarks to find which data block to remove, so that is o(m) for each removal, where m = no of bookmarks.

Adding a new block when there is an empty block; we search through the bookmark to find a bookmark that has empty set to true. that all.

Pros:
		1. no need to go through large data blocks for finding empty blocks. searching bookmarks much faster.
		2. very quick removal.
		3. very quick addition when empty blocks are present.
		4. very less fragmentation.
		5. Efficient use of block memory, very less empty blocks at the start of buffer, as we are filling up the first empty space we get.
		6. Index access much faster as there will be running through bookmark and not actual data blocks.

Cons:
		1. A little slower than initial design due to loops.
		2. requires additional memory for bookmark for each data block created [when there are no empty blocks.]
		
References:
		http://www.thinkmind.org/download.php?articleid=computation_tools_2012_1_10_80006
		http://eatplayhate.me/2010/09/04/memory-management-from-the-ground-up-2-foundations/
