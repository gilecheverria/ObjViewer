// Copyright 2007 Gilberto Echeverria
//
// This file is part of ObjReader.


#include <stdio.h>
#include "tools.h"
#include "lists.h"
#include "edges.h"


// Create a new edge pointer.
edgePtr newEdge (int counter, int edgeVertex1, int edgeVertex2)
{
	edgePtr		newEdge = NULL;

	newEdge = (edgePtr) xmalloc ( sizeof (edgeStruct) );

	newEdge->edgeId = counter;
	newEdge->vertex1 = edgeVertex1;
	newEdge->vertex2 = edgeVertex2;

	return (newEdge);
}


// Print the contents of a list of edges.
void printEdgeList (nodePtr edgeList, int listLength)
{
	edgePtr		edge = NULL;
	int			i;

	printf ("Edge list:\n");

	for (i=0; i<listLength; i++)
	{
		edge = getNodeData (edgeList, i);
		printf ("Edge %d ==>   %d - %d\n", edge->edgeId, edge->vertex1, edge->vertex2);
	}
}
