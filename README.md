Mitchell Neville

CSC 541 - Assignment 4

# Disk B-Trees

This C++ application is intended as an exercise in disk persisted B-trees. While deletion is not supported in this implementation, insertion and searching are. The B-tree supports integer keys and an order greater or equal to three. The general algorithm used for adding in this implementaion is as follows:

1. Search the B-tree for the leaf node L responsible for k. If k is stored in L's key list, print
````Entry with key=k already exists````
on-screen and stop, since duplicate keys are not allowed.
2. Create a new key list K that contains L's keys, plus k, sorted in ascending order.
3. If L's key list is not full, replace it with K, update L's child offsets, write L back to disk, and stop.
4. Otherwise, split K about its median key value k_m into left and right key lists K_L = (k0, ... , k_m-1) and K_R = (k_m+1, ... , k_n-1). Use ceiling to calculate m = ⌈(n-1)/2⌉. For example, if n = 3, m = 1. If n = 4, m = 2.
5. Save K_L and its associated child offsets in L, then write L back to disk.
6. Save K_R and its associated child offsets in a new node R, then append R to the end of the index file.
7. Promote k_m , L's offset, and R's offset and insert them in L's parent node. If the parent's key list is full, recursively split its list and promote the median to its parent.
8. If a promotion is made to a root node with a full key list, split and create a new root node holding k_m and offsets to L and R.

# File Structure

Example input and output files are located in ````data/examples````. The main C++ source is located in the ````src```` directory, and header files are located in the ````include```` directory. A makefile is provided at the root level. 

# Running the Program
The application has been targeted toward a Linux platform. A makefile has been included for compilation and running tests. In order to run the program, navigate to the root directory for the project, then run
````
$ make
````
After this, you can execute the application. The application requires two arguments: the *binary file for the index* and the *order* for the B-tree. It is assumed that degree will be greater or equal to 3. In general, you run the application like:
````
$ ./assn_4 <index file> <B-tree order>
````
For example, you could save a degree-4 B-tree to a file called ````index.bin```` using the following command:
````
$ ./assn_4 index.bin 4
````
If the index file does not exist, then it is created and a new B-tree is started. Otherwise, it is assumed that the existing index matches the order specified in the command argument. 

In order to remove obj files and directories, you can run
````
$ make clean
````

You may also run some of the example inputs using:
````
$ make test
````
This will create application output files with names like ````my-output-01a.txt```` at the root level of the project. These are then diff'ed against the expected output files in the examples directory. The output from the diff is located in a file called ````test_results.txt```` at the root level. 

## User Interface

Once the application has started, you may enter commands for inserting, searching, and printing the B-tree. The four commands are as follows:

- ````add k```` Add a new integer key with a value k to the binary index
- ````find k```` Find an entry with a key value of ````k```` in the index if it exists
- ````print```` Print the contents and the structure of the B-tree on-screen
- ````end```` Update the root node's offset at the front of the index file and close the indexfile, ending the program

### Interpreting the Output

An example of starting a new order-4 B-tree, inserting some keys, and seeing the results is as follows:

````
add 1
add 2
add 3
print
1: 1,2,3
add 4
print
1: 3
2: 1,2 4
...continue adding until 13...
print
1: 9 2: 3,6 12 3: 1,2 4,5 7,8 10,11 13
````
