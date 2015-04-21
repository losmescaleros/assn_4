#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stack>
#include <queue>
#include <math.h>
#include <string.h>

#include <main.h>

std::string indexFile;
int treeOrder;

FILE* fp;
// Offset to the B-tree root node
long root;

// Forward declarations
void
cmdLoop();
void
add(int k);
void
insertIntoArray(long* a, int n, long k, int index);
void
split(int* sortedKeys, long* children, std::stack<long> path, bool splitHappened);
void
find(int k);
bool
nodeContainsKey(int k, btree_node &node);
void
sortArray(int *a, int n);
void
printTree();
void
printNodeByOffset(long offset);
void
printNode(btree_node &node);
void
readNode(long offset, btree_node &node);
void
writeRoot();
void
writeNode(btree_node node, long offset);

int 
main(int argc, char* argv[]){
	// Program arguments should be of form: "assn_4 index-file order"
	if (argc != 3){
		printf("Incorrect number of arguments");
		exit(EXIT_ERROR);
	}

	indexFile = argv[1];
	std::string strTreeOrder = argv[2];
	treeOrder = atoi(strTreeOrder.c_str());

	fp = fopen(indexFile.c_str(), "r+b");
	// If the file doesn't exist, set root offset to unknown, otherwise read root offset 
	// at front of file
	if (fp == NULL){
		root = -1;
		fp = fopen(indexFile.c_str(), "w+b");
		// Write root to newly created index file
		fwrite(&root, sizeof(long), 1, fp);
	} 
	else{
		// File exists. Read root offset from front
		fread(&root, sizeof(long), 1, fp);
	}

	cmdLoop();

	exit(EXIT_SUCCESS);
}


void
cmdLoop(){
	while (true){
		std::string input = "";

		std::string cmd;
		std::cin >> cmd;
		
		if (cmd.compare(CMD_ADD) == 0){
			int k;
			std::cin >> k;
			add(k);
		} 
		else if (cmd.compare(CMD_FIND) == 0){
			int k;
			std::cin >> k;
			find(k);
		} 
		else if (cmd.compare(CMD_PRINT) == 0){
			printTree();
		} 
		else if (cmd.compare(CMD_END) == 0){
			// Update root node's offset at front of index
			fseek(fp, 0, SEEK_SET);
			fwrite(&root, sizeof(long), 1, fp);
			// Close index file
			fclose(fp);
			// End program
			break;
		}
		else if (cmd.compare("test") == 0)
		{
			add(1);
			add(2);
			add(3);
			add(4);
			add(5);
			add(6);
			add(7); 
			add(8);
			add(9);
			add(10);
			add(11);
			add(12);
			add(13);
		}
		else{
			printf("Unrecognized command.\n");
		}
	}
}

void
setArrayToZero(int* a, int n)
{
	for (int i = 0; i < n; i++)
	{
		a[i] = 0;
	}
}

void
add(int k){
	btree_node n;
	n.n = 0;
	n.key = (int*)calloc(treeOrder - 1, sizeof(int));
	n.child = (long *)calloc(treeOrder, sizeof(long));
	int* keyList = (int*)calloc(treeOrder, sizeof(int));
	// If the tree is empty, we can insert into a new root
	if (root == -1)
	{
		n.key[n.n++] = k;
		root = sizeof(long);
		writeNode(n, root);
	}
	else // Otherwise, the tree is not empty. Traverse to find correct leaf
	{
		readNode(root, n);
		if (nodeContainsKey(k, n))
		{
			printf("Entry with key=%d already exists\n", k);
			return;
		}
		std::stack<long> path;
		path.push(root);
		// 1. Search the tree for leaf node L responsible for k
		while (n.child[0] != 0)
		{
			int i;
			for (i = 0; i < n.n; i++)
			{
				if (k == n.key[i])
				{
					printf("Entry with key=%d already exists\n", k);
					return;
				}
				if (k < n.key[i])
				{
					break;
				}
				else if (k > n.key[i] && k < n.key[i + 1])
				{
					i++;
					break;
				}
				else
				{
					continue;
				}
			}
			path.push(n.child[i]);
			readNode(n.child[i], n);
			if (nodeContainsKey(k, n))
			{
				printf("Entry with key=%d already exists\n", k);
				return;
			}
		}
		// 2. Create a new key list with L's keys plus k sorted in ascending order
		memcpy(keyList, n.key, n.n * sizeof(int));
		keyList[n.n] = k;
		sortArray(keyList, n.n + 1);
		// 3. If L's key list is not full, replace it with new list and write L
		if (n.n != treeOrder - 1)
		{			
			n.key[n.n++] = k;
			sortArray(n.key, n.n);
			writeNode(n, path.top());
		}
		else // 4. Otherwise, split key list about its median value
		{
			long* children = (long*)calloc(treeOrder + 1, sizeof(long));
			memcpy(children, n.child, sizeof(long) * treeOrder);
			split(keyList, children, path, false);
		}
	}

	free(n.key);
	free(n.child);
	free(keyList);
}

void
split(int* sortedKeys, long* children, std::stack<long> path, bool splitHappened)
{
	btree_node l_node;
	l_node.n = 0;
	l_node.child = (long*)calloc(treeOrder, sizeof(long));
	l_node.key = (int*)calloc(treeOrder - 1, sizeof(int));
	// readNode(path.top(), l_node);
	btree_node r_node;
	r_node.n = 0;
	r_node.child = (long*)calloc(treeOrder, sizeof(long));
	r_node.key = (int*)calloc(treeOrder - 1, sizeof(int));

	int medIndex = treeOrder / 2;
	l_node.n = medIndex;
	r_node.n = treeOrder - 1 - l_node.n;

	int median = sortedKeys[medIndex];
	for (int i = 0; i < l_node.n; i++)
	{
		l_node.key[i] = sortedKeys[i];
	}
	for (int i = 0; i < r_node.n; i++)
	{
		r_node.key[i] = sortedKeys[medIndex + 1 + i];
	}
	for (int i = 0; i < l_node.n + 1; i++)
	{
		l_node.child[i] = children[i];
	}
	/*if (splitHappened)
	{
		for (int i = 0; i < r_node.n + 2; i++)
		{
			r_node.child[i] = children[medIndex + 1 + i];
		}
	}
	else
	{*/
		for (int i = 0; i < r_node.n + 1; i++)
		{
			r_node.child[i] = children[medIndex + 1 + i];
		}
	/*}*/
	long l_off = path.top();
	writeNode(l_node, l_off);
	path.pop();
	fseek(fp, 0, SEEK_END);
	long r_off = ftell(fp);
	writeNode(r_node, r_off);

	btree_node p_node;
	p_node.n = 0;
	p_node.child = (long*)calloc(treeOrder, sizeof(long));
	p_node.key = (int*)calloc(treeOrder - 1, sizeof(int));
	if (path.size() > 0)
	{
		
		readNode(path.top(), p_node);
		memcpy(children, p_node.child, sizeof(long) * treeOrder);
		children[treeOrder] = 0;
		sortedKeys[treeOrder - 1] = 0;
		memcpy(sortedKeys, p_node.key, sizeof(int) * (treeOrder - 1));
		
		sortedKeys[p_node.n] = median;
		sortArray(sortedKeys, treeOrder);
		int index;
		for (int i = 0; i < treeOrder; i++)
		{
			if (sortedKeys[i] == median)
			{
				index = i;
				break;
			}
		}

		insertIntoArray(children, treeOrder + 1, r_off, index + 1);

		if (p_node.n == treeOrder - 1)
		{
			split(sortedKeys, children, path, true);
		}
		else
		{
			memcpy(p_node.key, sortedKeys, sizeof(int) * (treeOrder - 1));
			p_node.n++;
			memcpy(p_node.child, children, sizeof(long) * treeOrder);
			writeNode(p_node, path.top());
		}
		
	}
	else
	{
		p_node.key[p_node.n++] = median;
		p_node.child[0] = l_off;
		p_node.child[1] = r_off;
		fseek(fp, 0, SEEK_END);
		long p_off = ftell(fp);
		writeNode(p_node, p_off);
		root = p_off;
	}

	free(p_node.key);
	free(p_node.child);
	free(l_node.key);
	free(l_node.child);
	free(r_node.key);
	free(r_node.child);
}

void
find(int k)
{
	if (root != -1)
	{
		
	}
	btree_node n;
	n.n = 0;
	n.key = (int*)calloc(treeOrder - 1, sizeof(int));
	n.child = (long *)calloc(treeOrder, sizeof(long));

	readNode(root, n);

	if (nodeContainsKey(k, n))
	{
		printf("Entry with key=%d exists\n", k);
		return;
	}

	while (n.child[0] != 0)
	{
		int i;
		for (i = 0; i < n.n; i++)
		{
			if (k == n.key[i])
			{
				printf("Entry with key=%d exists\n", k);
				return;
			}
			if (k < n.key[i])
			{
				break;
			}
			else if (k > n.key[i] && k < n.key[i + 1])
			{
				i++;
				break;
			}
			else
			{
				continue;
			}
		}

		readNode(n.child[i], n);
		if (nodeContainsKey(k, n))
		{
			printf("Entry with key=%d exists\n", k);
			return;
		}
	}
	
	printf("Entry with key=%d does not exist\n", k);
}

void
insertIntoArray(long* a, int n, long k, int index)
{
	int tempSize = n - index - 1;
	long* temp = (long*)calloc(tempSize, sizeof(long));
	for (int i = 0; i < tempSize; i++)
	{
		temp[i] = a[index + i];
	}

	a[index] = k;
	for (int i = 0; i < tempSize; i++)
	{
		a[index + 1 + i] = temp[i];
	}

	free(temp);
}

bool
nodeContainsKey(int k, btree_node &node)
{
	for (int i = 0; i < node.n; i++)
	{
		if (node.key[i] == k)
		{
			return true;
		}
	}

	return false;
}

void
sortArray(int *a, int n){
	int temp;

	for (int i = 0; i < n; i++){
		for (int j = i; j < n; j++){
			if (a[i] > a[j] && a[j] != 0)
			{
				temp = a[i];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}
}

void
printTree(){
	int level = 1;
	if (root == -1)
	{
		printf("%2d:", level);
	}
	else
	{
		std::queue<long> offsets;
		std::queue<long> children;
		offsets.push(root);
		btree_node t_node;
		t_node.n = 0;
		t_node.key = (int*)calloc(treeOrder - 1, sizeof(int));
		t_node.child = (long*)calloc(treeOrder, sizeof(long));
		printf("%2d: ", level);
		while (!offsets.empty())
		{
			readNode(offsets.front(), t_node);
			offsets.pop();
			printNode(t_node);
			int c = 0;
			for (int i = 0; i < treeOrder; i++)
			{
				
				if (t_node.child[i] != 0)
				{
					children.push(t_node.child[i]);
					c++;
				}
			}
			if (offsets.empty() && !children.empty())
			{
				printf("\n%2d: ", ++level);
				std::swap(offsets, children);
			}
		}
	}
	printf("\n");
}

void
printNodeByOffset(long offset){
	btree_node t_node;
	t_node.n = 0;
	t_node.key = (int*)calloc(treeOrder - 1, sizeof(int));
	t_node.child = (long*)calloc(treeOrder, sizeof(long));

	readNode(offset, t_node);
	printNode(t_node);

	free(t_node.key);
	free(t_node.child);
}

void 
printNode(btree_node &node){
	int i;
	for (i = 0; i < node.n - 1; i++){
		printf("%d,", node.key[i]);
	}
	printf("%d ", node.key[node.n - 1]);
}

void
readNode(long offset, btree_node &node){
	fseek(fp, offset, SEEK_SET);

	fread(&node.n, sizeof(int), 1, fp);
	fread(node.key, sizeof(int), treeOrder - 1, fp);
	fread(node.child, sizeof(long), treeOrder, fp);
}

void
writeRoot(){
	fseek(fp, 0, SEEK_SET);
	fwrite(&root, sizeof(long), 1, fp);
}

void
writeNode(btree_node node, long offset)
{
	if (offset == -1)
	{
		fseek(fp, 0, SEEK_END);
	}
	else
	{
		fseek(fp, offset, SEEK_SET);
	}
	
	fwrite(&node.n, sizeof(int), 1, fp);
	fwrite(node.key, sizeof(int), treeOrder - 1, fp);
	fwrite(node.child, sizeof(long), treeOrder, fp);
}
