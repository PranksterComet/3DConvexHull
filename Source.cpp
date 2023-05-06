#include <windows.h>  // for MS Windows
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glut.h>  // GLUT, include glu.h and gl.h
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <cstdlib>
#include <map>
#include <time.h>

using namespace std;

#define PI 3.14159265359

/*
Problemos
Inactive edges Appearing

*/
/* Global variables */
char title[] = "3D Shapes";
float xrot = 0, yrot = 0, zrot = 0, angSpeed = 0.1, ztr = 0, sc = 5;
bool comp = false;
int sz = 2000;
vector<vector<int>> points, faces, act, pedges;
vector<vector<vector<int>>> edges(sz, vector<vector<int>>(sz, vector<int>(2, 0)));
vector<vector<int>> test = { };
//{ {0, 0, 0}, {10, 0, 0}, {0, 10, 0}, {0, 0, -10}, {5, 2, -1} };

void printVec(vector<int> v) {
    for (auto x : v) {
        cout << x << " ";
    }
    cout << '\n';
}

void print2d(vector<vector<int>> v) {
    for (auto x : v) {
        printVec(x);
    }
}

int Sign(vector<int>p0, vector<int>p1, vector<int>p2, vector<int> P) {
    for (int j = 0; j < 3; j++) {
        p1[j] -= p0[j];
        p2[j] -= p0[j];
        P[j] -= p0[j];
    }

    //Overflow Chance!!
    int ans = P[0] * (p1[1] * p2[2] - p2[1] * p1[2]) - P[1] * (p1[0] * p2[2] - p2[0] * p1[2]) + P[2] * (p1[0] * p2[1] - p1[1] * p2[0]);
    if (ans == 0) return ans;
    
    return ans / abs(ans);
}


vector<vector<int>> IncHull(vector<vector<int>> p /*points*/) {
    

    print2d(p);

    faces = { {0, 1, 2}, {0, 3, 1}, {0, 2, 3}, {1, 3, 2} };
    
    int n = p.size();
    
    for (int i = 0; i < faces.size(); i++) {
        vector<int> f = faces[i];
        for (int j = 0; j < 3; j++) {
            int u = j;
            int v = (j + 1) % 3;
            act.push_back({ f[u], f[v] });
            edges[f[u]][f[v]][0] = i; // face index 
            edges[f[u]][f[v]][1] = 0; // face not visible
        }
    }
    cout << "Active Edges Of Initial Tet: " << '\n';
    print2d(act);

    for (int i = 4; i < n; i++) {
        cout << "Current Point";
        printVec(p[i]);

        for (auto v : act) {
            int f = edges[v[0]][v[1]][0];
            vector<int> fc = faces[f];
            if (Sign(p[fc[0]], p[fc[1]], p[fc[2]], p[i]) == 1) {
                edges[v[0]][v[1]][1] = 1;
            }

            else {
                edges[v[0]][v[1]][1] = 0;
                //cout << "Invisible: ";
                //cout << f << '\n';
            }
        }

        
        vector<vector<int>> nxt;

        for (auto v : act) {
            vector<int> inf = edges[v[0]][v[1]];
            vector<int> inv = edges[v[1]][v[0]];
            if (inf[1] == 0) {
                nxt.push_back(v);
            }

            else {
                if (inv[1] == 0) {
                    vector<int> fc = { v[0], v[1], i };
                    faces.push_back(fc);
                    for (int j = 0; j < 3; j++) {
                        int a = j;
                        int b = (j + 1) % 3;
                        nxt.push_back({ fc[a], fc[b] });
                        edges[fc[a]][fc[b]][0] = faces.size() - 1;
                    }
                }
            }
        }

        act = nxt;
    }

    return act;
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        angSpeed += 0.1;
        if (!comp) {
            pedges = IncHull(test);

            cout << pedges.size() << '\n';
            for (auto v : pedges) {
                cout << v[0] << " " << v[1] << '\n';
            }
            cout << test.size() << '\n';
            comp = true;
        }
        
    }

    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        angSpeed -= 0.1;
    }
                                                            
    glutPostRedisplay();
}

void keyPressed(unsigned char key, int x, int y) {
    if (key == 'z') {
        ztr += angSpeed;
        //cout << ztr << '\n';
    }

    if (key == 'x') {
        ztr -= angSpeed;
    }

    if (key == 'a') {
        yrot -= angSpeed;
    }

    if (key == 'd') {
        yrot += angSpeed;
    }
    
    if (key == 's') {
        xrot += angSpeed;
    }

    if (key == 'w') {
        xrot -= angSpeed;
    }

    if (key == 'q') {
        zrot -= angSpeed;
    }

    if (key == 'e') {
        zrot += angSpeed;
    }

    if (key == 'r') {
        xrot = 0, yrot = 0, zrot = 0, ztr = 0;
    }
    
    glutPostRedisplay();
}

/* Initialize OpenGL Graphics */
void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

/* Handler for window-repaint event. Called back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix

    // Render a color-cube consisting of 6 quads with different colors
    glLoadIdentity();                 // Reset the model-view matrix
    
    glTranslatef(1.5f, 0.0f, -7.0f);  // Move right and into the screen
    glRotatef(yrot * 10, 0.0f, 1.0f, 0.0f);
    glRotatef(xrot * 10, 1.0f, 0.0f, 0.0f);
    glRotatef(zrot * 10, 0.0f, 0.0f, 1.0f);


    glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
    // Top face (y = 1.0f)
    // Define vertices in counter-clockwise (CCW) order with normal pointing out
    /*
    glColor3f(0.0f, 1.0f, 0.0f);     // Green
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);

    // Bottom face (y = -1.0f)
    glColor3f(1.0f, 0.5f, 0.0f);     // Orange
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Front face  (z = 1.0f)
    glColor3f(1.0f, 0.0f, 0.0f);     // Red
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    // Back face (z = -1.0f)
    glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);

    // Left face (x = -1.0f)
    glColor3f(0.0f, 0.0f, 1.0f);     // Blue
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    // Right face (x = 1.0f)
    glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    */
    glEnd();  // End of drawing color-cube

    // Render a pyramid consists of 4 triangles
    glLoadIdentity();                  // Reset the model-view matrix
    
    glTranslatef(0.0f, 0.0f, -6.0f + ztr);  // Move right and into the screen
    glRotatef(yrot * 10, 0.0f, 1.0f, 0.0f);
    glRotatef(xrot * 10, 1.0f, 0.0f, 0.0f);
    glRotatef(zrot * 10, 0.0f, 0.0f, 1.0f);

    glColor3f(1.0, 0.0, 0.0);
    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);
    /*
    glVertex3f(0.5, 0, 0);
    glVertex3f(0.5, 0, -0.5);
    glVertex3f(2.0, 0, -0.5);
    */

    /*
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; j < 2; k++) {
                glVertex3f(float(i)/10, float(j) / 10, -1*float(k) / 10);
            }
        }
    }
    */

    for (auto p : test) {
        glVertex3f(float(p[0])/sc, float(p[1])/sc, float(p[2])/sc);
    }

    glEnd();

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -6.0f + ztr);  // Move right and into the screen
    glRotatef(yrot * 10, 0.0f, 1.0f, 0.0f);
    glRotatef(xrot * 10, 1.0f, 0.0f, 0.0f);
    glRotatef(zrot * 10, 0.0f, 0.0f, 1.0f);
    // Move left and into the screen

    glBegin(GL_TRIANGLES);  // Begin drawing the pyramid with 4 triangles
    int c = 0;
    
    if (comp) {
        for (auto v : pedges) {
            glColor3f((c/4)%2, (c/2)%2, max(c%2, 0.5));
            vector<int> fc = faces[edges[v[0]][v[1]][0]];
            for (int j = 0; j < 3; j++) {
                //cout << float(test[fc[j]][0]) / sc << " " << float(test[fc[j]][1]) / sc << " " << float(test[fc[j]][2]) / sc <<'\n';
                glVertex3f(float(test[fc[j]][0]) / sc, float(test[fc[j]][1]) / sc, float(test[fc[j]][2]) / sc);
            }
            c++;
        }
    }
    /*
    // Front
    glColor3f(1.0f, 0.0f, 0.0f);     // Red
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);     // Green
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 1.0f);     // Blue
    glVertex3f(1.0f, -1.0f, 1.0f);

    // Right
    glColor3f(1.0f, 0.0f, 0.0f);     // Red
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);     // Blue
    glVertex3f(1.0f, -1.0f, 1.0f);
    glColor3f(0.0f, 1.0f, 0.0f);     // Green
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Back
    glColor3f(1.0f, 0.0f, 0.0f);     // Red
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);     // Green
    glVertex3f(1.0f, -1.0f, -1.0f);
    glColor3f(0.0f, 0.0f, 1.0f);     // Blue
    glVertex3f(-1.0f, -1.0f, -1.0f);

    // Left
    glColor3f(1.0f, 0.0f, 0.0f);       // Red
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);       // Blue
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glColor3f(0.0f, 1.0f, 0.0f);       // Green
    glVertex3f(-1.0f, -1.0f, 1.0f);
    */
    glEnd();   // Done drawing the pyramid

    glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
    glutInit(&argc, argv);            // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
    glutInitWindowSize(640, 480);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutCreateWindow(title);          // Create window with the given title

    //cout << Sign({0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, -1}) << '\n';
    

    
    int num = 20;
    float R = 10, z = 10, r = 10;

    /*
    while (z >= 0) {
        r = sqrtf(float(pow(R, 2) - pow(z, 2)));
        while (r > 0) {
            for (int j = 0; j < num; j++) {
                float ang = float(j) * (2 * PI / float(num));
                test.push_back({ int(r * cos(ang)), int(r * sin(ang)) , int(R - z) });
            }
            r--;
        }
        z--;
    }
    */
    
    // Circle
    
    /*
    for (int j = 0; j < num; j++) {
        double ang = double(j) * (2 * PI / double(num));
        test.push_back({ int(r * cos(ang)), int(r * sin(ang)) , 0 });
        test.push_back({ int(r + 2), int(r * sin(ang)) , int(r * cos(ang)) });
    }
    */
    
    
    int tot;
    cout << "Total Points: " << '\n';
    cin >> tot;
    srand(time(0));
    int lim = 15;
    for (int i = 0; i < tot; i++) {
        int x = (rand() % lim)*(2*(rand()%2) - 1);
        int y = (rand() % lim)*(2*(rand()%2) - 1);
        int z = (rand() % lim)*(2*(rand()%2) - 1);

        test.push_back({ x, y, z });
    }
    
    random_shuffle(test.begin(), test.end());

    while (Sign(test[1], test[2], test[3], test[0]) == 0) {
        random_shuffle(test.begin(), test.end());
    }

    if (Sign(test[1], test[2], test[3], test[0]) == -1) {
        swap(test[0], test[1]);
    }

    cout << "Num Points: ";
    cout << test.size() << '\n';



    glutMouseFunc(mouse);           //Camera Movement
    glutKeyboardFunc(keyPressed);
    glutDisplayFunc(display);       // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);       // Register callback handler for window re-size event
    initGL();                       // Our own OpenGL initialization
    glutMainLoop();                 // Enter the infinite event-processing loop
    
    
    
    return 0;
}