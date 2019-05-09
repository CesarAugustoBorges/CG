#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <tinyxml2.h>
#include <math.h>
#include <IL/il.h>
#include "Ponto.h"
#include "Scene.h"
#include "EngineException.h"

using namespace std;
using namespace tinyxml2;

Scene scene;
XMLDocument doc;

//Frames per second Variables
int timebase = 0, frame = 0;

//Camera Variables
#define K 0.03f
float camX = 0, camY=0, camZ = 50;
float lX=0, lY=0, lZ=0;
float upX=0, upY=1, upZ=0;
float alpha = 3.1415, beta = 0, radius = 50;
GLenum OPTION = GL_FILL;
int mode=0;


std::string getAttributeOrDefault(XMLElement *element, const char* atr, std::string defaultValue){
    const XMLAttribute *atrXml = element->FindAttribute(atr);
    return atrXml == nullptr ? defaultValue : atrXml->Value();
}

float getAttributeOrDefault(XMLElement *element, const char* atr, float defaultValue){
    const XMLAttribute *atrXml = element->FindAttribute(atr);
    return atrXml == nullptr ? defaultValue : (float) atof((char*)atrXml->Value());
}

float getAttributeOrDefault(XMLElement *element, std::string atr, float defaultValue){
    return getAttributeOrDefault(element, atr.c_str(), defaultValue);
}

char* getAttributeOrDefault3(XMLElement *element, const char* atr, char* defaultValue){
    const XMLAttribute *atrXml = element->FindAttribute(atr);
    return atrXml == nullptr ? defaultValue : (char*)atrXml->Value();
}

void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(h == 0)
        h = 1;

    // compute window's aspect ratio
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void drawAxes(){
    glBegin(GL_LINES);

    glColor3f(1,0,0);
    glVertex3f(1000,0,0);
    glVertex3f(-1000,0,0);

    glColor3f(0,1,0);
    glVertex3f(0,1000,0);
    glVertex3f(0,-1000,0);

    glColor3f(0,0,1);
    glVertex3f(0,0,1000);
    glVertex3f(0,0,-1000);

    glEnd();

    glColor3f(1,1,1);
}

/*
 * Desenha as figuras dentro da estrutura da scene
 */
void draw(){
   // drawAxes();

    vector<Group> groups = scene.getGroups();
    for(int i = 0; i < groups.size(); i++){
        Group g = groups[i];
        g.draw();
    }
}

void viewFramesPerSecond(){
    float fps;
    char s[64];

    frame++;
    int time=glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        fps = frame*1000.0/(time-timebase);
        timebase = time;
        frame = 0;
        sprintf(s, "FPS: %f6.2", fps);
        glutSetWindowTitle(s);
    }
}

void renderScene(void) {
        // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, OPTION);

    // set the camera
    glLoadIdentity();
    gluLookAt(camX,camY,camZ,
              lX,lY,lZ,
              upX,upY,upZ);

    drawAxes();
    draw();
    viewFramesPerSecond();

    // End of frame
    glutSwapBuffers();
}

void sphericalToCartesian(){
    if(mode==1){
        camX = lX + (radius * cos(beta) * sin(alpha));
        camY = lY + (radius * sin(beta));
        camZ = lZ + (radius * cos(beta) * cos(alpha));
    }
    if(mode==0){
        lX = camX + (radius * cos(beta) * sin(alpha));
        lY = camY + (radius * sin(beta));
        lZ = camZ + (radius * cos(beta) * cos(alpha));
    }
}

void processSpecialKeys(int key, int xx, int yy) {
    int modeMult=-1;
    if(mode==1){modeMult=1;}
    switch (key) {
        case GLUT_KEY_RIGHT:
            alpha += modeMult*0.1;
            sphericalToCartesian();
            break;
        case GLUT_KEY_LEFT:
            alpha -= modeMult*0.1;
            sphericalToCartesian();
            break;
        case GLUT_KEY_UP:
            beta += 0.1f;
            if (beta >= 1.57f){beta = 1.57f;}
            sphericalToCartesian();
            break;
        case GLUT_KEY_DOWN:
            beta -= 0.1f;
            if (beta <= -1.57f){beta = -1.57f;}
            sphericalToCartesian();
            break;
        default:
            break;
    }

    radius = sqrt(pow((lX-camX),2) + pow((lY-camY),2) + pow((lZ-camZ),2));

    glutPostRedisplay();
}

void processKeys(unsigned char key, int xx, int yy) {
    float dX = lX - camX;
    float dY = lY - camY;
    float dZ = lZ - camZ;
    float rX,rY,rZ;

    switch(key){
        case 'w':
            camX = camX + K*dX;
            camY = camY + K*dY;
            camZ = camZ + K*dZ;
            lX = lX + K*dX;
            lY = lY + K*dY;
            lZ = lZ + K*dZ;
            break;
        case 's':
            camX = camX - K*dX;
            camY = camY - K*dY;
            camZ = camZ - K*dZ;
            lX = lX - K*dX;
            lY = lY - K*dY;
            lZ = lZ - K*dZ;
            break;
        case 'a':
            rX = -dZ;
            rY = 0;
            rZ = dX;
            camX = camX - K*rX;
            camY = camY - K*rY;
            camZ = camZ - K*rZ;
            lX = lX - K*rX;
            lY = lY - K*rY;
            lZ = lZ - K*rZ;
            break;
        case 'd':
            rX = -dZ;
            rY = 0;
            rZ = dX;
            camX = camX + K*rX;
            camY = camY + K*rY;
            camZ = camZ + K*rZ;
            lX = lX + K*rX;
            lY = lY + K*rY;
            lZ = lZ + K*rZ;
            break;
        case 'i':
            camY += 0.5f;
            lY += 0.5f;
            break;
        case 'j':
            camY -= 0.5f;
            lY -= 0.5f;
            break;
        case 'm':
            if(mode==0){mode=1;}
            else{mode=0;}
            alpha+=3.1415;
            beta = 0 - beta;
            break;
        case '1':
            OPTION = GL_FILL;
            break;
        case '2':
            OPTION = GL_LINE;
            break;
        case '3':
            OPTION = GL_POINT;
            break;
        default:
            break;
    }

    glutPostRedisplay();
}

Model loadModel(XMLElement *model){
    Model m = Model((char*) model->FindAttribute("file")->Value());
    std::string *texturePath = new std::string(getAttributeOrDefault(model, "texture", ""));
    m.setTexture(texturePath);
    float red,green,blue;
    std::string colors[] = {"", "diff", "spec", "emiss", "ambi"};
    for(int i = 0; i < 1; i++){
        red = getAttributeOrDefault(model, colors[i] + "R", -1);
        green = getAttributeOrDefault(model, colors[i] + "G", -1);
        blue = getAttributeOrDefault(model, colors[i] + "B", -1);

        if(red >= 0 && green >= 0 && blue >= 0)
            switch(i){
                case 0: m.setColour(red/255, green/255, blue/255); break;
                case 1: m.setDiffuseColor(red/255, green/255, blue/255); break;
                case 2: m.setSpecularColor(red/255, green/255, blue/255); break;
                case 3: m.setEmissiveColor(red/255, green/255, blue/255); break;
                case 4: m.setAmbientColor(red/255, green/255, blue/255); break;
                default: break;
            }
        else if(red >= 0 || green >= 0 || blue >= 0) {
            throw EngineException("R, G and B attributes are all required to define color: " + colors[i]);
        }
    }
    return m;
}

void loadTranslation(XMLElement *trans, Group *group){
    if(trans->FindAttribute("time") != nullptr){
        auto *t = new TranslateCatMull();
        t->setTransTime(getAttributeOrDefault(trans, "time", 0));
        XMLElement *child = trans->FirstChildElement();
        while(child){
            if(strcmp(child->Value(), "point") == 0){
                float transX, transY, transZ;
                transX = getAttributeOrDefault(child, "X", 0);
                transY = getAttributeOrDefault(child, "Y", 0);
                transZ = getAttributeOrDefault(child, "Z", 0);
                t->addPointToTranslation(transX, transY, transZ);
            } else {
                throw EngineException(string("There is no default tag in translation \"") + child->Value() + "\"");
            }
            child = child->NextSiblingElement();
        }
        group->setTranslate(t);
    } else {
        float transX, transY, transZ;
        transX = getAttributeOrDefault(trans, "X", 0);
        transY = getAttributeOrDefault(trans, "Y", 0);
        transZ = getAttributeOrDefault(trans, "Z", 0);
        auto *t = new TranslateDefault(transX, transY, transZ);
        group->setTranslate(t);
    }
}

void loadRotate(XMLElement *rotate, Group *group){
    float ang, axisX, axisY, axisZ;
    axisX = getAttributeOrDefault(rotate, "axisX", 0);
    axisY = getAttributeOrDefault(rotate, "axisY", 0);
    axisZ = getAttributeOrDefault(rotate, "axisZ", 0);
    if(rotate->FindAttribute("time") != nullptr){
        ang = getAttributeOrDefault(rotate, "time", 0);
        RotateWithTime *r = new RotateWithTime(ang, axisX, axisY, axisZ);
        group->setRotate(r);
    } else {
        ang = getAttributeOrDefault(rotate, "ang", 0);
        RotateDefault *r = new RotateDefault(ang, axisX, axisY, axisZ);
        group->setRotate(r);
    }
}

Group loadGroup(XMLElement *group){
    Group g;
    Group toAdd;
    XMLElement *child = group->FirstChildElement();
    while(child) {
        try {
            if (strcmp(child->Value(), "translate") == 0) {
                loadTranslation(child, &g);
            } else if (strcmp(child->Value(), "rotate") == 0) {
                loadRotate(child, &g);
            } else if (strcmp(child->Value(), "scale") == 0) {
                float scaleX, scaleY, scaleZ;
                scaleX = getAttributeOrDefault(child, "X", 1);
                scaleY = getAttributeOrDefault(child, "Y", 1);
                scaleZ = getAttributeOrDefault(child, "Z", 1);
                auto *s = new Scale(scaleX, scaleY, scaleZ);
                g.setScale(s);
            } else if (strcmp(child->Value(), "group") == 0) {
                toAdd = loadGroup(child);
                g.addGroup(&toAdd);
            } else if (strcmp(child->Value(), "models") == 0) {
                XMLElement *model = child->FirstChildElement("model");
                while (model) {
                    Model m = loadModel(model);
                    g.addModel(m);
                    model = model->NextSiblingElement("model");
                }
            } else {
                throw EngineException(string("There is no default tag \"") + child->Value() + "\"");
            }
        } catch (EngineException &e){
            printf("%s", e.what());
            exit(1);
        }
        child = child->NextSiblingElement();
    }
    return g;
}

void loadLights(){
    XMLElement *child;
    child = doc.FirstChildElement( "scene" )->FirstChildElement( "lights");
    if(child) child = child->FirstChildElement();
    while(child){
        short id = 0;
        if(strcmp(child->Value(), "light") == 0){
            string type = getAttributeOrDefault(child, "type", "");

            if(type == string("")) throw EngineException("type attribute is required in a light");

            if(type == string("POINT")){
                float posX, posY, posZ;
                posX = getAttributeOrDefault(child, "posX", 0);
                posY = getAttributeOrDefault(child, "posY", 0);
                posZ = getAttributeOrDefault(child, "posZ", 0);
                auto *l = new PointLight(id, posX, posY, posZ);
                scene.addLight(l);
            } else if(type == string("DIRECTIONAL")){
                float dirX, dirY, dirZ;
                dirX = getAttributeOrDefault(child, "dirX", 0);
                dirY = getAttributeOrDefault(child, "dirY", 0);
                dirZ = getAttributeOrDefault(child, "dirZ", 0);
                auto *l = new DiretionalLight(id, dirX, dirY, dirZ);
                scene.addLight(l);
            } else if (type == string("SPOT")){
                float posX, posY, posZ, dirX, dirY, dirZ;
                posX = getAttributeOrDefault(child, "posX", 0);
                posY = getAttributeOrDefault(child, "posY", 0);
                posZ = getAttributeOrDefault(child, "posZ", 0);
                dirX = getAttributeOrDefault(child, "dirX", 0);
                dirY = getAttributeOrDefault(child, "dirY", 0);
                dirZ = getAttributeOrDefault(child, "dirZ", 0);
                auto *l = new SpotLight(id, posX, posY, posZ, dirX, dirY, dirZ);
                scene.addLight(l);
            } else{
                throw EngineException(string("There is no default type: ") + type);
            }
        }
        else throw EngineException(string("tag") + child->Value() + string("not recognized in <lights> </lights>"));
    }
}

/*
 * Faz parse do ficheiro XML colocando a scene em memoria numa estrutura apropriada.
 */
void loadScene() {
    printf("Loading Scene\n");
    XMLElement *child;
    doc.LoadFile( "../scene3.xml" );

    child = doc.FirstChildElement( "scene" )->FirstChildElement( "group");
    while(child){
        Group group = loadGroup(child);
        scene.addGroup(group);
        child = child->NextSiblingElement( "group");
    }
    loadLights();
    printf("Finished loading Scene!!!\n");
}

int main(int argc, char** argv) {
    //init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(1000,800);
    glutCreateWindow("MyWindow");
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //Required callback registry
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    //Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

#ifndef __APPLE__
    glewInit();
#endif

    ilInit();

    loadScene();

    //OpenGL settings
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    //enter GLUT's main cycle
    glutMainLoop();

    return 0;
}