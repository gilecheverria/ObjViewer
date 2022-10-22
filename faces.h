// Data structures

// FACE
typedef struct faceStruct
{
	int			faceId;
	int			faceVertices;
	int			vertex[4];
	vectorPtr	faceNormal;
} faceStruct;

typedef faceStruct* facePtr;


// Function definitions
void printFaceList (nodePtr list);

// Convert a linked list of data into an array.
facePtr listToFaceArray (nodePtr list, int listLength);

// Allocate memory for a face pointer
//  and fill it with null values.
facePtr createEmptyFace (void);
