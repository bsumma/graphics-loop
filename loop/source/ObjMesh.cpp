#include "common.h"

//This code takes in a triangle index inside the triangles vector
//It then produces 4 triangles that are the subdivision of the initial triangle
std::vector <trimesh::triangle_t >  Mesh::subdivideTriangle(unsigned int i) {
  std::vector < trimesh::triangle_t > newTriangles(4);
  
  //TODO split edges and compute new triangles for a given triangle at index i

  return newTriangles;
  
};


bool Mesh::loopSubdivision() {
  
  //TODO Subdivide triangles
  
  
  //TODO Move old vertices
  
  
  //Rebuild half-edge datastructure
  trimesh::unordered_edges_from_triangles( triangles.size(), &triangles[0], edges );
  mesh.build( vertices.size(), triangles.size(), &triangles[0], edges.size(), &edges[0] );
  computeNormals();
  
};


bool Mesh::getGLarraysSmooth(std::vector < vec4 > &GLvertices,
                             std::vector < vec3 > &GLnormals) 
{
  
  std::vector < vec3 > vertex_normal;
  
  for(unsigned int i=0; i < vertices.size(); i++){
    
    std::vector< trimesh::index_t > faces =  mesh.vertex_face_neighbors(i);
    vec3 normal = vec3(0,0,0);
    for(unsigned int j =0; j < faces.size(); j++){
      normal += triangle_normals[faces[j]];
    }
    normal /= (float)faces.size();
    vertex_normal.push_back(normalize(normal));
  }

  for(unsigned int i=0; i < triangles.size(); i++){
    unsigned long a = triangles[i].v[0];
    unsigned long b = triangles[i].v[1];
    unsigned long c = triangles[i].v[2];
    
    GLvertices.push_back(vertices[a]);
    GLvertices.push_back(vertices[b]);
    GLvertices.push_back(vertices[c]);
    
    GLnormals.push_back(vertex_normal[a]);
    GLnormals.push_back(vertex_normal[b]);
    GLnormals.push_back(vertex_normal[c]);
    
  }
  
}

bool Mesh::getGLarrays(std::vector < vec4 > &GLvertices,
                       std::vector < vec3 > &GLnormals)
{
  
  for(unsigned int i=0; i < triangles.size(); i++){
    unsigned long a = triangles[i].v[0];
    unsigned long b = triangles[i].v[1];
    unsigned long c = triangles[i].v[2];
    
    GLvertices.push_back(vertices[a]);
    GLvertices.push_back(vertices[b]);
    GLvertices.push_back(vertices[c]);
    
    GLnormals.push_back(triangle_normals[i]);
    GLnormals.push_back(triangle_normals[i]);
    GLnormals.push_back(triangle_normals[i]);
    
  }
  
}

void Mesh::computeNormals()
{
  
  
  triangle_normals.resize(triangles.size(), vec3(1.0, 0.0, 0.0));
  
  for(unsigned int i=0; i < triangles.size(); i++){
    unsigned long a = triangles[i].v[0];
    unsigned long b = triangles[i].v[1];
    unsigned long c = triangles[i].v[2];
    
    vec4 bVec = vertices[b] - vertices[a];
    vec4 cVec = vertices[c] - vertices[a];


    vec3 normal = normalize(cross(bVec,cVec));
    
    if( dot(normal, triangle_normals[i]) < 0){ normal = -normal; }
    
    triangle_normals[i] = normal;

  }
}


bool Mesh::loadOBJ(const char * path)
{
  
  std::vector< vec3 > normals_temp;
  
  hasUV = true;
  
#ifdef _WIN32
  std::wstring wcfn;
  if ( u8names_towc(path, wcfn) != 0 ){
    printf("Impossible to process the file name !\n");
    return false;
  }
  FILE * file = _wfopen(wcfn.c_str(), L"r");
#else
  FILE * file = fopen(path, "r");
#endif //_WIN32
  if( file == NULL ){
    printf("Impossible to open the file !\n");
    return false;
  }
  
  char *line = new char[128];
  char *lineHeader = new char[128];
  
  while(true){
    memset(line, 0 , 128);
    memset(lineHeader, 0 , 128);
    
    if(fgets(line, 128, file) == NULL){ break; }
    sscanf(line, "%s ", lineHeader);
    
    if ( strcmp( lineHeader, "v" ) == 0 ){
      vec3 vertex;
      sscanf(&line[2], "%f %f %f", &vertex.x, &vertex.y, &vertex.z );
      vertices.push_back(vertex);
      if(vertex.x < box_min.x){box_min.x = vertex.x; }
      if(vertex.y < box_min.y){box_min.y = vertex.y; }
      if(vertex.z < box_min.z){box_min.z = vertex.z; }
      if(vertex.x > box_max.x){box_max.x = vertex.x; }
      if(vertex.y > box_max.y){box_max.y = vertex.y; }
      if(vertex.z > box_max.z){box_max.z = vertex.z; }
    }else if ( strcmp( lineHeader, "vt" ) == 0 ){
      vec2 uv;
      sscanf(&line[3], "%f %f", &uv.x, &uv.y );
      uvs.push_back(uv);
    }else if ( strcmp( lineHeader, "vn" ) == 0 ){
      vec3 normal;
      sscanf(&line[3], "%f %f %f", &normal.x, &normal.y, &normal.z );
      normals_temp.push_back(normal);
    }else if ( strcmp( lineHeader, "f" ) == 0 ){
      std::string vertex1, vertex2, vertex3;
      int vertexIndex[3], uvIndex[3], normalIndex[3];
      int matches = sscanf(&line[2], "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                           &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                           &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
      if (matches != 9){
        int matches = sscanf(&line[2], "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0],
                             &vertexIndex[1], &normalIndex[1],
                             &vertexIndex[2], &normalIndex[2]);
        if (matches == 6){
          hasUV = false;
        } else {
          printf("File can't be read by our simple parser : ( Try exporting with other options\n");
          return false;
        }
      }
      
      /* handle negative indices */
      /* (adjust for size during processing of each face, as per the old
       *  OBJ specification, instead of after the end of the file) */
      for (int negati = 0; negati < 3; negati++){
        if (vertexIndex[negati] < 0){
          vertexIndex[negati]+=vertices.size();
          vertexIndex[negati]++; /* <- OBJ indices are one-based */
        }
        if (uvIndex[negati] < 0){
          uvIndex[negati]+=uvs.size();
          uvIndex[negati]++;
        }
        if (normalIndex[negati] < 0){
          normalIndex[negati]+=normals_temp.size();
          normalIndex[negati]++;
        }
      }
      
      trimesh::triangle_t triangle;
      triangle.v[0] = vertexIndex[0]-1;
      triangle.v[1] = vertexIndex[1]-1;
      triangle.v[2] = vertexIndex[2]-1;
      triangles.push_back(triangle);
      //just an intialization
      
      vec3 temp = (normals_temp[normalIndex[0]-1]+
                   normals_temp[normalIndex[1]-1]+
                   normals_temp[normalIndex[2]-1])/3.0;
      
      triangle_normals.push_back(normalize(temp));

    }
  }
  
  delete[] line;
  delete[] lineHeader;
  
  std::cout << "Read " << vertices.size() << " vertices\n";
  std::cout << "Read " << normals_temp.size() << " normals\n";
  std::cout << "Read " << triangles.size() << " faces\n";
  
  trimesh::unordered_edges_from_triangles( triangles.size(), &triangles[0], edges );
  
  std::cout << "Read " << edges.size() << " edges\n";

  mesh.build( vertices.size(), triangles.size(), &triangles[0], edges.size(), &edges[0] );
  
  //Find normals

  computeNormals();

  center = box_min+(box_max-box_min)/2.0;
  scale = (std::max)(box_max.x - box_min.x, box_max.y-box_min.y);
  
  model_view = Scale(1.0/scale,           //Make the extents 0-1
                     1.0/scale,
                     1.0/scale)*
  Translate(-center);  //Orient Model About Center
  
  
  return true;
}


