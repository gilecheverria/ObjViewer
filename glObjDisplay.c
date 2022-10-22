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
#include "matrices.h"
#include "object3D.h"

#include "glObjectLists.h"
#include "glTools.h"
#include "glTextures.h"

#define AXES_LIST			0
#define	SPHERE_LIST			1

#define	OBJECT_LIST			2
#define WIREFRAME_LIST		3
#define	OBJECT_NORMALS_LIST	4

#define FIXED_LISTS			5

#define NUM_TEXTURES		3


// External variables
extern objectStruct		ObjectData;

extern GLUquadricObj*	quadratic;
extern GLuint			displayNormals;

// Variables for printing messages within OpenGL
extern int				width, height;
extern GLuint			font;

// Global variables
GLuint*			listIndex;
GLuint			textures[NUM_TEXTURES];


// Size of the normals
double			lengthFactor = 2.0f;
// Maximum length of objects to be displayed.
int 	        Max_Size = 3.0;



// Variables to select which areas to show.
boolean			ToggleDisplayInfo = FALSE;
boolean			ToggleCullFaces = TRUE;
boolean			ToggleWireframe = TRUE;
boolean			ToggleAxes = FALSE;
boolean			ToggleTextures = FALSE;


// Function declarations
void gl_prepare_scene ();
void gl_cleanup ();
void gl_prepare_scene_textures ();
void gl_draw_scene (int object);
void gl_create_display_lists ();
void gl_create_axis_list (GLuint list);
void gl_show_single_normal (int vertexNumber, objectStruct objectData);
void gl_quadric_sphere (GLuint list);
void gl_scene_event( int event, int data, int xpos, int ypos );

// External event handler function
extern void gl_event( int event, int data, int xpos,  int ypos );

// External function to print in OpenGL
extern void gl_printf( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, GLint x, GLint y, GLuint font, const char *format, ... );

// Call the functions to load the textures and to create the
//  display lists.
void gl_prepare_scene ()
{
	// Reduce the size of the object read, to fit in the display area.
	ObjectData.displayVertexArray = normalizeVertexArray (ObjectData.vertexArray, ObjectData.numVertices, Max_Size);

	// gl_prepare_scene_textures ();

	gl_create_display_lists ();
}


void gl_cleanup ()
{
	free (listIndex);
	free (quadratic);
}


// Initialize the pointers to the textures utilized.
void gl_prepare_scene_textures ()
{
	// char*   texture_bmp   = "duda-small.bmp";
	// char*   texture_bmp   = "duda-medium.bmp";
	char*   texture_bmp   = "duda-big.bmp";
	// char*   texture_bmp   = "../FaceRecognition/Textured_Data/duda.bmp";

	loadGLTextures (&textures[0], texture_bmp, NEAR);
	loadGLTextures (&textures[1], texture_bmp, LINEAR);
	loadGLTextures (&textures[2], texture_bmp, MIPMAP);
}


// Scene with the object and the arcs.
void gl_draw_scene_1 ()
{
	GLboolean		light_status;
	GLint			polyModeParams[2];

	// Store the current lighting status.
	gl_no_lighting (&light_status);

	if (ToggleTextures)
		glEnable (GL_TEXTURE_2D);
	else
		glDisable (GL_TEXTURE_2D);

	if (displayNormals)
		glCallList (listIndex[OBJECT_NORMALS_LIST]);

	// Draw the reference axes
	if (ToggleAxes)
		glCallList (listIndex[AXES_LIST]);

	gl_restore_lighting (light_status);

	// Draw the object.
	if (ToggleCullFaces)
		glEnable (GL_CULL_FACE);

	glCallList (listIndex[OBJECT_LIST]);

	// Draw the wireframe.
	if (ToggleWireframe)
	{
		// Store the current lighting status.
		gl_no_lighting (&light_status);

		// Store the current polygon mode.
		glGetIntegerv (GL_POLYGON_MODE, polyModeParams);
		// Switch to line mode (wireframe).
		glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
		// glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

		glCallList (listIndex[WIREFRAME_LIST]);

		// Switch back to the previous state.
		glPolygonMode (GL_FRONT, polyModeParams[0]);
		glPolygonMode (GL_BACK, polyModeParams[1]);

		gl_restore_lighting (light_status);
	}

	glDisable (GL_CULL_FACE);

	if (ToggleDisplayInfo)
	{
		if (displayNormals)
			gl_printf( 0.0f, 0.0f, 0.0f, 1.0f, 40, height - 40, font, "Showing Vertex Normals" );
	}
}


// Main function to generate all display lists to be used.
void gl_create_display_lists ()
{
	int				i;
	int				numLists;

	numLists = FIXED_LISTS;

	listIndex = (GLuint*) xmalloc (sizeof(GLuint) * numLists);

	// Allocate space for the Display lists, and initialize the indexes.
	listIndex[0] = glGenLists (numLists);
	if (listIndex[0] == 0)
	{
		printf ("Unable to create Display Lists.\nExiting.\n");
		exit (1);
	}
	for (i=1; i<numLists; i++)
		listIndex[i] = listIndex[i-1] + 1;

	gl_create_axis_list (listIndex[AXES_LIST]);
	gl_faces_to_display_list (listIndex[OBJECT_LIST], ObjectData, 1);
	gl_edges_to_display_list (listIndex[WIREFRAME_LIST], ObjectData);
	gl_normals_to_display_list (listIndex[OBJECT_NORMALS_LIST], ObjectData);

	gl_quadric_sphere (listIndex[SPHERE_LIST]);
}


// Draw the reference axis.
void gl_create_axis_list (GLuint list)
{
	float       red[4] = {1.0f,0.0f,0.0f,1.0f};
	float       green[4] = {0.0f,1.0f,0.0f,1.0f};
	float       blue[4] = {0.0f,0.0f,1.0f,1.0f};

	glNewList (list, GL_COMPILE);

		glBegin (GL_LINES);

		// Axis for X is RED
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
		glColor4fv( red );
		glVertex3f(-2.0f, 0.0f, 0.0f);
		glVertex3f(2.0f, 0.0f, 0.0f);

		// Axis for Y is GREEN
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
		glColor4fv( green );
		glVertex3f(0.0f, -2.0f, 0.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);

		// Axis for Z is BLUE
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
		glColor4fv( blue );
		glVertex3f(0.0f, 0.0f, -2.0f);
		glVertex3f(0.0f, 0.0f, 2.0f);

		glEnd ();

	glEndList ();
}


// Generate a star made up of the normals to the faces
//  of the object. This is done by making all the
//  normals begin at the origin.
void gl_show_single_normal (int vertexNumber, objectStruct objectData)
{
	float		faceColour[4] = {1.0f,0.0f,1.0f,1.0f};
	double		n_x, n_y, n_z;
	vertexStruct	normalVector;

	glColor4fv ( faceColour );
	glLineWidth( 3.0 );

 	normalVector = objectData.normalArray[vertexNumber];
	n_x = normalVector.x * 3;
	n_y = normalVector.y * 3;
	n_z = normalVector.z * 3;

	glBegin (GL_LINES);
		glVertex3f (0.0f, 0.0f, 0.0f);
		glVertex3f (n_x, n_y, n_z);
	glEnd ();

	glLineWidth( 1.0 );
}


// Generate a quadric sphere, to show as the surface
//  of the Gauss Map.
void gl_quadric_sphere (GLuint list)
{
	// Initialize the list, in compile mode.
	glNewList (list, GL_COMPILE);

		gluSphere(quadratic, lengthFactor, 32, 32);

	glEndList ();
}


// Function to handle scene specific events.
void gl_scene_event( int event, int data, int xpos, int ypos )
{
	//int		numVertices;

	//numVertices = ObjectData.numVertices;

	switch (event)
	{
		case 4:
			switch (data)
			{
				// Toggle the printing of messages.
				case '1':
					ToggleDisplayInfo ^= 1;
					break;

				// Toggle back face culling.
				case '2':
					ToggleCullFaces ^= 1;
					break;

				// Toggle display of the wireframe overlay.
				case '3':
					ToggleWireframe ^= 1;
					break;

				// Toggle display of the reference axis.
				case '4':
					ToggleAxes ^= 1;
					break;

				// Toggle display of the reference axis.
				case '5':
					ToggleTextures ^= 1;
					break;

				// Call the standard event handler.
				default:
					gl_event (event, data, xpos, ypos);
					break;
			}
			break;

		// Call the standard event handler.
		default:
			gl_event (event, data, xpos, ypos);
			break;
	}
}
