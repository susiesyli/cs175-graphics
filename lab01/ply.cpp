/*  =================== File Information =================
File Name: ply.cpp
Description: parses, renders, scales and centers a .ply object 
Author: Susie Li, Clarence Yeh, Qing Zhong 

Purpose:
Examples:
===================================================== */
#define _CRT_SECURE_NO_WARNINGS
#define GL_SILENCE_DEPRECATION

#include "ply.h"
#include "geometry.h"
#include <FL/gl.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string>

using namespace std;

/*  ===============================================
Desc: Default constructor for a ply object
Precondition:
Postcondition:
=============================================== */
ply::ply() {
    vertexList  = NULL;
    faceList    = NULL;
    properties  = 0;
    faceCount   = 0;
    vertexCount = 0;
}

/*  ===============================================
Desc: constructor for a ply object with a default path
Precondition:
Postcondition:
=============================================== */
ply::ply(string filePath) {
    vertexList  = NULL;
    faceList    = NULL;
    faceCount   = 0;
    vertexCount = 0;
    properties  = 0;
    reload(filePath);
}


/*  ===============================================
Desc: Destructor for a ply object
Precondition: Memory has been already allocated
Postcondition:
=============================================== */
ply::~ply() {
    // Delete the allocated arrays
    if (vertexList != NULL) delete[] vertexList;

    for (int i = 0; i < faceCount; i++) {
        delete[] faceList[i].vertexList;
    }

    if (faceList != NULL) delete[] faceList;
    // Set pointers to NULL
    vertexList = NULL;
    faceList   = NULL;
}

/*  ===============================================
Desc: reloads the geometry for a 3D object
Precondition:
Postcondition:
=============================================== */
void ply::reload(string _filePath) {
    filePath = _filePath;
    // reclaim memory allocated in each array
    if (vertexList != NULL) delete[] vertexList;
    for (int i = 0; i < faceCount; i++) {
        delete[] faceList[i].vertexList;
    }
    if (faceList != NULL) delete[] faceList;
    // Set pointers to array
    vertexList  = NULL;
    faceList    = NULL;
    faceCount   = 0;
    vertexCount = 0;

    // Call our function again to load new vertex and face information.
    loadGeometry();
}

/*  ===============================================
Desc: You get to implement this
Precondition:
Postcondition:
=============================================== */
void ply::loadGeometry() {

    /*
    1. Parse the header
    2.) Update any private or helper variables in the ply.h private section
    3.) allocate memory for the vertexList
    3a.) Populate vertices
    4.) allocate memory for the faceList
    4a.) Populate faceList
    */


    ifstream myfile(filePath.c_str()); // load the file
    if (myfile.is_open()) {            // if the file is accessable
        properties = -2; // set the properties because there are extras labeled

        string line;
        char  *token_pointer;
        char  *lineCopy = new char[256];
        int    count;
        bool   reading_header = true;
        // loop for reading the header
        while (reading_header && getline(myfile, line)) {

            // get the first token in the line, this will determine which
            // action to take.
            strcpy(lineCopy, line.c_str());
            token_pointer = strtok(lineCopy, " \r");
            // case when the element label is spotted:
            if (strcmp(token_pointer, "element") == 0) {
                token_pointer = strtok(NULL, " \r");

                // When the vertex token is spotted read in the next token
                // and use it to set the vertexCount and initialize vertexList
                if (strcmp(token_pointer, "vertex") == 0) {
                    token_pointer = strtok(NULL, "  \r");
                    vertexCount   = atoi(token_pointer);
                    vertexList    = new vertex[vertexCount];
                }

                // When the face label is spotted read in the next token and
                // use it to set the faceCount and initialize faceList.
                if (strcmp(token_pointer, "face") == 0) {
                    token_pointer = strtok(NULL, "  \r");
                    faceCount     = atoi(token_pointer);
                    faceList      = new face[faceCount];
                }
            }
            // if property label increment the number of properties.
            if (strcmp(token_pointer, "property") == 0) { properties++; }
            // if end_header break the header loop and move to reading vertices.
            if (strcmp(token_pointer, "end_header") == 0) {
                reading_header = false;
            }
        }

        // Read in exactly vertexCount number of lines after reading the header
        // and set the appropriate vertex in the vertexList.
        for (int i = 0; i < vertexCount; i++) {

            getline(myfile, line);
            strcpy(lineCopy, line.c_str());

            // by convention the first three are x, y, z and we ignore the rest
            if (properties >= 0) {
                vertexList[i].x = atof(strtok(lineCopy, " \r"));
            }
            if (properties >= 1) {
                vertexList[i].y = atof(strtok(NULL, " \r"));
            }
            if (properties >= 2) {
                vertexList[i].z = atof(strtok(NULL, " \r"));
            }
        }

        // Read in the faces (exactly faceCount number of lines) and set the
        // appropriate face in the faceList
        for (int i = 0; i < faceCount; i++) {

            getline(myfile, line);

            strcpy(lineCopy, line.c_str());
            count                   = atoi(strtok(lineCopy, " \r"));
            faceList[i].vertexCount = count; // number of vertices stored
            faceList[i].vertexList  = new int[count]; // initialize the vertices

            // set the vertices from the input, reading only the number of
            // vertices that are specified
            for (int j = 0; j < count; j++) {
                faceList[i].vertexList[j] = atoi(strtok(NULL, " \r"));
            }
        }
        delete[] lineCopy;
        myfile.close();
        scaleAndCenter();
    }
    // if the path is invalid, report then exit.
    else {
        cout << "cannot open file " << filePath.c_str() << "\n";
    }
};

/*  ===============================================
Desc: Moves all the geometry so that the object is centered at 0, 0, 0 and
scaled to be between 0.5 and -0.5 Precondition: after all the vetices and faces
have been loaded in Postcondition:
=============================================== */
void ply::scaleAndCenter() {
    int   i;
    float max, new_max = 0.0f;

    /* STEP 1: center at origin 
       The idea is to first find the average (center) of the object, then 
       shift each vertex towards the origin by this distance. */
    float x_avg, y_avg, z_avg, x_sum, y_sum, z_sum; 
    x_sum = y_sum = z_sum = 0.0;

    for (i = 0; i < vertexCount; i++) {
        x_sum += vertexList[i].x;
        y_sum += vertexList[i].y;
        z_sum += vertexList[i].z;
    }
    x_avg = x_sum / vertexCount;
    y_avg = y_sum / vertexCount;
    z_avg = z_sum / vertexCount;

    /* Restore the offset by shifting every vertex towards the origin */
    for (i = 0; i < vertexCount; i++) {
        vertexList[i].x -= x_avg;
        vertexList[i].y -= y_avg;
        vertexList[i].z -= z_avg;
    }

    /* STEP 2: scale everything to fit inside -0.5 to 0.5 */
    // sanity check. The printf should say that the max is 0.5
    for (i = 0; i < vertexCount; i++) {
        if (max < fabs(vertexList[i].x)) max = fabs(vertexList[i].x);
        if (max < fabs(vertexList[i].y)) max = fabs(vertexList[i].y);
        if (max < fabs(vertexList[i].z)) max = fabs(vertexList[i].z);
    }
    printf("max vertex value BEFORE scaling: %f\n", max);

    float scaling_factor = max / 0.5;
    for (i = 0; i < vertexCount; i++) {
        vertexList[i].x /= scaling_factor;
        vertexList[i].y /= scaling_factor;
        vertexList[i].z /= scaling_factor;
    }

    for (i = 0; i < vertexCount; i++) {
        if (new_max < fabs(vertexList[i].x)) new_max = fabs(vertexList[i].x);
        if (new_max < fabs(vertexList[i].y)) new_max = fabs(vertexList[i].y);
        if (new_max < fabs(vertexList[i].z)) new_max = fabs(vertexList[i].z);
    }
    printf("max vertex value BEFORE scaling: %f\n", new_max);

}

/*  ===============================================
Desc: Draws a filled 3D object
Precondition:
Postcondition:
Error Condition: If we haven't allocated memory for our
faceList or vertexList then do not attempt to render.
=============================================== */
void ply::render() {
    if (vertexList == NULL || faceList == NULL) { return; }

    for (int i = 0; i < faceCount; i++) { /* loop through each face */ 
        float v[9]; /* stores 3 vertices */
        for (int j = 0; j < faceList[i].vertexCount; j++) {  
                int idx = faceList[i].vertexList[j];
                v[j * 3 + 0] = vertexList[idx].x;
                v[j * 3 + 1] = vertexList[idx].y;
                v[j * 3 + 2] = vertexList[idx].z;
        }
        glBegin(GL_TRIANGLES);
        setNormal(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]); 
        glVertex3f(v[0], v[1], v[2]);
        glVertex3f(v[3], v[4], v[5]);
        glVertex3f(v[6], v[7], v[8]);
        glEnd();
    }

    //----- The code below uses a 2D array to store vertices -----//
    /* This was our initial attempt, using a 2D array to store vertices for
       readability. We then changed it to the 1D array above for concision */
    // for (int i = 0; i < faceCount; i++) { // loop through each face
    //     float v[3][3]; /* stores 3 vertices */
    //     for (int j = 0; j < faceList[i].vertexCount; j++) { 
    //             int idx = faceList[i].vertexList[j];
    //             v[j][0] = vertexList[idx].x;
    //             v[j][1] = vertexList[idx].y;
    //             v[j][2] = vertexList[idx].z;
    //     }
    //     glBegin(GL_TRIANGLES);
    //     setNormal(v[0][0], v[0][1], v[0][2], v[1][0], v[1][1], 
    //               v[1][2], v[2][0], v[2][1], v[2][2]); 
    //     glVertex3f(v[0][0], v[0][1], v[0][2]);
    //     glVertex3f(v[1][0], v[1][1], v[1][2]);
    //     glVertex3f(v[2][0], v[2][1], v[2][2]);
    //     glEnd();
    // }
}

/*  ===============================================
Desc: Prints some statistics about the file you have read in
This is useful for debugging information to see if we parse our file correctly.

Precondition:
Postcondition:
=============================================== */
void ply::printAttributes() {
    cout << "==== ply Mesh Attributes=====" << endl;
    cout << "vertex count:" << vertexCount << endl;
    cout << "face count:" << faceCount << endl;
    cout << "properties:" << properties << endl;
}

/*  ===============================================
Desc: Iterate through our array and print out each vertex.

Precondition:
Postcondition:
=============================================== */
void ply::printVertexList() {
    if (vertexList == NULL) {
        return;
    } else {
        for (int i = 0; i < vertexCount; i++) {
            cout << vertexList[i].x << "," << vertexList[i].y << ","
                 << vertexList[i].z << endl;
        }
    }
}

/*  ===============================================
Desc: Iterate through our array and print out each face.

Precondition: 
Postcondition:
=============================================== */
void ply::printFaceList() {
    if (faceList == NULL) {
        return;
    } else {
        // For each of our faces
        for (int i = 0; i < faceCount; i++) {
            // Get the vertices that make up each face from the face list
            for (int j = 0; j < faceList[i].vertexCount; j++) {
                // Print out the vertex
                int index = faceList[i].vertexList[j];
                cout << vertexList[index].x << "," << vertexList[index].y << ","
                     << vertexList[index].z << endl;
            }
        }
    }
}

void ply::setNormal(float x1, float y1, float z1, float x2, float y2, float z2,
                    float x3, float y3, float z3) {

    float v1x, v1y, v1z;
    float v2x, v2y, v2z;
    float cx, cy, cz;

    // find vector between x2 and x1
    v1x = x1 - x2;
    v1y = y1 - y2;
    v1z = z1 - z2;

    // find vector between x3 and x2
    v2x = x2 - x3;
    v2y = y2 - y3;
    v2z = z2 - z3;

    // cross product v1xv2
    cx = v1y * v2z - v1z * v2y;
    cy = v1z * v2x - v1x * v2z;
    cz = v1x * v2y - v1y * v2x;

    // normalize
    float length = sqrt(cx * cx + cy * cy + cz * cz);
    cx           = cx / length;
    cy           = cy / length;
    cz           = cz / length;

    glNormal3f(cx, cy, cz);
}
