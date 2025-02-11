/*  =================== File Information =================
    File Name: ply.h
    Description:
    Author: Michael Shah

    Purpose:	Specification for using
    Examples:	See example below for using PLY class
    ===================================================== */
#ifndef PLY_H
#define PLY_H

#include "geometry.h"
#include <iostream>
#include <string>

/*  ============== ply ==============
    Purpose: Load a PLY File

    Note that the ply file inherits from a base class called 'entity'
    This class stores common transformations that can be applied to 3D
   entities(such as mesh files, lights, or cameras)

    Example usage:

    1.) ply* myPLY = new ply (filenamePath);
    2.) myPLY->render();
    3.) delete myPLY;

    ==================================== */
class ply {

public:
    ply();
    ply(std::string filePath);
    ~ply();

    /*	===============================================
        Desc: reloads the geometry for a 3D object
    =============================================== */
    void reload(std::string _filePath);

    /*	===============================================
        Desc: Draws a filled 3D object
    =============================================== */
    void render();

    /*	===============================================
        Desc: Prints some statistics about the file you have read in
    =============================================== */
    void printAttributes();

    /*  ===============================================
        Desc: Helper function for you to debug if
        you are reading in the correct data.
        (Generally these would not be public functions,
        they are here to help you understand the interface)
        =============================================== */
    void printVertexList();
    void printFaceList();

private:
    /*	===============================================
        Desc: Helper function used in the constructor
        =============================================== */
    void loadGeometry();
    void scaleAndCenter();
    void setNormal(float x1, float y1, float z1, float x2, float y2, float z2,
                   float x3, float y3, float z3);

    /*	===============================================
        Header Data

        These variables are useful to store information
        about the mesh we are loading.  Often these values
        are stored in the header, or can be useful for
        debugging.
        =============================================== */
    // Store the path to our file
    std::string filePath;
    // Stores the number of vertics loaded
    int vertexCount;
    // Stores the number of faces loaded
    int faceCount;
    // Tells us how many properites exist in the file
    int properties;
    // A dynamically allocated array that stores
    // a vertex
    vertex *vertexList;
    // A dynamically allocated array that stores
    // a list of faces (essentially integers that will
    // be looked up from the vertex list)
    face *faceList;
};

#endif