// Data type definition
typedef struct edgeStruct
{
	int		edgeId;
	int		vertex1;
	int		vertex2;
} edgeStruct;

typedef edgeStruct* edgePtr;


// Function declarations
edgePtr newEdge (int counter, int edgeVertex1, int edgeVertex2);


void printEdgeList (nodePtr edgeList, int listLength);
