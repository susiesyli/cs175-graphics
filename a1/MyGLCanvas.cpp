#include "MyGLCanvas.h"

int Shape::m_segmentsX;
int Shape::m_segmentsY;

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) {
    mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

    rotVec      = glm::vec3(0.0f, 0.0f, 0.0f);
    eyePosition = glm::vec3(0.0f, 0.0f, 3.0f);

    wireframe = 0;
    fill      = 1;
    normal    = 0;
    smooth    = 0;
    segmentsX = segmentsY = 10;
    scale                 = 1.0f;

    objType  = SHAPE_CUBE;
    cube     = new Cube();
    cylinder = new Cylinder();
    cone     = new Cone();
    sphere   = new Sphere();
    // torus = new Torus();
    shape = cube;

    shape->setSegments(segmentsX, segmentsY);
}

MyGLCanvas::~MyGLCanvas() {
    delete cube;
    delete cylinder;
    delete cone;
    delete sphere;
    // delete torus;
}

void MyGLCanvas::setShape(OBJ_TYPE type) {
    objType = type;
    switch (type) {
    case SHAPE_CUBE: shape = cube; break;
    case SHAPE_CYLINDER: shape = cylinder; break;
    case SHAPE_CONE: shape = cone; break;
    case SHAPE_SPHERE: shape = sphere; break;
    case SHAPE_SPECIAL1:
        // shape = torus;
        break;
    default: shape = cube;
    }

    printf("set shape to: %d\n", type);
}

void MyGLCanvas::setSegments() {
    shape->setSegments(segmentsX, segmentsY);
}

void MyGLCanvas::draw() {
    if (!valid()) { // this is called when the GL canvas is set up for the first time or when it is
                    // resized...
        printf("establishing GL context\n");

        glViewport(0, 0, w(), h());
        updateCamera(w(), h());

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        GLfloat light_pos0[] = {eyePosition.x, eyePosition.y, eyePosition.z, 0.0f};
        GLfloat ambient[]    = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat diffuse[]    = {0.9f, 0.9f, 0.9f, 1.0f};

        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);

        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        /**********************************************/
        /*    Enable normalizing normal vectors       */
        /*    (e.g. normals not affected by glScalef) */
        /**********************************************/

        glEnable(GL_NORMALIZE);

        /****************************************/
        /*          Enable z-buferring          */
        /****************************************/

        glEnable(GL_DEPTH_TEST);
        glPolygonOffset(1, 1);
        glFrontFace(GL_CCW); // make sure that the ordering is counter-clock wise
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (smooth) {
        glShadeModel(GL_SMOOTH);
    } else {
        glShadeModel(GL_FLAT);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // rotate object
    glRotatef(rotVec.x, 1.0, 0.0, 0.0);
    glRotatef(rotVec.y, 0.0, 1.0, 0.0);
    glRotatef(rotVec.z, 0.0, 0.0, 1.0);

    glScalef(scale, scale, scale);

    drawAxis();
    drawScene();
}

void MyGLCanvas::drawScene() {
    glPushMatrix();

    if (normal) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0, 0.0, 0.0);
        shape->drawNormal();
        glEnable(GL_LIGHTING);
    }

    if (wireframe) {
        glDisable(GL_LIGHTING);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glColor3f(1.0f, 1.0f, 0.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shape->draw();
        glEnable(GL_LIGHTING);
    }

    if (fill) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glColor3f(0.5, 0.5, 0.5);
        glPolygonMode(GL_FRONT, GL_FILL);
        shape->draw();
    }

    glPopMatrix();
}

int MyGLCanvas::handle(int e) {
    // printf("Event was %s (%d)\n", fl_eventnames[e], e);
    switch (e) {
    case FL_KEYUP: printf("keyboard event: key pressed: %c\n", Fl::event_key()); break;
    case FL_MOUSEWHEEL: break;
    }

    return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
    Fl_Gl_Window::resize(x, y, w, h);
    puts("resize called");
}

void MyGLCanvas::drawAxis() {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(1.0, 0, 0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0.0, 1.0, 0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1.0);
    glEnd();
    glEnable(GL_LIGHTING);
}

void MyGLCanvas::updateCamera(int width, int height) {
    float xy_aspect;
    xy_aspect = (float)width / (float)height;
    // Determine if we are modifying the camera(GL_PROJECITON) matrix(which is our viewing volume)
    // Otherwise we could modify the object transormations in our world with GL_MODELVIEW
    glMatrixMode(GL_PROJECTION);
    // Reset the Projection matrix to an identity matrix
    glLoadIdentity();
    gluPerspective(45.0f, xy_aspect, 0.1f, 10.0f);
    gluLookAt(eyePosition.x, eyePosition.y, eyePosition.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}
