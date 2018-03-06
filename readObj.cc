/*
Copyright (C) 2018 Andreas Bertsatos <andreas.bertsatos@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <octave/oct.h>

struct Coord
{
	    double x, y, z;
};
struct Faces
{
      int a, b, c;
};  
struct TCoord
{
      double u, v  ;
};
      

DEFUN_DLD (readObj, args, nargout, 
          "-*- texinfo -*-\n\
@deftypefn{Loadable function} @var{output_arguments} = readObj(@var{filename})\n\
\n\
\n\
Example: [@var{v}, @var{f}] = readObj(\"3DMesh.obj\")\n\
\n\
\n\
This function loads a triangular 3D Mesh from Wavefront Obj file and stores\n\
its elements into the appropriately defined output arguments.\n\
\n\
If only two output arguments are given then only the vertices and the faces of\n\
the mesh are returned. If four arguments are given, then vertex normals and\n\
face normals or texture coordinates and texture faces are returned, depending\n\
on which are present. If both sets are present then @code{readObj} returns only\n\
the texture coordinates and their corresponding faces. If six output arguments are\n\
given then all elements are returned as numerical array in the following order:\n\
\n\
@var{Vertices} as an Nx3 matrix with floating point values.\n\
\n\
@var{Faces} as an Nx3 matrix with integer values.\n\
\n\
@var{Texture Coordinates} as an Nx2 matrix with floating point values.\n\
\n\
@var{Texture Faces} as an Nx3 matrix with integer values.\n\
\n\
@var{Vertex Normals} as an Nx3 matrix with floating point values.\n\
\n\
@var{Face Normals} as an Nx3 matrix with integer values.\n\
\n\
If odd number of output arguments are provided, then the last output argument is\n\
used for returning the .mtl filename, where material parameters are stored.\n\
Note that @code{readObj} handles explicitly triangular mesh objects. If Obj file\n\
does not contain a proper triangular mesh, then an error message is returned.\n\
@end deftypefn")
{

  // Check if there is a valid number of output arguments
  if (nargout < 2 || nargout > 7)
  {
      std::cout << "Invalid number of output arguments.\n";
      return octave_value_list();
  }
  
  // store filename string of obj file
  std::string file = args(0).string_value();
  // define string variable for storing material library file if referenced in obj
  std::string mtl_filename;
  // open file
  std::ifstream inputFile(file.c_str());
  // define matrices for storing vertices and faces retrieved from obj
  std::vector<Coord> vertex;
  std::vector<Faces> face;
  // define matrices for storing normals and texture coordinates from obj
  std::vector<Coord> normals;
  std::vector<TCoord> texture;
  // define matrices for storing face normals and face texture
  std::vector<Faces> face_normals;
  std::vector<Faces> face_texture;
  
  
  // initiate counters for vertices, normals, texture and faces
  octave_idx_type vertex_counter = 0;
  octave_idx_type normals_counter = 0;
  octave_idx_type texture_counter = 0;
  octave_idx_type face_counter = 0;
  octave_idx_type faceT_counter = 0;
  octave_idx_type faceN_counter = 0;
  
  // check that file exists
  if(inputFile)
  {
    // define string for reading obj file per line
    std::string line;
    while (std::getline(inputFile, line))
    {
        if(line[0] == 'm' && line[1] == 't' && line[2] == 'l')
        {
            int str_start = line.rfind(" ") + 1;
            if(line.find("./") != -1) {str_start = line.find("./") + 2;}
            int str_end = line.length();
            int str_len = str_end - str_start;
            mtl_filename = line.substr(str_start, str_len);
        }
        if(line[0] == 'v' && line[1] == ' ')
        {
            float tmpx, tmpy, tmpz;
          
            sscanf(line.c_str(), "v %f %f %f" ,&tmpx,&tmpy,&tmpz);
        
            Coord temp3D = {tmpx, tmpy, tmpz};
          
            vertex.push_back(temp3D);
          
            vertex_counter++;
        }
        if(line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
        {
            float tmpx, tmpy, tmpz;
          
            sscanf(line.c_str(), "vn %f %f %f" ,&tmpx,&tmpy,&tmpz);
        
            Coord temp3D = {tmpx, tmpy, tmpz};
          
            normals.push_back(temp3D);
          
            normals_counter++;
        }
        if(line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        {
            float tmpu, tmpv;
          
            sscanf(line.c_str(), "vt %f %f" ,&tmpu,&tmpv);
        
            TCoord tempTexture = {tmpu, tmpv};
          
            texture.push_back(tempTexture);
          
            texture_counter++;  
        }  
        if(line[0] == 'f' && line[1] == ' ')
        {
            // check for non triangular mesh
            int v1=0, v2=0, v3=0, v4=0, vt1=0, vt2=0;
            int vt3=0, vt4=0, vn1=0, vn2=0, vn3=0, vn4=0;
            sscanf(line.c_str(), "f %d %d %d %d" ,&v1,&v2,&v3,&v4);
            if (v1 > 0 && v2 > 0 && v3 > 0 && v4 > 0)
            {    
                std::cout << "Mesh is not triangular.\n";
                return octave_value_list();
            }
            sscanf(line.c_str(), "f %d/%d %d/%d %d/%d %d/%d", 
                    &v1,&vt1,&v2,&vt2,&v3,&vt3,&v4,&vt4);
            if (v1 > 0 && v2 > 0 && v3 > 0 && v4 > 0 
                && vt1 > 0 && vt2 > 0 && vt3 > 0 && vt4 > 0)
            {    
                std::cout << "Mesh is not triangular.\n";
                return octave_value_list();
            }
            sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
                    &v1,&vt1,&vn1,&v2,&vt2,&vn2,&v3,&vt3,&vn3,&v4,&vt4,&vn4);
            if (v1 > 0 && v2 > 0 && v3 > 0 && v4 > 0 && vt1 > 0 && vt2 > 0 
                && vt3 > 0 && vt4 > 0 && vn1 > 0 && vn2 > 0 && vn3 > 0 && vn4 > 0)
            {    
                std::cout << "Mesh is not triangular.\n";
                return octave_value_list();
            }
            sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d",
                    &v1,&vn1,&v2,&vn2,&v3,&vn3,&v4,&vn4);
            if (v1 > 0 && v2 > 0 && v3 > 0 && v4 > 0 && vn1 > 0 
                && vn2 > 0 && vn3 > 0 && vn4 > 0)
            {    
                std::cout << "Mesh is not triangular.\n";
                return octave_value_list();
            }
            
            
            face_counter++;
            // Reset temp face variables
            v1=0;v2=0;v3=0,vt1=0,vt2=0,vt3=0,vn1=0,vn2=0,vn3=0;

            // scan for faces only
            sscanf(line.c_str(), "f %d %d %d" ,&v1,&v2,&v3);
            if (v1 > 0 && v2 > 0 && v3 > 0)
            {    
                Faces temp_face_v = {v1, v2, v3};
                face.push_back(temp_face_v);
            }
            else
            {
              // scan for faces with texture
              sscanf(line.c_str(), "f %d/%d %d/%d %d/%d",
                      &v1,&vt1,&v2,&vt2,&v3,&vt3);
              if (v1 > 0 && v2 > 0 && v3 > 0 && vt1 > 0 && vt2 > 0 && vt3 > 0)
              {    
                  Faces temp_face_v = {v1, v2, v3};
                  Faces temp_face_vt = {vt1, vt2, vt3};
                  face.push_back(temp_face_v);
                  face_texture.push_back(temp_face_vt);
                  faceT_counter++;
              }
              else
              {
                // scan for faces with texture and normals
                sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d" 
                      ,&v1,&vt1,&vn1,&v2,&vt2,&vn2,&v3,&vt3,&vn3);
                if (v1 > 0 && v2 > 0 && v3 > 0 && vt1 > 0 && vt2 > 0 
                    && vt3 > 0 && vn1 > 0 && vn2 > 0 && vn3 > 0)
                {    
                    Faces temp_face_v = {v1, v2, v3};
                    Faces temp_face_vt = {vt1, vt2, vt3};
                    Faces temp_face_vn = {vn1, vn2, vn3};
                    face.push_back(temp_face_v);
                    face_texture.push_back(temp_face_vt);
                    face_normals.push_back(temp_face_vn);
                    faceT_counter++;
                    faceN_counter++;
                }
                else
                {
                  // scan for faces with normals
                  sscanf(line.c_str(), "f %d//%d %d//%d %d//%d" 
                        ,&v1,&vn1,&v2,&vn2,&v3,&vn3);
                  if (v1 > 0 && v2 > 0 && v3 > 0 && vn1 > 0 && vn2 > 0 && vn3 > 0)
                  {    
                      Faces temp_face_v = {v1, v2, v3};
                      Faces temp_face_vn = {vn1, vn2, vn3};
                      face.push_back(temp_face_v);
                      face_normals.push_back(temp_face_vn);
                      faceN_counter++;
                  }
                  else
                  {
                      std::cout << "Mesh is not triangular.\n";
                      return octave_value_list();
                  }
                }
              }
            }
        }  
    }
  }
  else
  {
      std::cout << "Failure opening file.\n";
      return octave_value_list();
  }
  
    
  std::cout << "Model file contained " << vertex_counter << " vertices and "
            << face_counter << " faces.\n";

  // check if vertex coordinates exist and store them in Octave array
  Matrix V (vertex_counter, 3);
  if (vertex_counter > 0)
  {
      //Matrix V (vertex_counter, 3);
      for (octave_idx_type i = 0; i < vertex_counter; i++)
      {
          V(i,0) = vertex[i].x;
          V(i,1) = vertex[i].y;
          V(i,2) = vertex[i].z;
      }
  }
  else
  {
      std::cout << "Mesh does not contain any vertices.\n";
      return octave_value_list();
  }
  // check if texture coordinates exist and store them in Octave array
  Matrix VT (texture_counter, 2);
  if (texture_counter > 0)
  {
      std::cout << "Mesh contains texture.\n";
      for (octave_idx_type i = 0; i < texture_counter; i++)
      {
          VT(i,0) = texture[i].u;
          VT(i,1) = texture[i].v;
      }
  }
  else
  {
      std::cout << "Mesh does not contain any texture.\n";
  }
  // check if normal coordinates exist and store them in Octave array
  Matrix VN (normals_counter, 3);
  if (normals_counter > 0)
  {
      std::cout << "Mesh contains normals.\n";
      for (octave_idx_type i = 0; i < normals_counter; i++)
      {
          VN(i,0) = normals[i].x;
          VN(i,1) = normals[i].y;
          VN(i,2) = normals[i].z;
      }
  }
  else
  {
      std::cout << "Mesh does not contain any normals.\n";
  }
  // check if faces exist and store them in Octave array
  Matrix F (face_counter, 3);
  if (face_counter > 0)
  {
      for (octave_idx_type i = 0; i < face_counter; i++)
      {
          F(i,0) = face[i].a;
          F(i,1) = face[i].b;
          F(i,2) = face[i].c;
      }
  }
  else
  {
      std::cout << "Mesh does not contain any faces.\n";
      return octave_value_list();
  }
  // check if texture faces exist and store them in Octave array
  Matrix FT (faceT_counter, 3);
  if (faceT_counter > 0)
  {
      for (octave_idx_type i = 0; i < faceT_counter; i++)
      {
          FT(i,0) = face_texture[i].a;
          FT(i,1) = face_texture[i].b;
          FT(i,2) = face_texture[i].c;
      }
  }
  else
  {
      std::cout << "Mesh does not contain any texture faces.\n";
  }
  // check if face normals exist and store them in Octave array
  Matrix FN (faceN_counter, 3);
  if (faceN_counter > 0)
  {
      for (octave_idx_type i = 0; i < faceN_counter; i++)
      {
          FN(i,0) = face_normals[i].a;
          FN(i,1) = face_normals[i].b;
          FN(i,2) = face_normals[i].c;
      }
  }
  else
  {
      std::cout << "Mesh does not contain any face normals.\n";
  }
  
  // check the number of output arguments and store the appropriate matrices
  // to the octave_value_list variable. If only two output arguments are given
  // then store only the vertices and the faces of the mesh. If four arguments
  // are given, then additionally store vertex normals and face normals or 
  // texture coordinates and texture faces depending on which are present. If
  // both sets are present then store texture coordinates and their faces.
  //
  // If six output arguments are given, then return all matrices in the order
  // Vertices, Faces, Vertex Texture, Face Texture, Vertex Normals, Face Normals
  //
  // If odd number of output arguments is present, then the last output
  // argument is used for storing the filename with the material parameters.
  
  
  // define return value list
  octave_value_list retval;
  
  if (nargout == 2)
  {
      retval(0) = V;
      retval(1) = F;
  }
  if (nargout == 3)
  {
      retval(0) = V;
      retval(1) = F;
      retval(2) = mtl_filename.c_str();
      std::cout << "Material library file is present\n";
  }
  if (nargout == 4 && faceT_counter > 0 && texture_counter > 0)
  {
      retval(0) = V;
      retval(1) = F;
      retval(2) = VT;
      retval(3) = FT;
  }
  if (nargout == 4 && faceT_counter == 0 && texture_counter == 0 
          && faceN_counter > 0 && normals_counter > 0)
  {
      retval(0) = V;
      retval(1) = F;
      retval(2) = VN;
      retval(3) = FN;
  }
  if (nargout == 5 && faceT_counter > 0 && texture_counter > 0)
  {
      retval(0) = V;
      retval(1) = F;
      retval(2) = VT;
      retval(3) = FT;
      retval(4) = mtl_filename.c_str();
      std::cout << "Material library file is present\n";
  }
  if (nargout == 5 && faceT_counter == 0 && texture_counter == 0 
        && faceN_counter > 0 && normals_counter > 0)
  {
      retval(0) = V;
      retval(1) = F;
      retval(2) = VN;
      retval(3) = FN;
      retval(4) = mtl_filename.c_str();
      std::cout << "Material library file is present\n";
  }
  if (nargout == 6)
  {
      retval(0) = V;
      retval(1) = F;
      retval(2) = VT;
      retval(3) = FT;
      retval(4) = VN;
      retval(5) = FN;
  }
  if (nargout == 7)
  {
      retval(0) = V;
      retval(1) = F;
      retval(2) = VT;
      retval(3) = FT;
      retval(4) = VN;
      retval(5) = FN;
      retval(6) = mtl_filename.c_str();
      std::cout << "Material library file is present\n";
  }
  
  return retval;
}

