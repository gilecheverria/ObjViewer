// Generate the display list for the model described
//  by the vertices and faces.
void gl_faces_to_display_list (GLuint list, objectStruct objectData, int colourType);

// Generate the display list for the wireframe
//  to be shown on top of the actual object.
void gl_edges_to_display_list (GLuint list, objectStruct objectData);

// Generate the display list for the normals
//  to the vertices.
void gl_normals_to_display_list (GLuint list, objectStruct objectData);
