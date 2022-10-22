// Copyright 2007 Gilberto Echeverria
//
// This file is part of ObjReader.


#include <GL/gl.h>
#include <GL/glu.h>

#include "tools.h"
#include "lists.h"
#include "vertices.h"
#include "geometry.h"
#include "faces.h"
#include "edges.h"
#include "object3D.h"

#include "glTools.h"


extern double*	VertexAngleArray;
extern GLuint	textures[];

// Generate the display list for the model described
//  by the vertices and faces.
void gl_faces_to_display_list (GLuint list, objectStruct objectData, int colourType)
{
	vertexPtr	vertexArray = objectData.displayVertexArray;
	vertexPtr	normalArray = objectData.normalArray;
	vertexPtr	textureArray = objectData.textureArray;
	faceStruct	face;
	int			i;
	int			counter = 1;
	int			v1, v2, v3, v4;
	float		faceColour[4] = {0.4f,0.4f,0.4f,1.0f};

	GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
	// GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat low_shininess[] = { 5.0 };

	// Initialize the list, in compile mode.
	glNewList (list, GL_COMPILE);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);

	// Use textures if specified.
	if (objectData.textured)
	{
		glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		// glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, textures[1]);   /* select our  texture */
	}

	for (i=1; i<=objectData.numFaces; i++)
	{
		face = objectData.faceArray[i];
		v1 = face.vertex[0];
		v2 = face.vertex[1];
		v3 = face.vertex[2];
		v4 = face.vertex[3];

		glLoadName (counter++);

		if (face.faceVertices == 4)
		    glBegin (GL_QUADS);
		else
		    glBegin (GL_TRIANGLES);

			glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, faceColour);
			glColor4fv ( faceColour );

			glNormal3f (normalArray[v1].x, normalArray[v1].y, normalArray[v1].z);
			glVertex3f (vertexArray[v1].x, vertexArray[v1].y, vertexArray[v1].z);
			if (objectData.textured)
				glTexCoord2f(textureArray[v1].x, textureArray[v1].y);

			glNormal3f (normalArray[v2].x, normalArray[v2].y, normalArray[v2].z);
			glVertex3f (vertexArray[v2].x, vertexArray[v2].y, vertexArray[v2].z);
			if (objectData.textured)
				glTexCoord2f(textureArray[v2].x, textureArray[v2].y);

			glNormal3f (normalArray[v3].x, normalArray[v3].y, normalArray[v3].z);
			glVertex3f (vertexArray[v3].x, vertexArray[v3].y, vertexArray[v3].z);
			if (objectData.textured)
				glTexCoord2f(textureArray[v3].x, textureArray[v3].y);

			if (face.faceVertices == 4)
			{
				glNormal3f (normalArray[v4].x, normalArray[v4].y, normalArray[v4].z);
				glVertex3f (vertexArray[v4].x, vertexArray[v4].y, vertexArray[v4].z);
				if (objectData.textured)
					glTexCoord2f(textureArray[v4].x, textureArray[v4].y);
			}
		glEnd ();
	}

	glEndList ();
}


// Generate the display list for the model described
//  by the vertices and faces.
void gl_edges_to_display_list (GLuint list, objectStruct objectData)
{
	vertexPtr	vertexArray = objectData.displayVertexArray;
	GLint		polyModeParams[2];
	faceStruct	face;
	float		edgeColour[4] = {0.0f,0.0f,0.0f,1.0f};
	int			i;
	int			v1, v2, v3, v4;

	// Initialize the list, in compile mode.
	glNewList (list, GL_COMPILE);

	// Store the current polygon mode.
	glGetIntegerv (GL_POLYGON_MODE, polyModeParams);
	// Switch to line mode (wireframe).
	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, edgeColour);
	glColor4fv ( edgeColour );

	glLineWidth ( 2.0 );

	for (i=1; i<=objectData.numFaces; i++)
	{
		face = objectData.faceArray[i];
		v1 = face.vertex[0];
		v2 = face.vertex[1];
		v3 = face.vertex[2];
		v4 = face.vertex[3];

		if (face.faceVertices == 4)
		    glBegin (GL_QUADS);
		else
		    glBegin (GL_TRIANGLES);

			glVertex3f (vertexArray[v1].x, vertexArray[v1].y, vertexArray[v1].z);
			glVertex3f (vertexArray[v2].x, vertexArray[v2].y, vertexArray[v2].z);
			glVertex3f (vertexArray[v3].x, vertexArray[v3].y, vertexArray[v3].z);
			if (face.faceVertices == 4)
				glVertex3f (vertexArray[v4].x, vertexArray[v4].y, vertexArray[v4].z);
		glEnd ();
	}

	glLineWidth ( 1.0 );

	// Switch back to the previous state.
	glPolygonMode (GL_FRONT, polyModeParams[0]);
	glPolygonMode (GL_BACK, polyModeParams[1]);

	glDisable(GL_POLYGON_OFFSET_FILL);

	glEndList ();
}


// Generate the display list for the normals
//  to the vertices.
void gl_normals_to_display_list (GLuint list, objectStruct objectData)
{
	vertexPtr	vertexArray = objectData.displayVertexArray;
	vertexPtr	normalArray = objectData.normalArray;
	float		faceColour[4] = {1.0f,0.0f,1.0f,1.0f};
	double		v_x, v_y, v_z;
	double		n_x, n_y, n_z;
	int		i;
	int		numVertices = objectData.numVertices;

	// Initialize the list, in compile mode.
	glNewList (list, GL_COMPILE);

	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, faceColour);
	glColor4fv ( faceColour );

	for (i=1; i<=numVertices; i++)
	{
		v_x = vertexArray[i].x;
		v_y = vertexArray[i].y;
		v_z = vertexArray[i].z;

		n_x = normalArray[i].x / 1.0f;
		n_y = normalArray[i].y / 1.0f;
		n_z = normalArray[i].z / 1.0f;

		glBegin (GL_LINES);
			glVertex3f (v_x, v_y, v_z);
			glVertex3f (v_x+n_x, v_y+n_y, v_z+n_z);
		glEnd ();
	}

	glEndList ();
}
