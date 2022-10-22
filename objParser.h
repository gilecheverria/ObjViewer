// Parse an obj file and create lists with the vertices
//  and faces.
objectStruct readObjFile (char* filename);

// Create a new vertex structure with the information from
//  the .obj file.
vertexPtr readVertex (FILE* OBJ_FD);

// Create a new vertex structure with the information from
//  the .obj file, for a texture vertex. Use only 2 coordinates.
vertexPtr readTextureVertex (FILE* OBJ_FD);

// Create a new face structure with the information from
//  the .obj file.
facePtr readFace (FILE* OBJ_FD, boolean textured, boolean normals);

// Add a new face to the array of faces per vertex.
// The 'faceId' will be inserted in the lists corresponding
//  to each of the vertices that compose the face.
nodePtr* addFaceToVertexFaceArray (nodePtr* vertexFaceArray, facePtr face, int numVertices, int* facesPerVertex);


// Determine the list of edges defined in a 3D object.
nodePtr getObjectEdges (facePtr face, nodePtr edgeList, nodePtr* finalEdgeList, int *listLength, int faceId);

// Check if an edge has already been inserted in the list.
int existingEdge (nodePtr edgeList, edgePtr edge, int listLength);


// Find if an edge in a polygon is concave or convex.
void get_edge_orientation (nodePtr edgeList, facePtr faceArray, vertexPtr vertexArray);
