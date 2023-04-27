//--------------------------------------------------
// Author: Synarah Sitaf
// Date: 3/23/23
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include "plymesh.h"
#include "osutils.h"

using namespace std;
using namespace glm;
using namespace agl;

class MeshViewer : public Window {
public:
    MeshViewer() : Window() {
    }

    void setup() {
        file = GetFilenamesInDir("../models", "ply");
        for (int i = 0; i < file.size(); i++){
            PLYMesh curFile;
            curFile.load("../models/" + file[i]);
            m.push_back(curFile);
        }
        mesh = m[0];
        renderer.loadShader("normals", "../shaders/normals.vs", "../shaders/normals.fs");
        renderer.loadShader("phong-vertex", "../shaders/phong-vertex.vs", "../shaders/phong-vertex.fs");
        renderer.loadShader("phong-pixel", "../shaders/phong-pixel.vs", "../shaders/phong-pixel.fs");
        renderer.loadShader("phong-vertex-light", "../shaders/phong-vertex-light.vs", "../shaders/phong-vertex-light.fs");
        renderer.loadShader("phong-pixel-light", "../shaders/phong-pixels-light.vs", "../shaders/phong-pixels-light.fs");
        renderer.loadShader("toons", "../shaders/toons.vs", "../shaders/toons.fs");
        renderer.loadShader("spotlight", "../shaders/spotlight.vs", "../shaders/spotlight.fs");
        renderer.loadShader("holes", "../shaders/holes.vs", "../shaders/holes.fs");
    }

    void mouseMotion(int x, int y, int dx, int dy) {
        if (click) {
            h = h + dy * 0.1;
            a = a - dx * 0.1;
            float x = rad * sin(a) * cos(h);
            float y = rad * sin(h);
            float z = rad * cos(a) * cos(h);
            eyePos = vec3(x, y, z);
        }
    }

    void mouseDown(int button, int mods) {
        click = true;
    }

    void mouseUp(int button, int mods) {
        click = false;
    }

    void scroll(float dx, float dy) {
        rad = rad + dy;
        float x = rad* sin(a) * cos(h);
        float y = rad*sin(h);
        float z = rad* cos(a) * cos(h);
        eyePos = vec3(x, y, z);
    }

    void keyUp(int key, int mods) {
      if(key == GLFW_KEY_N){
         if(curIn == m.size() -1){
            curIn = 0;
            std::cout << file[curIn] << std::endl;
         }
         else{
            curIn = curIn + 1;
            std::cout << file[curIn] << std::endl;
         }
      }
      else if(key == GLFW_KEY_P){
         if(curIn == 0){
            curIn = m.size() -1;
            std::cout << file[curIn] << std::endl;
         }
         else{
            curIn = curIn - 1;
            std::cout << file[curIn] << std::endl;
         }
      }
      else if(key == GLFW_KEY_S){
          if(curSh == sh.size() - 1){
              curSh = 0;
          }
          else{
              curSh = curSh + 1;
          } 
      }
   }

    void draw() {
        renderer.beginShader(sh[curSh]);
        renderer.setUniform("m.ka", vec3(0.1f));
        renderer.setUniform("m.kd", vec3(1.0f));
        renderer.setUniform("m.ks", vec3(0.6f));
        renderer.setUniform("m.shine", 44.4f);
        renderer.setUniform("l.la", vec3(0.5f,0.0f,0.5f));
        renderer.setUniform("l.ld", vec3(0.5f,0.0f,0.5f));
        renderer.setUniform("l.ls", vec3(0.5f,0.0f,0.5f));
        renderer.setUniform("l2.la", vec3(1.0f,1.0f,0.0f));
        renderer.setUniform("l2.ld", vec3(1.0f,1.0f,0.0f));
        renderer.setUniform("l2.ls", vec3(1.0f,1.0f,0.0f));
        renderer.setUniform("l2.pos", vec4(5.5f,10.5f,-10.5f,1.0f));
        renderer.setUniform("l3.la", vec3(1.0f));
        renderer.setUniform("l3.ld", vec3(1.0f));
        renderer.setUniform("l3.ls", vec3(1.0f));
        renderer.setUniform("l3.pos", vec4(25.0f,15.0f,-15.0f,1.0f));
        renderer.setUniform("l3.exp", 15.0f);
        renderer.setUniform("l3.co", 30.0f);
        


        mesh = m[curIn]; 
        float aspect = ((float)width()) / height();
        renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
        renderer.lookAt(eyePos, lookPos, up);

       
        float sx = 1;
        float sy = 1;
        float sz = 1;
        GLfloat xr = mesh.maxBounds().x - mesh.minBounds().x;
        GLfloat yr = mesh.maxBounds().y - mesh.minBounds().y;
        GLfloat zr = mesh.maxBounds().z - mesh.minBounds().z;
        if ((xr > yr) || (xr > zr)) {
            sx = 5.0f / xr;
            sy = 5.0f / xr;
            sz = 5.0f / xr;
        }
        else if (yr > zr) {
            sx = 5.0f / yr;
            sy = 5.0f / yr;
            sz = 5.0f / yr;
        }    
        else if (zr > yr) {
            sx = 5.0f / zr;
            sy = 5.0f / zr;
            sz = 5.0f / zr;

        }
        renderer.scale(vec3(sx, sy, sz));

        //renderer.scale(vec3(sin(elapsedTime())*10, cos(elapsedTime())*10, -cos(elapsedTime()*10)));
        renderer.translate(vec3(0, 0, 0));
        renderer.lookAt(eyePos,lookPos, up);

        renderer.mesh(mesh);
        //renderer.cube(); // for debugging!

        
        renderer.endShader();
    }

protected:
    std::vector<string> file = GetFilenamesInDir("../models", "ply");
    PLYMesh mesh;
    std::vector<PLYMesh> m;
    std::vector<string> sh = {"normals", "phong-vertex", "phong-pixel","phong-vertex-light","phong-pixel-light","toons","spotlight","holes"};
    vec3 eyePos = vec3(10, 0, 0);
    vec3 lookPos = vec3(0, 0, 0);
    vec3 up = vec3(0, 1, 0);
    bool click = false;
    float h = 0;
    float a = 0;
    float rad = 10;
    int curIn = 0;
    int curSh = 0;
};

int main(int argc, char** argv)
{
    MeshViewer viewer;
    viewer.run();
    return 0;
}