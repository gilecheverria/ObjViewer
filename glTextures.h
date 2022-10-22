#define BACK_ID		0	// The texture ID for the back side of the cube
#define FRONT_ID	1	// The texture ID for the front side of the cube
#define BOTTOM_ID	2	// The texture ID for the bottom side of the cube
#define TOP_ID		3	// The texture ID for the top side of the cube
#define LEFT_ID		4	// The texture ID for the left side of the cube
#define RIGHT_ID	5	// The texture ID for the right side of the cube

typedef enum {NEAR, LINEAR, MIPMAP}	textureFilter;

// Function declarations.

void loadGLTextures(GLuint* textureIndex, char* fileName, textureFilter filter);
