// Copyright 2007 Gilberto Echeverria
//
// This file is part of ObjReader.


#include <stdio.h>
#include <stdlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/keysym.h>

#include "tools.h"
#include "glTextures.h"

typedef struct {
    int width;
    int height;
    unsigned char *data;
} textureImage;


/* simple loader for 24bit bitmaps (data is in rgb-format) */
int loadBMP(char *filename, textureImage *texture)
{
    FILE *file;
    unsigned short int bfType;
    long int bfOffBits;
    short int biPlanes;
    short int biBitCount;
    long int biSizeImage;
    int i;
    unsigned char temp;

    texture->data = NULL;

    /* make sure the file is there and open it read-only (binary) */
    if ((file = fopen(filename, "rb")) == NULL)
    {
        printf("File not found : %s\n", filename);
        return 0;
    }
    if(!fread(&bfType, sizeof(short int), 1, file))
    {
        printf("Error reading file!\n");
        return 0;
    }
    /* check if file is a bitmap */
    if (bfType != 19778)
    {
        printf("Not a Bitmap-File!\n");
        return 0;
    }
    /* get the file size */
    /* skip file size and reserved fields of bitmap file header */
    fseek(file, 8, SEEK_CUR);
    /* get the position of the actual bitmap data */
    if (!fread(&bfOffBits, sizeof(long int), 1, file))
    {
        printf("Error reading file!\n");
        return 0;
    }
// printf("Data at Offset: %ld\n", bfOffBits);
    /* skip size of bitmap info header */
    fseek(file, 4, SEEK_CUR);
    /* get the width of the bitmap */
    fread(&texture->width, sizeof(int), 1, file);
// printf("Width of Bitmap: %d\n", texture->width);
    /* get the height of the bitmap */
    fread(&texture->height, sizeof(int), 1, file);
// printf("Height of Bitmap: %d\n", texture->height);
    /* get the number of planes (must be set to 1) */
    fread(&biPlanes, sizeof(short int), 1, file);
    if (biPlanes != 1)
    {
        printf("Error: number of Planes not 1!\n");
        return 0;
    }
    /* get the number of bits per pixel */
    if (!fread(&biBitCount, sizeof(short int), 1, file))
    {
        printf("Error reading file!\n");
        return 0;
    }
    // printf("Bits per Pixel: %d\n", biBitCount);
    if (biBitCount != 24)
    {
        printf("Bits per Pixel not 24\n");
        return 0;
    }
    // calculate the size of the image in bytes
    biSizeImage = texture->width * texture->height * 3;

// printf("Size of the image data: %ld\n", biSizeImage);
    texture->data = malloc(biSizeImage);
    /* seek to the actual data */
    fseek(file, bfOffBits, SEEK_SET);
    if (!fread(texture->data, biSizeImage, 1, file))
    {
        printf("Error loading file!\n");
        return 0;
    }
    // swap red and blue (bgr -> rgb)
    for (i = 0; i < biSizeImage; i += 3)
    {
        temp = texture->data[i];
        texture->data[i] = texture->data[i + 2];
        texture->data[i + 2] = temp;
    }
    return 1;
}


// Initialize a texture pointer for OpenGL.
// Load Bitmaps And Convert To Textures.
void loadGLTextures(GLuint* textureIndex, char* fileName, textureFilter filter)
{
    textureImage	*texti = NULL;

    texti = xmalloc (sizeof(textureImage));

    if (loadBMP(fileName, texti))
    {
        glGenTextures(1, textureIndex);   /* create the textures */

		switch (filter)
		{
			case NEAR:	// No filter
				glBindTexture(GL_TEXTURE_2D, *textureIndex);
				// actually generate the texture
				glTexImage2D(GL_TEXTURE_2D, 0, 3, texti->width, texti->height, 0,
					GL_RGB, GL_UNSIGNED_BYTE, texti->data);
				// set filtering method
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;

			case LINEAR: 	// Linear filter
				glBindTexture(GL_TEXTURE_2D, *textureIndex);
				// actually generate the texture
				glTexImage2D(GL_TEXTURE_2D, 0, 3, texti->width, texti->height, 0,
					GL_RGB, GL_UNSIGNED_BYTE, texti->data);
				// set filtering method
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			case MIPMAP:	// Mip Mapped texture
				glBindTexture(GL_TEXTURE_2D, *textureIndex);
				// actually generate the texture
				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texti->width, texti->height,
					GL_RGB, GL_UNSIGNED_BYTE, texti->data);
				// set filtering method
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				break;
		}
    }
    else
	    printf ("Unable to load textures!\n");

    /* free the ram we used in our texture generation process */
    if (texti)
    {
        if (texti->data)
            free(texti->data);
        free(texti);
    }
}
