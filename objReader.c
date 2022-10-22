// Copyright 2007 Gilberto Echeverria
//
// This file is part of ObjReader.


#include <stdio.h>

#include "tools.h"
#include "lists.h"
#include "vertices.h"
#include "geometry.h"
#include "faces.h"
#include "edges.h"
#include "object3D.h"
#include "objParser.h"


// External functions
extern int glMain ();

// Global variables associated with the objects generated
objectStruct	ObjectData;

// Function declarations
void usage (char* programName);
void generateObject (char* inputFile);
void freeObjectMemory ();


/* program entry point */
int main (int argc, char* argv[])
{
	char		inputFile[60];

	if (argc < 2)
	{
		usage (argv[0]);
		exit (1);
	}

	strcpy (inputFile, argv[1]);

	////////	OBJECT ACQUISITION	////////
	printf ("\n[STAGE 1] -- READING DATA: '%s'\n", inputFile); fflush (stdout);
	generateObject (inputFile);
	printf ("[done]\n"); fflush (stdout);

	////////	RENDERING WITH OpenGL	////////
	printf ("\n[STAGE 2] -- GRAPHICAL DISPLAY\n"); fflush (stdout);
	glMain ();
	printf ("[done]\n"); fflush (stdout);

	freeObjectMemory ();

	return (0);
}


// Prints information about the arguments to supply to the program.
void usage (char* programName)
{
	printf ("Usage: %s [input file name]\n", programName);
	exit (1);
}


// Read the input file and generate an Object structure.
void generateObject (char* inputFile)
{
	// Read the input file and store the information in a list.
	ObjectData = readObjFile (inputFile);

	printf ("\tObject read has %d vertices and %d faces and %d edges\n", ObjectData.numVertices, ObjectData.numFaces, ObjectData.numEdges);
}


// Deallocate the memory used for the global variables.
void freeObjectMemory ()
{
	int			i;

	free (ObjectData.vertexArray);
	free (ObjectData.normalArray);
	freeList (ObjectData.edgeList);
	for (i=1; i<=ObjectData.numVertices; i++)
	{
		freeList (ObjectData.vertexFaceArray[i]);
	}
	for (i=1; i<=ObjectData.numFaces; i++)
	{
		free (ObjectData.faceArray[i].faceNormal);
	}
	free (ObjectData.faceArray);
	free (ObjectData.vertexFaceArray);
}
