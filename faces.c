// Copyright 2007 Gilberto Echeverria
//
// This file is part of ObjReader.


#include "tools.h"
#include "lists.h"
#include "vertices.h"
#include "geometry.h"
#include "faces.h"

//Print the contents of a list of faces.
void printFaceList (nodePtr list)
{
	nodePtr		pointer = list;
	facePtr		face = NULL;

	while (pointer != NULL)
	{
		face = pointer->data;
		printf ("Face: %d has %d vertices: %d, %d, %d", face->faceId, face->faceVertices, face->vertex[0], face->vertex[1], face->vertex[2]);
		if (face->faceVertices == 4)
			printf (", %d\n", face->vertex[3]);
		else
			printf ("\n");
		pointer = pointer->next;
	}
}


// Convert a linked list of data into an array.
facePtr listToFaceArray (nodePtr list, int listLength)
{
	nodePtr			pointer = list;
	facePtr			faceArray = NULL;
	int				counter = 0;

	faceArray = (facePtr) xmalloc (sizeof (faceStruct) * listLength);

	while (pointer != NULL)
	{
		memcpy (&faceArray[counter], pointer->data, sizeof(faceStruct));

		pointer = pointer->next;
		counter++;
	}

	return (faceArray);
}


// Allocate memory for a face pointer
//  and fill it with null values.
facePtr	createEmptyFace (void)
{
	int			i;
	facePtr		face = NULL;

	face = (facePtr) xmalloc (sizeof (faceStruct));

	face->faceId = 0;
	face->faceVertices = 0;
	face->faceNormal = NULL;
	for (i=0; i<4; i++)
		face->vertex[i] = 0;

	return (face);
}
