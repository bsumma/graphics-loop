//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/


#ifndef __OBJLOADER__
#define __OBJLOADER__

#include "common.h"

using namespace Angel;

class Mesh{
public:
  bool hasUV;
  
  //Ordered set of mesh vertices
  std::vector< vec3 > vertices;
  //Ordered set of vertex uv
  std::vector< vec2 > uvs;
  
  //set of triangles, triangle_t is a struct of three indices in vertices
  std::vector< trimesh::triangle_t > triangles;
  //set of normals for each triangle
  std::vector< vec3 > triangle_normals;

  //Our half-edge mesh representation
  trimesh::trimesh_t mesh;

  //set of edges produced by half-edge code
  std::vector< trimesh::edge_t > edges;

  //A handy edge to index map
  std::map < std::pair < trimesh::index_t, trimesh::index_t > , trimesh::index_t > edge_map;
  

  vec3 box_min;
  vec3 box_max;
  vec3 center;
  float scale;
  
  mat4 model_view;
  
  Mesh(const char * path)
    : box_min((std::numeric_limits< float >::max)(),
              (std::numeric_limits< float >::max)(),
              (std::numeric_limits< float >::max)() ),
    box_max(0,0,0),
    center(0,0,0),
    scale(1.0),
    model_view(){ loadOBJ(path); }
  
  unsigned long getNumTri(){ return triangles.size(); }

  bool loadOBJ(const char * path);
  
  friend std::ostream& operator << ( std::ostream& os, const Mesh& v ) {
    os << "Vertices:\n";
    for(unsigned int i=0; i < v.vertices.size(); i++){
      os << "\t\t" << v.vertices[i] << "\n";
    }
    os << "Normals:\n";
    for(unsigned int i=0; i < v.triangle_normals.size(); i++){
      os << "\t\t" << v.triangle_normals[i] << "\n";
    }
 
    return os;
  }
  
  std::vector <trimesh::triangle_t > subdivideTriangle(unsigned int i);
  
  bool loopSubdivision();
  
  void computeNormals();
  
  bool getGLarraysSmooth(std::vector < vec4 > &vertices,
                         std::vector < vec3 > &normals);
  
  bool getGLarrays(std::vector < vec4 > &vertices,
                       std::vector < vec3 > &normals);
  
};


#endif  //#ifndef __OBJLOADER__
