
//////////////////////////////////////////////////////////////////////////////
/////////////// Demo: Using PNG images as textures in openGL /////////////////
//////////////////////////////////////////////////////////////////////////////

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/glext.h>
#else
#include <GL/glut.h>
#include <GL/glext.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <vector>
#include "lodepng.h"

using namespace std;

int WIDTH=640; // window width & height
int HEIGHT=480;
vector <unsigned char> image; // storage for image(pixel array)
unsigned imageWidth;  // image width and height
unsigned imageHeight;
GLuint texname;
float scale=1.0;
/** Sets current texture to given image
    @param img is image vector that has already been loaded
    @param width is width of the image
    @param height is height of image
*/
void setTexture(vector<unsigned char> img, unsigned width, unsigned height)
{
    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_2D, texname);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // without this texture darkens
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, &img[0]);
}
/** OpenGL seems to draw images vertically flipped
        this function inverts our data so that it displays correctly
        @param img is our image data vector
        @param width is our image width
        @param height is our image height
*/
void invert(vector<unsigned char> &img,const unsigned width,const unsigned height)
{
    unsigned char *imageptr = &img[0];
    unsigned char *first = NULL;
    unsigned char *last = NULL;
    unsigned char temp = 0;
    for( int h = 0; h <(int) height/2; ++h )
    {

        first = imageptr + h * width * 4;
        last = imageptr + (height - h - 1) * width*4;
        for( int i = 0; i < (int)width*4; ++i )
        {
            temp = *first;
            *first = *last;
            *last = temp;
            ++first;
            ++last;
        }
    }
}
/**
Load image into memory
*/
void loadImage(const char* name)
{
    //use lodepng decode to decode image
    int error;
    if((error=lodepng::decode(image,imageWidth,imageHeight,name)))
    {
        cout<<name<<":"<<lodepng_error_text(error)<<endl;
        exit(1);
    }
    else
        invert(image,imageWidth,imageHeight);

}
static void resize(int width, int height)
{
    WIDTH=width;
    HEIGHT=height;
    glClearColor(0,0,0,0);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,width,0,height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}
static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    setTexture(image,imageWidth,imageHeight);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-(imageWidth*scale/2.0),HEIGHT/2.0-(imageHeight*scale/2.0),0);
    glScalef(scale,scale,1);
    glBegin(GL_POLYGON);
        glTexCoord2d(0,0);  glVertex2f(0,0);
        glTexCoord2d(0,1);  glVertex2f(0,imageHeight);
        glTexCoord2d(1,1);  glVertex2f(imageWidth,imageHeight);
        glTexCoord2d(1,0);  glVertex2f(imageWidth,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}
static void key(unsigned char key,int x,int y)
{
    switch(key)
    {
    case 27:
        exit(0);
        break;
    case '+':
        scale+=.1;
        break;
    case '-':
        scale-=.1;
        break;
    }
}
static void idle(void)
{
    glutPostRedisplay();
}
int main(int argc, char *argv[])
{
    //load image to memory
    if(argc<2)
    {
     cout<<"usage:"<<argv[0]<<" <image path>";
     return 1;
    }
    loadImage(argv[1]);
    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH,HEIGHT);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
    glutCreateWindow("pngViewer");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    glutMainLoop();
    return EXIT_SUCCESS;
}
