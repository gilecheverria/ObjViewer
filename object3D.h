// Data structures

// OBJECT

typedef struct objectStruct
{
	int			numVertices;
	int			numFaces;
	int			numNormals;
	int			numEdges;

	boolean		textured;

	vertexPtr	vertexArray;
	vertexPtr	displayVertexArray;
	vertexPtr	textureArray;
	vertexPtr	normalArray;
	facePtr		faceArray;
	nodePtr		edgeList;
	nodePtr*	vertexFaceArray;
} objectStruct;

typedef objectStruct* objectPtr;
