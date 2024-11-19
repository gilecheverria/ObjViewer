// Copyright 2007 Gilberto Echeverria
//
// This file is part of ObjReader.


#include "tools.h"
#include "lists.h"
#include "vertices.h"
#include "geometry.h"
#include "faces.h"
#include "edges.h"
#include "object3D.h"
#include "normals3D.h"
#include "objParser.h"

// Global variable to dump useless data.
char*		string;


// FUNCTION FOR DEBUGGING.
// Prints the contents of the vertexFaceArray
void printVertexFaceArray (nodePtr* vertexFaceArray, int numVertices)
{
	    int         i;
		int*        index;
		nodePtr     list;

		for (i=1; i<=numVertices; i++)
		{
			printf ("List for vertex %d:\t", i);
			list = vertexFaceArray[i];
			while (list != NULL)
			{
				index = (int*) list->data;
				printf ("%d ", *index);
				list = list->next;
			}
			printf ("\tEND OF LIST\n");
		}
}


// Parse an obj file and create lists with the vertices and faces.
// Also creates an array with lists of faces that converge
//  on each of the vertices.
objectStruct readObjFile (char* filename)
{
	FILE*			OBJ_FD = NULL;
	char			keyword;
	nodePtr			vertexList = NULL;
	nodePtr			vertexListTail = NULL;
	nodePtr			texture_vertex_list;
	nodePtr			texture_vertex_list_tail;
	nodePtr			faceList = NULL;
	nodePtr			faceListTail = NULL;
	// nodePtr			edgeList = NULL;
	nodePtr			finalEdgeList = NULL;
	nodePtr*		vertexFaceArray = NULL;
	vertexPtr		vertex = NULL;
	vertexPtr		texture_vertex = NULL;
	vertexPtr		vertexArray = NULL;
	vertexPtr		textureArray = NULL;
	vertexPtr		normalArray = NULL;
	facePtr			faceArray = NULL;
	facePtr			face = NULL;
	int				i;
	int				vertex_counter = 0;
	int				texture_counter = 0;
	int				face_counter = 0;
	// int				edge_counter = 0;
	int*			facesPerVertex = NULL;
	boolean			textured = FALSE;
	boolean			normals = FALSE;
	objectStruct	objectData;

	OBJ_FD = xfopen (filename, "r");

	string = (char*) xmalloc (sizeof (char) * 100);

	// Add an empty vertex at the beginning of the vertex list.
	// It will be disregarded, and thus the vertices will be
	//  indexed beginning with 1, when they are transfered to
	//  an array.
	vertex = createVertex (0, 0, 0, 0);
	vertexList = addNode (NULL, vertex);
	vertexListTail = vertexList;

	// Do the same for the list of texture vertices.
	texture_vertex = createVertex (0, 0, 0, 0);
	texture_vertex_list = addNode (NULL, texture_vertex);
	texture_vertex_list_tail = texture_vertex_list;

	// Add an empty face at the beginning of the face list.
	// It will be disregarded, and thus the faces will be
	//  indexed beginning with 1, when they are transfered to
	//  an array.
	face = createEmptyFace ();
	faceList = addNode (NULL, face);
	faceListTail = faceList;

	while (!feof (OBJ_FD))
	{
		keyword = getc (OBJ_FD);

		switch (keyword)
		{
			case 'v':	// A vertex definition
				keyword = getc (OBJ_FD);
				if (keyword == ' ' || keyword == '\t')	// Normal vertex
				{
					vertex = readVertex (OBJ_FD);
					vertexListTail = addNode (vertexListTail, vertex);
					vertex_counter++;
					vertex->vertexId = vertex_counter;
				}
				else if (keyword == 't')	// Texture vertex
				{
					textured = TRUE;
					texture_vertex = readTextureVertex (OBJ_FD);
					texture_vertex_list_tail = addNode (texture_vertex_list_tail, texture_vertex);
					texture_counter++;
					texture_vertex->vertexId = texture_counter;
				}
				else if (keyword == 'n')	// Normal vertex
				{
					normals = TRUE;
				}
				break;
			case 'f':	// A face definition
				face = readFace (OBJ_FD, textured, normals);
				faceListTail = addNode (faceListTail, face);
				face_counter++;
				face->faceId = face_counter;

				// Initialize arrays for vertex faces.
				if (vertexFaceArray == NULL)
				{
					vertexFaceArray = (nodePtr*) xmalloc ( sizeof (nodePtr) * (vertex_counter+1) );
					facesPerVertex = (int*) xmalloc ( sizeof(int) * (vertex_counter+1) );
					for (i=0; i<=vertex_counter; i++)
					{
						vertexFaceArray[i] = NULL;
						facesPerVertex[i] = 0;
					}
				}
				vertexFaceArray = addFaceToVertexFaceArray (vertexFaceArray, face, vertex_counter, facesPerVertex);
				// edgeList = getObjectEdges (face, edgeList, &finalEdgeList, &edge_counter, face_counter);
				break;
			case '\n':	// A blank line
				break;
			default:	// Any other case
				// Read and discard the line
				fgets(string, 100, OBJ_FD);
				break;
		}
	}

	free (string);

	fclose (OBJ_FD);

	// Convert the list of vertices into an array.
	// The array will have 'numVertices'+1 elements.
	// The element at index 0 is blank, allowing the
	//  real vertices to start from index 1.
	vertexArray = listToVertexArray (vertexList, vertex_counter+1);

	// Convert the list of texture vertices into an array.
	if (textured)
		textureArray = listToVertexArray (texture_vertex_list, texture_counter+1);

	// Obtain the normals for each individual vertex.
	normalArray = normalsFromVertices (vertexArray, faceList, vertex_counter);

	// Convert the list of faces into an array.
	// The array will have 'numFaces'+1 elements.
	// The element at index 0 is blank, allowing the
	//  real faces to start from index 1.
	faceArray = listToFaceArray (faceList, face_counter+1);

	// get_edge_orientation (finalEdgeList, faceArray, vertexArray);

	// Copy the data just read into the object structure
	objectData.numVertices = vertex_counter;
	objectData.numFaces = face_counter;
	objectData.numEdges = getListLength (finalEdgeList);
	objectData.textured = textured;
	objectData.vertexArray = vertexArray;
	objectData.displayVertexArray = NULL;
	objectData.normalArray = normalArray;
	objectData.faceArray = faceArray;
	objectData.edgeList = finalEdgeList;
	objectData.vertexFaceArray = vertexFaceArray;
	// objectData.facesPerVertex = facesPerVertex;

	if (textured)
		objectData.textureArray = textureArray;
	else
		objectData.textureArray = NULL;

	freeList (vertexList);
	freeList (texture_vertex_list);
	freeList (faceList);

	return (objectData);
}


// Create a new vertex structure with the information from
//  the .obj file.
vertexPtr readVertex (FILE* OBJ_FD)
{
	double		x;
	double		y;
	double		z;
	vertexPtr	newVertex = NULL;

	fscanf (OBJ_FD, "%lf %lf %lf", &x, &y, &z);

	newVertex = (vertexPtr) xmalloc (sizeof(vertexStruct));

	newVertex->x = x;
	newVertex->y = y;
	newVertex->z = z;
	newVertex->extra = 0;

	// Discard the rest of the line.
	fgets(string, 100, OBJ_FD);

	return (newVertex);
}


// Create a new vertex structure with the information from
//  the .obj file, for a texture vertex. Use only 2 coordinates.
vertexPtr readTextureVertex (FILE* OBJ_FD)
{
	double		x;
	double		y;
	vertexPtr	newVertex = NULL;

	fscanf (OBJ_FD, "%lf %lf", &x, &y);

	newVertex = (vertexPtr) xmalloc (sizeof(vertexStruct));

	newVertex->x = x;
	newVertex->y = y;
	newVertex->z = 0.0;
	newVertex->extra = 0;

	// Discard the rest of the line.
	fgets(string, 100, OBJ_FD);

	return (newVertex);
}


// Create a new face structure with the information from
//  the .obj file.
facePtr readFace (FILE* OBJ_FD, boolean textured, boolean normals)
{
	int			vertex[3];
	int			texture[3];
	int			normal[3];
	int			i;
	char		nextChar;
	facePtr		new_face = NULL;

	new_face = (facePtr) xmalloc (sizeof(faceStruct));

	new_face->faceVertices = 3;

	for (i=0; i<3; i++)
	{
		fscanf (OBJ_FD, "%d", &vertex[i]);
		if (textured)
		{
			fscanf (OBJ_FD, "/%d", &texture[i]);
			if (normals)
				fscanf (OBJ_FD, "/%d", &normal[i]);
		}
		else if (normals)
			fscanf (OBJ_FD, "//%d", &normal[i]);

		new_face->vertex[i] = vertex[i];
	}

	new_face->vertex[3] = -1;

	// Check to see if there is a fourth vertex in the face.
	nextChar = getc (OBJ_FD);

	// If the next character is different from:
	//  ASCII 10 = LF (line feed)
	//  ASCII 13 = CR (carriage return)
	if ( (nextChar != 10) && (nextChar != 13) && (nextChar != EOF) )
	{
		new_face->faceVertices = 4;

		fscanf (OBJ_FD, "%d", &vertex[i]);
		if (textured)
		{
			fscanf (OBJ_FD, "/%d", &texture[i]);
			if (normals)
				fscanf (OBJ_FD, "/%d", &normal[i]);
		}
		else if (normals)
			fscanf (OBJ_FD, "//%d", &normal[i]);

		// Se the fourth vertex. (i = 3)
		new_face->vertex[i] = vertex[i];

		// Discard the rest of the line.
		fgets(string, 100, OBJ_FD);
	}

	return (new_face);
}


// Add a new face to the array of faces per vertex.
// The 'faceId' will be inserted in the lists corresponding
//  to each of the vertices that compose the face.
nodePtr* addFaceToVertexFaceArray (nodePtr* vertexFaceArray, facePtr face, int numVertices, int* facesPerVertex)
{
	int			i;
	int			currentVertex;
	int*		index;
	nodePtr		tmpList = NULL;

	// For each of the vertices delimiting this face.
	for (i=0; i<face->faceVertices; i++)
	{
		currentVertex = face->vertex[i];
		// Get the list for the current vertex.
		tmpList = vertexFaceArray[currentVertex];
		// Copy the faceId to a new int.
		index = (int*) xmalloc (sizeof (int));
		memcpy (index, &(face->faceId), sizeof (int));
		// Insert the pointer into the list.
		tmpList = addFrontNode (tmpList, index);
		vertexFaceArray[currentVertex] = tmpList;

		// Increment the number of adjacent faces
		//  to the corresponding vertices.
		facesPerVertex[currentVertex] += 1;
	}

	return (vertexFaceArray);
}


/*
// Determine the list of edges defined in a 3D object.
nodePtr getObjectEdges (facePtr face, nodePtr edgeList, nodePtr* finalEdgeList, int *listLength, int faceId)
{
	edgePtr		edge = NULL;
	edgePtr		tempEdge = NULL;
	int			index;
	int			counter = *listLength;
	int			vertex1;
	int			vertex2;
	int			vertex3;
	int			edgeVertex1;
	int			edgeVertex2;
	static int	edge_counter = 1;

	vertex1 = face->vertex[0];
	vertex2 = face->vertex[1];
	vertex3 = face->vertex[2];

	// Sort the edge, so that the vertex with the lowest ID
	//  goes first. (Later used to identify duplicated edges).
	edgeVertex1 = (int) getMin (vertex1, vertex2);
	edgeVertex2 = (int) getMax (vertex1, vertex2);
	edge = newEdge (counter+1, edgeVertex1, edgeVertex2);
	edge->face1 = faceId;
	// Check that the edge is not already in.
	index = existingEdge (edgeList, edge, counter);
	// If the edge already exists.
	if (index >= 0)
	{
		// Remove the edge from the temp list.
		tempEdge = (edgePtr) removeNode (&edgeList, index);
		// Update the second adjacent face.
		tempEdge->face2 = faceId;
		tempEdge->edgeId = edge_counter++;
		// Add the edge to the final list.
		*finalEdgeList = addFrontNode (*finalEdgeList, tempEdge);
		counter--;
	}
	else
	{
		// Add the new edge.
		edgeList = addFrontNode (edgeList, edge);
		counter++;
	}

	edgeVertex1 = (int) getMin (vertex2, vertex3);
	edgeVertex2 = (int) getMax (vertex2, vertex3);
	edge = newEdge (counter+1, edgeVertex1, edgeVertex2);
	edge->face1 = faceId;
	// Check that the edge is not already in.
	index = existingEdge (edgeList, edge, counter);
	// If the edge already exists.
	if (index >= 0)
	{
		// Remove the edge from the temp list.
		tempEdge = (edgePtr) removeNode (&edgeList, index);
		// Update the second adjacent face.
		tempEdge->face2 = faceId;
		tempEdge->edgeId = edge_counter++;
		// Add the edge to the final list.
		*finalEdgeList = addFrontNode (*finalEdgeList, tempEdge);
		counter--;
	}
	else
	{
		// Add the new edge.
		edgeList = addFrontNode (edgeList, edge);
		counter++;
	}

	edgeVertex1 = (int) getMin (vertex3, vertex1);
	edgeVertex2 = (int) getMax (vertex3, vertex1);
	edge = newEdge (counter+1, edgeVertex1, edgeVertex2);
	edge->face1 = faceId;
	// Check that the edge is not already in.
	index = existingEdge (edgeList, edge, counter);
	// If the edge already exists.
	if (index >= 0)
	{
		// Remove the edge from the temp list.
		tempEdge = (edgePtr) removeNode (&edgeList, index);
		// Update the second adjacent face.
		tempEdge->face2 = faceId;
		tempEdge->edgeId = edge_counter++;
		// Add the edge to the final list.
		*finalEdgeList = addFrontNode (*finalEdgeList, tempEdge);
		counter--;
	}
	else
	{
		// Add the new edge.
		edgeList = addFrontNode (edgeList, edge);
		counter++;
	}

	*listLength = counter;

	return (edgeList);
}


// Check if an edge has already been inserted in the list.
int existingEdge (nodePtr edgeList, edgePtr edge, int listLength)
{
	edgePtr		listEdge;
	int			i;

	for (i=0; i<listLength; i++)
	{
		listEdge = (edgePtr) getNodeData (edgeList, i);

		if ( (listEdge->vertex1 == edge->vertex1) && (listEdge->vertex2 == edge->vertex2) )
		{
			return (i);
		}
	}

	return (-1);
}


// Find if an edge in a polygon is concave or convex.
void get_edge_orientation (nodePtr edgeList, facePtr faceArray, vertexPtr vertexArray)
{
	edgePtr			edge;
	faceStruct		face1;
	faceStruct		face2;
	vertexStruct	farVertex1;
	vertexStruct	farVertex2;
	int				i;
	int				j = 0;
	int				numEdges = getListLength (edgeList);
	int				farVertexId1;
	int				farVertexId2;
	double			baseDistance;
	double			endDistance;

	for (i=0; i<numEdges; i++)
	{
		edge = (edgePtr) getNodeData (edgeList, i);
		face1 = faceArray[edge->face1];
		face2 = faceArray[edge->face2];

		// Get a vertex of the faces that does not belong to the edge.
		j = 0;
		do
		{
			farVertexId1 = face1.vertex[j++];
		} while ( (farVertexId1 == edge->vertex1) || (farVertexId1 == edge->vertex2) );
		j = 0;
		do
		{
			farVertexId2 = face2.vertex[j++];
		} while ( (farVertexId2 == edge->vertex1) || (farVertexId2 == edge->vertex2) );

		farVertex1 = vertexArray[farVertexId1];
		farVertex2 = vertexArray[farVertexId2];

		// Get the distance from the two vertices.
		baseDistance = distance2Vertices (farVertex1, farVertex2);

		// Update the vertices, by adding the components of
		//  the normal vector of the face.
		farVertex1.x += (face1.faceNormal)->i;
		farVertex1.y += (face1.faceNormal)->j;
		farVertex1.z += (face1.faceNormal)->k;

		farVertex2.x += (face2.faceNormal)->i;
		farVertex2.y += (face2.faceNormal)->j;
		farVertex2.z += (face2.faceNormal)->k;

		// Get the new distance.
		endDistance = distance2Vertices (farVertex1, farVertex2);

		if (endDistance >= (baseDistance - EPSILON))
			edge->type = CONVEX;
		else
			edge->type = CONCAVE;
	}
}
*/
