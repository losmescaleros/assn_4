#define EXIT_SUCCESS 0
#define EXIT_ERROR -1

#define CMD_ADD "add"
#define CMD_FIND "find"
#define CMD_PRINT "print"
#define CMD_END "end"

typedef struct {
	// Number of keys in the node
	int n;
	// The node's key
	int* key;
	// Node's child subtee offsets
	long* child;
} btree_node;