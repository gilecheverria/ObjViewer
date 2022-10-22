// Copyright 2007 Gilberto Echeverria
//
// This file is part of ObjReader.


#include <math.h>
#include "tools.h"
#include "lists.h"
#include "vertices.h"
#include "geometry.h"


// Find the smallest of two numbers
double getMin (double first, double second)
{
	if (first < second)
		return (first);
	else
		return (second);
}


// Find the largest of two numbers
double getMax (double first, double second)
{
	if (first > second)
		return (first);
	else
		return (second);
}


double distance2Vertices (vertexStruct vertex1, vertexStruct vertex2)
{
	return ( sqrtf (squareDistance (vertex1, vertex2)) );
}


// Returns the square of the distance between 2 vertices.
double squareDistance (vertexStruct vertex1, vertexStruct vertex2)
{
	double	xDifference;
	double	yDifference;
	double	zDifference;

	xDifference = vertex2.x - vertex1.x;
	yDifference = vertex2.y - vertex1.y;
	zDifference = vertex2.z - vertex1.z;

	return ( (xDifference * xDifference) + (yDifference * yDifference) + (zDifference * zDifference) );
}


// Allocate memory and fill in the data for a segment structure.
segmentPtr createSegment (int vertex1, int vertex2, int info)
{
	segmentPtr	newSegment = NULL;

	newSegment = (segmentPtr) xmalloc (sizeof (segmentStruct));
	newSegment->vertex1 = vertex1;
	newSegment->vertex2 = vertex2;
	newSegment->info = info;

	return (newSegment);
}


// Find the components of a vector, enclosed between two vertices.
vectorPtr vectorFromVertices (vertexPtr vertex1, vertexPtr vertex2)
{
	vectorPtr	newVector;

	newVector = (vectorPtr) xmalloc (sizeof (vectorStruct));

	newVector->i = vertex2->x - vertex1->x;
	newVector->j = vertex2->y - vertex1->y;
	newVector->k = vertex2->z - vertex1->z;

	return (newVector);
}


// Find the angle formed by three vertices, by creating 2 vectors.
double angleBetweenVertices (vertexStruct vertex1, vertexStruct vertex2, vertexStruct vertex3)
{
	vectorPtr	vector1 = NULL;
	vectorPtr	vector2 = NULL;
	double		angle;

	vector1 = vectorFromVertices (&vertex1, &vertex2);
	vector2 = vectorFromVertices (&vertex2, &vertex3);

	angle = angleBetweenVectors (vector1, vector2);

	free (vector1);
	free (vector2);

	return (angle);
}


// Compute the angle between two vectors.
double angleBetweenVectors (vectorPtr vector1, vectorPtr vector2)
{
	double	length1;
	double	length2;
	double	lengthProduct;
	double	cosine;

	length1 = vectorLength (vector1);
	length2 = vectorLength (vector2);
	lengthProduct = length1 * length2;

	if (lengthProduct == 0)
	{
		printf ("Division by zero in 'angleBetweenVectors'. Returning zero\n");
		return (0.0);
	}
	else
	{
		cosine = dotProduct (vector1, vector2) / lengthProduct;

		// Check that the cosine is within logical boundaries.
		if (cosine > 1)
			return ( acos (1.0) );
		else if (cosine < -1)
			return ( acos (-1.0) );
		else
			return ( acos (cosine) );
	}
}


// Obtain the dot product of two vectors.
double dotProduct (vectorPtr vector1, vectorPtr vector2)
{
	return ( (vector2->i * vector1->i) + (vector2->j * vector1->j) + (vector2->k * vector1->k) );
}


// Compute the cross product of two vectors.
vectorPtr crossProduct (vectorPtr vector1, vectorPtr vector2)
{
	vectorPtr	newVector = NULL;

	newVector = (vectorPtr) xmalloc (sizeof (vectorStruct) );

	newVector->i = vector1->j * vector2->k - vector1->k * vector2->j;
	newVector->j = vector1->k * vector2->i - vector1->i * vector2->k;
	newVector->k = vector1->i * vector2->j - vector1->j * vector2->i;

	return (newVector);
}


// Sum two vectors.
vectorPtr sumVectors (vectorPtr vector1, vectorPtr vector2)
{
	vectorPtr	newVector = NULL;

	newVector = (vectorPtr) xmalloc (sizeof (vectorStruct) );

	newVector->i = vector1->i + vector2->i;
	newVector->j = vector1->j + vector2->j;
	newVector->k = vector1->k + vector2->k;

	return (newVector);
}

// Compute the length of a vector.
double vectorLength (vectorPtr vector)
{
	double	iSquared;
	double	jSquared;
	double	kSquared;

	iSquared = vector->i * vector->i;
	jSquared = vector->j * vector->j;
	kSquared = vector->k * vector->k;

	return ( sqrtf (iSquared + jSquared + kSquared) );
}


// Alter the size of a normal vector, to make it equal to 1.
void normalizeVector (vectorPtr vector)
{
	double		length;

	length = vectorLength (vector);

	if (length == 0.0f)
		length = 1.0f;

	vector->i /= length;
	vector->j /= length;
	vector->k /= length;
}


// Get the orientation of the triangle formed by three vertices.
// This is done by obtaining the slopes of the two segments generated by the vertices.
spinType orientationFromVertices_2D (vertexStruct vertex1, vertexStruct vertex2, vertexStruct vertex3)
{
	double	orientation;

	orientation = (vertex2.y - vertex1.y) * (vertex3.x - vertex2.x) - (vertex3.y - vertex2.y) * (vertex2.x - vertex1.x);

	// Clock Wise
	if ( orientation > (0 + EPSILON) )
		return (CW);
	// Counter Clock Wise
	else if ( orientation < (0 - EPSILON) )
		return (CCW);
	// Collinear
	else
		return (COL);
}


// Compute the slope of a segment structure.
double slopeFromSegment_2D (segmentPtr segment)
{
	vertexPtr	vertex1;
	vertexPtr	vertex2;

	vertex1 = (vertexPtr) segment->vertex1;
	vertex2 = (vertexPtr) segment->vertex2;

	return ( slopeFromVertices_2D (vertex1, vertex2) );
}


// Compute the slope of aline joining two vertices.
double slopeFromVertices_2D (vertexPtr vertex1, vertexPtr vertex2)
{
	return ( (vertex2->y - vertex1->y) / (vertex2->x - vertex1->x) );
}


// Check whether two segments intersect each other.
// Each segment is defined by its two endpoints.
boolean testSegmentsIntersect_2D (vertexStruct vertexA1, vertexStruct vertexA2, vertexStruct vertexB1, vertexStruct vertexB2)
{
	int		farVertex;
	spinType	orientation1;
	spinType	orientation2;
	spinType	orientation3;
	spinType	orientation4;

	orientation1 = orientationFromVertices_2D (vertexA1, vertexA2, vertexB1);
	orientation2 = orientationFromVertices_2D (vertexA1, vertexA2, vertexB2);
	orientation3 = orientationFromVertices_2D (vertexB1, vertexB2, vertexA1);
	orientation4 = orientationFromVertices_2D (vertexB1, vertexB2, vertexA2);

	// General case
	if ( (orientation1 != orientation2) && (orientation3 != orientation4) )
		return (TRUE);

	// Special case where the segments lie in the same line
	if ( (orientation1 ==  COL) && (orientation1 == orientation2) &&
	     (orientation2 == orientation3) && (orientation3 == orientation4) )
	{
		farVertex = findFarCollinearVertex (vertexA1, vertexA2, vertexB1);
		if (farVertex == vertexA2.vertexId)
			return (TRUE);
		else
		{
			farVertex = findFarCollinearVertex (vertexA1, vertexA2, vertexB2);
			if (farVertex == vertexA2.vertexId)
				return (TRUE);
		}
	}

	return (FALSE);
}


// Check whether two segments intersect each other.
boolean testSegmentsIntersect_NotInclusive_2D (vertexStruct vertexA1, vertexStruct vertexA2, vertexStruct vertexB1, vertexStruct vertexB2)
{
	int		farVertex;
	spinType	orientation1;
	spinType	orientation2;
	spinType	orientation3;
	spinType	orientation4;

	orientation1 = orientationFromVertices_2D (vertexA1, vertexA2, vertexB1);
	orientation2 = orientationFromVertices_2D (vertexA1, vertexA2, vertexB2);
	orientation3 = orientationFromVertices_2D (vertexB1, vertexB2, vertexA1);
	orientation4 = orientationFromVertices_2D (vertexB1, vertexB2, vertexA2);

	if ( !(	(vertexA1.vertexId == vertexB1.vertexId) ||
		(vertexA1.vertexId == vertexB2.vertexId) ||
		(vertexA2.vertexId == vertexB1.vertexId) ||
		(vertexA2.vertexId == vertexB2.vertexId)	) )
	{
		// Special case where one of the vertices in a segment
		//  lies on the other segment
		if (	( (orientation1 == COL) && (orientation2 != COL) ) ||
			( (orientation2 == COL) && (orientation1 != COL) ) ||
			( (orientation3 == COL) && (orientation4 != COL) ) ||
			( (orientation4 == COL) && (orientation3 != COL) )	)
				return (FALSE);
		// General case
		if ( (orientation1 != orientation2) && (orientation3 != orientation4) )
			return (TRUE);
	}

	// Special case where the segments lie in the same line
	if ( (orientation1 ==  COL) && (orientation1 == orientation2) &&
	     (orientation2 == orientation3) && (orientation3 == orientation4) )
	{
		farVertex = findFarCollinearVertex (vertexA1, vertexA2, vertexB1);
		if (	(vertexA1.vertexId != vertexB1.vertexId) &&
			(vertexA2.vertexId != vertexB1.vertexId) &&
			(farVertex == vertexA2.vertexId)		)
				return (TRUE);
		else
		{
			farVertex = findFarCollinearVertex (vertexA1, vertexA2, vertexB2);
			if (	(vertexA1.vertexId != vertexB2.vertexId) &&
				(vertexA2.vertexId != vertexB2.vertexId) &&
				(farVertex == vertexA2.vertexId)		)
					return (TRUE);
		}
	}

	return (FALSE);
}


// Test whether a segment intersects any of the segments in a curve.
boolean testSegmentCurveIntersect (vertexPtr vertexArray, nodePtr curve, vertexStruct vertex1, vertexStruct vertex2)
{
	nodePtr		curvePointer = curve;

	vertexStruct	curveVertex1;
	vertexStruct	curveVertex2;

	double		segmentMinY;
	double		segmentMaxY;
	double		curveMinY;
	double		curveMaxY;

	int*		vertexIndex = NULL;

	boolean		intersection = FALSE;

	vertexIndex = (int*) curvePointer->data;
	curveVertex1 = vertexArray[*vertexIndex];
	curvePointer = curvePointer->next;

	// Find the maximum and minimum values on the segment of interest.
	segmentMaxY = getMax (vertex1.y, vertex2.y);
	segmentMinY = getMin (vertex1.y, vertex2.y);

	while (curvePointer != NULL)
	{
		vertexIndex = (int*) curvePointer->data;
		curveVertex2 = vertexArray[*vertexIndex];

		// Find the maximum and minimum values on the current segment of the curve.
		curveMaxY = getMax (curveVertex1.y, curveVertex2.y);
		curveMinY = getMin (curveVertex1.y, curveVertex2.y);

		// Test for intersections only if any of the vertices of one segment falls
		//  within the range of the other segment.
		if (	( (curveVertex1.y <= segmentMaxY) && (curveVertex1.y >= segmentMinY) ) ||
			( (curveVertex2.y <= segmentMaxY) && (curveVertex2.y >= segmentMinY) ) ||
			( (vertex1.y <= curveMaxY) && (vertex1.y >= curveMinY) ) ||
			( (vertex2.y <= curveMaxY) && (vertex2.y >= curveMinY) )		)
		{
			if (	(vertex1.vertexId != curveVertex1.vertexId) &&
				(vertex1.vertexId != curveVertex2.vertexId) &&
				(vertex2.vertexId != curveVertex1.vertexId) &&
				(vertex2.vertexId != curveVertex2.vertexId)		)
				intersection = testSegmentsIntersect_2D (curveVertex1, curveVertex2, vertex1, vertex2);
			else
				intersection = testSegmentsIntersect_NotInclusive_2D (curveVertex1, curveVertex2, vertex1, vertex2);
			if (intersection)
				break;
		}

		curveVertex1 = curveVertex2;
		curvePointer = curvePointer->next;
	}

	return (intersection);
}


// Check which of two collinear vertices is farther from a reference vertex
// Returns the vertexId of the vertex that is farther away.
int findFarCollinearVertex (vertexStruct vertex1, vertexStruct vertex2, vertexStruct vertex3)
{
	vectorPtr	vector1_2 = NULL;
	vectorPtr	vector1_3 = NULL;
	double		length1_2;
	double		length1_3;
	double		cosine;
	int		farVertex;

	vector1_2 = vectorFromVertices (&vertex1, &vertex2);
	vector1_3 = vectorFromVertices (&vertex1, &vertex3);
	length1_2 = vectorLength (vector1_2);
	length1_3 = vectorLength (vector1_3);
	cosine = dotProduct (vector1_2, vector1_3) / (length1_2 * length1_3);

	// if ( (cosine > 0) && (length1_3 > length1_2) )

	// If the vertices are collinear in different directions,
	//  with respect to vertex1, then none is farther away.
	if (cosine < 0)
		farVertex = -1;
	else
	{
		if (length1_3 > length1_2)
			farVertex = vertex3.vertexId;
		else
			farVertex = vertex2.vertexId;
	}

	free (vector1_2);
	free (vector1_3);

	return (farVertex);
}


// Find the distance from a vertex to the segment defined within two other vertices.
// It returns the distance if the vertex is in front of the segment. If not, returns -1.
double distanceVertexToSegment (vertexStruct vertex, vertexStruct segmentVertex1, vertexStruct segmentVertex2)
{
	vectorPtr	segmentVector = NULL;
	vectorPtr	vectorToVertex = NULL;
	vertexPtr	newVertex = NULL;

	double		product1;
	double		product2;
	double		result = -1;
	double		b;

	segmentVector = vectorFromVertices (&segmentVertex2, &segmentVertex1);
	vectorToVertex = vectorFromVertices (&vertex, &segmentVertex1);

	// Distance to a segment from any vertex.
	product1 = dotProduct (vectorToVertex, segmentVector);
	if (product1 <= 0)
		result = distance2Vertices (vertex, segmentVertex1);
	else
	{
		product2 = dotProduct (segmentVector, segmentVector);
		if (product2 <= product1)
			result = distance2Vertices (vertex, segmentVertex2);

		else
		{
			b = product1 / product2;
			newVertex = createVertex (-1,	segmentVertex1.x - b * segmentVector->i,
							segmentVertex1.y - b * segmentVector->j,
							segmentVertex1.z - b * segmentVector->k	);
			result = distance2Vertices (vertex, *newVertex);
		}
	}

	/*
	// Distance to a segment from a vertex which can be projected
	//  inside of the segment.
	product1 = dotProduct (vectorToVertex, segmentVector);
	if (product1 == 0)
		result = distance2Vertices (vertex, segmentVertex1);
	else if (product1 > 0)
	{
		product2 = dotProduct (segmentVector, segmentVector);
		if (product2 == product1)
			result = distance2Vertices (vertex, segmentVertex2);

		else if (product1 < product2)
		{
			b = product1 / product2;
			newVertex = createVertex (-1,	segmentVertex1->x - b * segmentVector->i,
							segmentVertex1->y - b * segmentVector->j,
							segmentVertex1->z - b * segmentVector->k	);
			result = distance2Vertices (vertex, *newVertex);
		}
	}
	*/

	free (segmentVector);
	free (vectorToVertex);
	free (newVertex);

	return (result);
}


// Print the contents of a list of vectors.
void printVectorList (nodePtr list)
{
	nodePtr		pointer = list;
	vectorPtr	vector = NULL;

	printf ("Vector list:\t");

	while (pointer->next != NULL)
	{
		vector = (vectorPtr) pointer->data;
		printf ("%d, ", vector->vectorId);
		pointer = pointer->next;
	}

	vector = (vectorPtr) pointer->data;
	printf ("%d\n", vector->vectorId);
}
