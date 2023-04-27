//--------------------------------------------------
// Author: Synarah Sitaf
// Date: 3/23/23
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include "plymesh.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>

using namespace std;
using namespace glm;

namespace agl {

   PLYMesh::PLYMesh(const std::string& filename) {
      load(filename);
   }

   PLYMesh::PLYMesh() {
   }

   void PLYMesh::init() {
      assert(_positions.size() != 0);
      initBuffers(&_faces, &_positions, &_normals);
   }

   PLYMesh::~PLYMesh() {
   }

   bool PLYMesh::load(const std::string& filename) {
      if (_positions.size() != 0) {
         std::cout << "WARNING: Cannot load different files with the same PLY mesh\n";
         return false;
      }
      // todo: your code here
      std::ifstream myFile(filename);

      if(!myFile.is_open()) {
         return false;
      }

      std::string info;
      if(!std::getline(myFile, info) || info != "ply") {
         return false;
      }

      int numVer = 0;
      int numFace = 0;
      while(std::getline(myFile, info) && info != "end_header"){
         std::istringstream s(info);
         std::string key;
         s >> key;

         if(key == "element"){
            std::string eleTy;
            int con;
            s >> eleTy >> con;

            if(eleTy == "vertex"){
               numVer = con;
            }
            else if(eleTy == "face"){
               numFace = con;
            }
         }
      }


      for(int i = 0; i < numVer; i++) {
         std::getline(myFile, info);
         std::istringstream s(info);
         float v1, v2, v3, v4, v5, v6;
         s >> v1 >> v2 >> v3 >> v4 >> v5 >> v6;

         _positions.push_back(v1);
         _positions.push_back(v2);
         _positions.push_back(v3);
         _normals.push_back(v4);
         _normals.push_back(v5);
         _normals.push_back(v6);
      }

      for(int i = 0; i < numFace; i++) {
         int con, f1, f2, f3;
         myFile >> con >> f1 >> f2 >> f3;

         _faces.push_back(f1);
         _faces.push_back(f2);
         _faces.push_back(f3);

         for(int j = 0; j < con - 3; j++){
            int fi;
            myFile >> fi;
            _faces.push_back(f1 - 1);
            _faces.push_back(fi - 1);
            _faces.push_back(fi - 1); 
         }
      }

      if(numVer == 0 || numFace == 0){
         return false;
      }

      return true;
   }

   glm::vec3 PLYMesh::minBounds() const {
      glm::vec3 min(_positions[0], _positions[1], _positions[2]);

      for(int i = 0; i < _positions.size(); i += 3){
      glm::vec3 ver(_positions[i], _positions[i+1], _positions[i+2]);
          if(ver.x < min.x){
            min.x = ver.x;
         }
         if(ver.y < min.y){
            min.y = ver.y;
         }
         if(ver.z < min.z){
            min.z = ver.z;
         }
      }
      return min;
   }

   glm::vec3 PLYMesh::maxBounds() const {
      glm::vec3 max(_positions[0], _positions[1], _positions[2]);

       for (int i = 0; i < _positions.size(); i += 3){
         glm::vec3 ver(_positions[i], _positions[i+1], _positions[i+2]);
         if(ver.x > max.x){
            max.x = ver.x;
         }
         if(ver.y > max.y){
            max.y = ver.y;
         }
         if(ver.z > max.z){
            max.z = ver.z;
         }
       }
      return max;
   }

   int PLYMesh::numVertices() const {
      return _positions.size()/3;
   }

   int PLYMesh::numTriangles() const {
      return _faces.size()/3;
   }

   const std::vector<GLfloat>& PLYMesh::positions() const {
      return _positions;
   }

   const std::vector<GLfloat>& PLYMesh::normals() const {
      return _normals;
   }

   const std::vector<GLuint>& PLYMesh::indices() const {
      return _faces;
   }

   void PLYMesh::clear(){
      _positions.clear();
      _normals.clear();
      _faces.clear();
   }
}
