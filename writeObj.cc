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
#include <octave/parse.h>

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
      

DEFUN_DLD (writeObj, args, , 
          "-*- texinfo -*-\n\
@deftypefn{Loadable function} writeObj(@var{input_arguments})\n\
\n\
\n\
Example: writeObj(V, F, \"3DMesh.obj\")\n\
\n\
\n\
This function saves a triangular 3D Mesh to a Wavefront Obj file according to\n\
its elements provided as input arguments.\n\
\n\
The function will only take 3, 5 or 7 input arguments. The last argument should\n\
be a string referring to the filename of the OBJ file, whereas the first 2, 4 or\n\
6 input arguments should be 2-dimensional matrices containing the elements of the\n\
triangular mesh in the following order:\n\
\n\
@var{Vertices} should be an Nx3 matrix with floating point values.\n\
\n\
@var{Faces} should be an Nx3 matrix with integer values.\n\
\n\
@var{Texture Coordinates} should be an Nx2 matrix with floating point values.\n\
\n\
@var{Texture Faces} should be an Nx3 matrix with integer values.\n\
\n\
@var{Vertex Normals} should be an Nx3 matrix with floating point values.\n\
\n\
@var{Face Normals} should be an Nx3 matrix with integer values.\n\
\n\
If 5 input arguments are provided, the function will determine whether there is\n\
a texture coordinates matrix or a vertex normals matrix by the dimensions of the\n\
matrix provided as the third input argument\n\
@end deftypefn")
{

  // count the number of input arguments and store their values
  // into the appropriate variables
  // check for invalid number of input arguments
  if (args.length() != 3 && args.length() != 5 && args.length() != 7)
  {
    std::cout << "Invalid number of input arguments.\n";
    return octave_value_list();
  }
  // for three input arguments
  //
  // check for last argument being a string
  if (args.length() == 3 && !args(2).is_string())
  {
    std::cout << "Third input argument should be a string.\n";
    return octave_value_list();
  }
  // check for first two arguments being real matrices
  if (!args(0).is_matrix_type() || !args(1).is_matrix_type())
  {
    std::cout << "The first two arguments should be real matrices.\n";
    return octave_value_list();
  }
  // considering two input arguments for vertices and faces respcectively and
  // and a third argument as string for filename under which the vertices and
  // will be saved
  if (args.length() == 3 && args(2).is_string())
  {
    // store vertices, faces and filename
    Matrix V = args(0).array_value();
    Matrix F = args(1).array_value();
    std::string filename = args(2).string_value();
    // find number of vertices and faces
    octave_idx_type V_rows = args(0).rows();
    octave_idx_type F_rows = args(1).rows();
    octave_idx_type V_columns = args(0).columns();
    octave_idx_type F_columns = args(1).columns();
    // ensure that there are at least 3 vertices and one face in the mesh and
    // vertex and face matrices are Nx3 in size
    if (V_rows < 3)
    {
      std::cout << "There should be at least 3 vertices in the mesh.\n";
      return octave_value_list();
    }
    if (V_columns != 3)
    {
      std::cout << "Vertex matrix should be Nx3 containing x,y,z coordinates.\n";
      return octave_value_list();
    }
    if (F_rows < 1)
    {
      std::cout << "There should be at least 1 face in the mesh.\n";
      return octave_value_list();
    }
    if (F_columns != 3)
    {
      std::cout << "Face matrix should be Nx3 containing three vertices.\n";
      return octave_value_list();
    }
    // define matrices for storing vertices and faces to file
    std::vector<Coord> vertex;
    std::vector<Faces> face;
    // store vertices in a vector
    float tmpx, tmpy, tmpz;
    for (octave_idx_type i = 0; i < V_rows; i++)
    {
      tmpx = V(i,0);
      tmpy = V(i,1);
      tmpz = V(i,2);
      Coord temp_vertex = {tmpx, tmpy, tmpz};
      vertex.push_back(temp_vertex);
    }
    // store faces in a vector
    int tmpa, tmpb, tmpc;
    for (octave_idx_type i = 0; i < F_rows; i++)
    {
      tmpa = F(i,0);
      tmpb = F(i,1);
      tmpc = F(i,2);
      Faces temp_face = {tmpa, tmpb, tmpc};
      face.push_back(temp_face);
    }
    ////
    ////
    //// code for creating file and storing elements goes here!!!
    //// vertices + faces
    ////
    ////
    // check if filename exists
    bool filename_exists = std::ifstream(filename.c_str()).good();
    if (filename_exists)
    {
      std::cout << "Filename already exists.\n";
      std::cout << "Do you want to replace? (yes or no)\n";
      std::string yes_or_no;
      getline(std::cin, yes_or_no);
      while (yes_or_no.compare("yes") && yes_or_no.compare("no"))
      {
        std::cout << "Please answer yes or no! ";
        getline(std::cin, yes_or_no);
      }
      if (yes_or_no.compare("yes"))
      {
        std::string newfilename;
        std::cout << "Please enter new filename: ";
        getline(std::cin, newfilename);
        filename = newfilename.c_str();
      }
    }
    std::ofstream outputFile(filename.c_str());
    if (!outputFile.is_open())
    {
      std::cout << "Error opening " << filename.c_str() << "for write\n";
      return octave_value_list();
    }
    else
    {
      // writing header to file
      outputFile << "#\n# OBJ File generated by GNU Octave\n# using 'writeObj' function\n";
      outputFile << "#\n# Object " << filename.c_str() << "\n#\n";
      outputFile << "# Vertices: " << V_rows << "\n";
      outputFile << "# Faces: " << F_rows << "\n#\n#\n";
      int i = filename.length() - 3;
      std::string mtlfilename = filename.c_str();
      outputFile << "mtllib ./" << mtlfilename.replace(i,3, "mtl") <<"\n\n";
      std::cout << "Writing to file... ";
      // write vertices to file
      for(std::vector<Coord>::iterator v_it = vertex.begin(); v_it != vertex.end(); ++v_it)
      {
        outputFile << "v " << v_it->x <<" "<< v_it->y <<" "<< v_it->z <<"\n";
      }
      // write faces to file
      for(std::vector<Faces>::iterator f_it = face.begin(); f_it != face.end(); ++f_it)
      {
        outputFile << "f " << f_it->a <<" "<< f_it->b <<" "<< f_it->c <<"\n";
      }
      outputFile.close();
      std::cout << "done!\n";
    }
    ////
    ////
    ////
    ////
    std::cout << "Mesh filename is " << filename.c_str() << "\n";
    std::cout << "Mesh has " << V_rows << " vertices.\n";
    std::cout << "Mesh has " << F_rows << " faces.\n";
    return octave_value_list();
  }
  // for five input arguments
  //
  // check for last argument being a string
  if (args.length() == 5 && !args(4).is_string())
  {
    std::cout << "Fifth input argument should be a string.\n";
    return octave_value_list();
  }
  // check for first four arguments being real matrices
  if (!args(0).is_matrix_type() || !args(1).is_matrix_type() || 
        !args(2).is_matrix_type() || !args(3).is_matrix_type())
  {
    std::cout << "The first four arguments should be real matrices.\n";
    return octave_value_list();
  }
  // considering four input arguments being real matrices, the first two are for
  // vertices and faces respcectively, whereas the latter will either be for
  // texture coordinates and texture faces or for vertex normals and face normals.
  // This will depend on the number of coordinates present in third input argument.
  // Te last input argument should again be a string
  // 
  // check for 5 input arguments with the third argument having two coordinates
  // present, namely u, v, and the last argument being a string
  if (args.length() == 5 && args(2).columns() == 2 && args(4).is_string())
  {
    // store vertices, faces and filename
    Matrix V = args(0).array_value();
    Matrix F = args(1).array_value();
    Matrix VT = args(2).array_value();
    Matrix FT = args(3).array_value();
    std::string filename = args(4).string_value();
    // find number of vertices and faces
    octave_idx_type V_rows = args(0).rows();
    octave_idx_type F_rows = args(1).rows();
    octave_idx_type VT_rows = args(2).rows();
    octave_idx_type FT_rows = args(3).rows();
    octave_idx_type V_columns = args(0).columns();
    octave_idx_type F_columns = args(1).columns();
    octave_idx_type VT_columns = args(2).columns();
    octave_idx_type FT_columns = args(3).columns();
    // ensure that there are at least 3 vertices and one face in the mesh and
    // vertex and face matrices are Nx3.
    if (V_rows < 3)
    {
      std::cout << "There should be at least 3 vertices in the mesh.\n";
      return octave_value_list();
    }
    if (V_columns != 3)
    {
      std::cout << "Vertex matrix should be Nx3 containing x,y,z coordinates.\n";
      return octave_value_list();
    }
    if (F_rows < 1)
    {
      std::cout << "There should be at least 1 face in the mesh.\n";
      return octave_value_list();
    }
    if (F_columns != 3)
    {
      std::cout << "Face matrix should be Nx3 containing three vertices.\n";
      return octave_value_list();
    }
    // check that texture faces are a Nx3 matrix
    if (FT_columns != 3)
    {
      std::cout << "Texture face matrix should be Nx3 containing three vertices.\n";
      return octave_value_list();
    }
    // define matrices for storing vertices, faces, texture coordinates and 
    // texture faces to file
    std::vector<Coord> vertex;
    std::vector<Faces> face;
    std::vector<TCoord> texture_coord;
    std::vector<Faces> face_texture;
    // store vertices in a vector
    float tmpx, tmpy, tmpz;
    for (octave_idx_type i = 0; i < V_rows; i++)
    {
      tmpx = V(i,0);
      tmpy = V(i,1);
      tmpz = V(i,2);
      Coord temp_vertex = {tmpx, tmpy, tmpz};
      vertex.push_back(temp_vertex);
    }
    // store faces in a vector
    int tmpa, tmpb, tmpc;
    for (octave_idx_type i = 0; i < F_rows; i++)
    {
      tmpa = F(i,0);
      tmpb = F(i,1);
      tmpc = F(i,2);
      Faces temp_face = {tmpa, tmpb, tmpc};
      face.push_back(temp_face);
    }
    // store texture coordinates in a vector
    float tmpu, tmpv;
    for (octave_idx_type i = 0; i < VT_rows; i++)
    {
      tmpu = VT(i,0);
      tmpv = VT(i,1);
      TCoord temp_texture_coord = {tmpu, tmpv};
      texture_coord.push_back(temp_texture_coord);
    }
    // store texture faces in a vector
    for (octave_idx_type i = 0; i < FT_rows; i++)
    {
      tmpa = FT(i,0);
      tmpb = FT(i,1);
      tmpc = FT(i,2);
      Faces temp_texture_face = {tmpa, tmpb, tmpc};
      face_texture.push_back(temp_texture_face);
    }
    // check that matrices for faces and texture faces have the same length
    // to ensure that the iterator will not break when writing to the file
    if (F_rows != FT_rows)
    {
      std::cout << "Faces and texture faces should contain the same\n"
                << "number of values.\n";
      return octave_value_list();
    }
    ////
    ////
    //// code for creating file and storing elements goes here!!!
    //// vertices + faces + texture coordinates + texture faces
    ////
    ////
    // check if filename exists
    bool filename_exists = std::ifstream(filename.c_str()).good();
    if (filename_exists)
    {
      std::cout << "Filename already exists.\n";
      std::cout << "Do you want to replace? (yes or no)\n";
      std::string yes_or_no;
      getline(std::cin, yes_or_no);
      while (yes_or_no.compare("yes") && yes_or_no.compare("no"))
      {
        std::cout << "Please answer yes or no! ";
        getline(std::cin, yes_or_no);
      }
      if (yes_or_no.compare("yes"))
      {
        std::string newfilename;
        std::cout << "Please enter new filename: ";
        getline(std::cin, newfilename);
        filename = newfilename.c_str();
      }
    }
    std::ofstream outputFile(filename.c_str());
    if (!outputFile.is_open())
    {
      std::cout << "Error opening " << filename.c_str() << "for write\n";
      return octave_value_list();
    }
    else
    {
      // writing header to file
      outputFile << "#\n# OBJ File generated by GNU Octave\n# using 'writeObj' function\n";
      outputFile << "#\n# Object " << filename.c_str() << "\n#\n";
      outputFile << "# Vertices: " << V_rows << "\n";
      outputFile << "# Faces: " << F_rows << "\n#\n#\n";
      int i = filename.length() - 3;
      std::string mtlfilename = filename.c_str();
      outputFile << "mtllib ./" << mtlfilename.replace(i,3, "mtl") <<"\n\n";
      std::cout << "Writing to file... ";
      // write vertices to file
      for(std::vector<Coord>::iterator v_it = vertex.begin(); v_it != vertex.end(); ++v_it)
      {
        outputFile << "v " << v_it->x <<" "<< v_it->y <<" "<< v_it->z <<"\n";
      }
      // write texture coordinates to file
      for(std::vector<TCoord>::iterator vt_it = texture_coord.begin(); 
                  vt_it != texture_coord.end(); ++vt_it)
      {
        outputFile << "vt " << vt_it->u <<" "<< vt_it->v <<"\n";
      }
      // write faces and texture faces to file
      std::vector<Faces>::const_iterator f_it;
      std::vector<Faces>::const_iterator ft_it;
      for(f_it = face.begin(), ft_it = face_texture.begin(); f_it != face.end(),
                  ft_it != face_texture.end(); ++f_it, ++ft_it)
      {
        outputFile << "f " << f_it->a <<"/"<< ft_it->a <<" "<< f_it->b <<"/"
                   << ft_it->b <<" "<< f_it->c <<"/"<< ft_it->c <<"\n";
      }
      outputFile.close();
      std::cout << "done!\n";
    }
    ////
    ////
    ////
    ////
    std::cout << "Mesh filename is " << filename.c_str() << "\n";
    std::cout << "Mesh has " << V_rows << " vertices.\n";
    std::cout << "Mesh has " << F_rows << " faces.\n";
    return octave_value_list();
  }
  // 
  // check for 5 input arguments with the third argument having three coordinates
  // present, namely x, y, z, and the last argument being a string
  if (args.length() == 5 && args(2).columns() == 3 && args(4).is_string())
  {
    // store vertices, faces and filename
    Matrix V = args(0).array_value();
    Matrix F = args(1).array_value();
    Matrix VN = args(2).array_value();
    Matrix FN = args(3).array_value();
    std::string filename = args(4).string_value();
    // find number of vertices and faces
    octave_idx_type V_rows = args(0).rows();
    octave_idx_type F_rows = args(1).rows();
    octave_idx_type VN_rows = args(2).rows();
    octave_idx_type FN_rows = args(3).rows();
    octave_idx_type V_columns = args(0).columns();
    octave_idx_type F_columns = args(1).columns();
    octave_idx_type VN_columns = args(2).columns();
    octave_idx_type FN_columns = args(3).columns();
    // ensure that there are at least 3 vertices and one face in the mesh and
    // vertex normals and face normals matrices are Nx3.
    if (V_rows < 3)
    {
      std::cout << "There should be at least 3 vertices in the mesh.\n";
      return octave_value_list();
    }
    if (V_columns != 3)
    {
      std::cout << "Vertex matrix should be Nx3 containing x,y,z coordinates.\n";
      return octave_value_list();
    }
    if (F_rows < 1)
    {
      std::cout << "There should be at least 1 face in the mesh.\n";
      return octave_value_list();
    }
    if (F_columns != 3)
    {
      std::cout << "Face matrix should be Nx3 containing three vertices.\n";
      return octave_value_list();
    }
    // check that face normals are a Nx3 matrix
    if (FN_columns != 3)
    {
      std::cout << "Face normals should be an Nx3 matrix.\n";
      return octave_value_list();
    }
    // define matrices for storing vertices and faces to file
    std::vector<Coord> vertex;
    std::vector<Faces> face;
    std::vector<Coord> vertex_normals;
    std::vector<Faces> face_normals;
    // store vertices in a vector
    float tmpx, tmpy, tmpz;
    for (octave_idx_type i = 0; i < V_rows; i++)
    {
      tmpx = V(i,0);
      tmpy = V(i,1);
      tmpz = V(i,2);
      Coord temp_vertex = {tmpx, tmpy, tmpz};
      vertex.push_back(temp_vertex);
    }
    // store faces in a vector
    int tmpa, tmpb, tmpc;
    for (octave_idx_type i = 0; i < F_rows; i++)
    {
      tmpa = F(i,0);
      tmpb = F(i,1);
      tmpc = F(i,2);
      Faces temp_face = {tmpa, tmpb, tmpc};
      face.push_back(temp_face);
    }
    // store vertex normals in a vector
    for (octave_idx_type i = 0; i < VN_rows; i++)
    {
      tmpx = VN(i,0);
      tmpy = VN(i,1);
      tmpz = VN(i,2);
      Coord temp_vertex_normals = {tmpx, tmpy, tmpz};
      vertex_normals.push_back(temp_vertex_normals);
    }
    // store face normals in a vector
    for (octave_idx_type i = 0; i < FN_rows; i++)
    {
      tmpa = FN(i,0);
      tmpb = FN(i,1);
      tmpc = FN(i,2);
      Faces temp_face_normals = {tmpa, tmpb, tmpc};
      face_normals.push_back(temp_face_normals);
    }
    // check that matrices for faces and face normals have the same length
    // to ensure that the iterator will not break when writing to the file
    if (F_rows != FN_rows)
    {
      std::cout << "Faces and face normals should contain the same\n"
                << "number of values.\n";
      return octave_value_list();
    }
    ////
    ////
    // code for creating file and storing elements goes here!!!
    // vertices + faces + vertex normals + face normals
    ////
    ////
    // check if filename exists
    bool filename_exists = std::ifstream(filename.c_str()).good();
    if (filename_exists)
    {
      std::cout << "Filename already exists.\n";
      std::cout << "Do you want to replace? (yes or no)\n";
      std::string yes_or_no;
      getline(std::cin, yes_or_no);
      while (yes_or_no.compare("yes") && yes_or_no.compare("no"))
      {
        std::cout << "Please answer yes or no! ";
        getline(std::cin, yes_or_no);
      }
      if (yes_or_no.compare("yes"))
      {
        std::string newfilename;
        std::cout << "Please enter new filename: ";
        getline(std::cin, newfilename);
        filename = newfilename.c_str();
      }
    }
    std::ofstream outputFile(filename.c_str());
    if (!outputFile.is_open())
    {
      std::cout << "Error opening " << filename.c_str() << "for write\n";
      return octave_value_list();
    }
    else
    {
      // writing header to file
      outputFile << "#\n# OBJ File generated by GNU Octave\n# using 'writeObj' function\n";
      outputFile << "#\n# Object " << filename.c_str() << "\n#\n";
      outputFile << "# Vertices: " << V_rows << "\n";
      outputFile << "# Faces: " << F_rows << "\n#\n#\n";
      int i = filename.length() - 3;
      std::string mtlfilename = filename.c_str();
      outputFile << "mtllib ./" << mtlfilename.replace(i,3, "mtl") <<"\n\n";
      std::cout << "Writing to file... ";
      // write vertices to file
      for(std::vector<Coord>::iterator v_it = vertex.begin(); v_it != vertex.end(); ++v_it)
      {
        outputFile << "v " << v_it->x <<" "<< v_it->y <<" "<< v_it->z <<"\n";
      }
      // write vertex normals to file
      for(std::vector<Coord>::iterator vn_it = vertex_normals.begin(); 
                  vn_it != vertex_normals.end(); ++vn_it)
      {
        outputFile << "vn " << vn_it->x <<" "<< vn_it->y <<" "<< vn_it->z <<"\n";
      }
      // write faces and face normals to file
      std::vector<Faces>::const_iterator f_it;
      std::vector<Faces>::const_iterator fn_it;
      for(f_it = face.begin(), fn_it = face_normals.begin(); f_it != face.end(),
                  fn_it != face_normals.end(); ++f_it, ++fn_it)
      {
        outputFile << "f " << f_it->a <<"//"<< fn_it->a <<" "<< f_it->b <<"//"
                   << fn_it->b <<" "<< f_it->c <<"//"<< fn_it->c <<"\n";
      }
      outputFile.close();
      std::cout << "done!\n";
    }
    ////
    ////
    ////
    ////
    std::cout << "Mesh filename is " << filename.c_str() << "\n";
    std::cout << "Mesh has " << V_rows << " vertices.\n";
    std::cout << "Mesh has " << F_rows << " faces.\n";
    return octave_value_list();
  }
  // for seven input arguments
  //
  // check for last argument being a string
  if (args.length() == 7 && !args(6).is_string())
  {
    std::cout << "Seventh input argument should be a string.\n";
    return octave_value_list();
  }
  // check for first six arguments being real matrices
  if (!args(0).is_matrix_type() || !args(1).is_matrix_type() || 
        !args(2).is_matrix_type() || !args(3).is_matrix_type() ||
        !args(4).is_matrix_type() || !args(5).is_matrix_type())
  {
    std::cout << "The first six arguments should be real matrices.\n";
    return octave_value_list();
  }
  // considering six input arguments being real matrices, the first two are for
  // vertices and faces respcectively, the next two are texture vertices and
  // texture faces, and the last two are vertex normals and face normals.
  // 
  // check for 7 input arguments with the last argument being a string
  if (args.length() == 7 && args(6).is_string())
  {
    // store vertices, faces and filename
    Matrix V = args(0).array_value();
    Matrix F = args(1).array_value();
    Matrix VT = args(2).array_value();
    Matrix FT = args(3).array_value();
    Matrix VN = args(4).array_value();
    Matrix FN = args(5).array_value();
    std::string filename = args(6).string_value();
    // find number of vertices and faces
    octave_idx_type V_rows = args(0).rows();
    octave_idx_type F_rows = args(1).rows();
    octave_idx_type VT_rows = args(2).rows();
    octave_idx_type FT_rows = args(3).rows();
    octave_idx_type VN_rows = args(4).rows();
    octave_idx_type FN_rows = args(5).rows();
    octave_idx_type V_columns = args(0).columns();
    octave_idx_type F_columns = args(1).columns();
    octave_idx_type VT_columns = args(2).columns();
    octave_idx_type FT_columns = args(3).columns();
    octave_idx_type VN_columns = args(4).columns();
    octave_idx_type FN_columns = args(5).columns();
    // ensure that there are at least 3 vertices and one face in the mesh and
    // vertex normals and face normals matrices are Nx3.
    if (V_rows < 3)
    {
      std::cout << "There should be at least 3 vertices in the mesh.\n";
      return octave_value_list();
    }
    if (V_columns != 3)
    {
      std::cout << "Vertex matrix should be Nx3 containing x,y,z coordinates.\n";
      return octave_value_list();
    }
    if (F_rows < 1)
    {
      std::cout << "There should be at least 1 face in the mesh.\n";
      return octave_value_list();
    }
    if (F_columns != 3)
    {
      std::cout << "Face matrix should be Nx3 containing three vertices.\n";
      return octave_value_list();
    }
    // check that texture coordinates are an Nx2 matrix
    if (VT_columns != 2)
    {
      std::cout << "Texture ccoordinates matrix should be an Nx2 matrix.\n";
      return octave_value_list();
    }
    // check that texture faces are an Nx3 matrix
    if (FT_columns != 3)
    {
      std::cout << "Texture faces matrix should be an Nx3 matrix.\n";
      return octave_value_list();
    }
    // check that vertex normals are an Nx3 matrix
    if (VN_columns != 3)
    {
      std::cout << "Vertex normals should be an Nx3 matrix.\n";
      return octave_value_list();
    }
    // check that face normals are an Nx3 matrix
    if (FN_columns != 3)
    {
      std::cout << "Face normals should be an Nx3 matrix.\n";
      return octave_value_list();
    }
    // define matrices for storing vertices, faces, texture coordinates,
    // texture faces, vertex normals and face normals to file
    std::vector<Coord> vertex;
    std::vector<Faces> face;
    std::vector<TCoord> texture_coord;
    std::vector<Faces> face_texture;
    std::vector<Coord> vertex_normals;
    std::vector<Faces> face_normals;
    // store vertices in a vector
    float tmpx, tmpy, tmpz;
    for (octave_idx_type i = 0; i < V_rows; i++)
    {
      tmpx = V(i,0);
      tmpy = V(i,1);
      tmpz = V(i,2);
      Coord temp_vertex = {tmpx, tmpy, tmpz};
      vertex.push_back(temp_vertex);
    }
    // store faces in a vector
    int tmpa, tmpb, tmpc;
    for (octave_idx_type i = 0; i < F_rows; i++)
    {
      tmpa = F(i,0);
      tmpb = F(i,1);
      tmpc = F(i,2);
      Faces temp_face = {tmpa, tmpb, tmpc};
      face.push_back(temp_face);
    }
    // store texture coordinates in a vector
    float tmpu, tmpv;
    for (octave_idx_type i = 0; i < VT_rows; i++)
    {
      tmpu = VT(i,0);
      tmpv = VT(i,1);
      TCoord temp_texture_coord = {tmpu, tmpv};
      texture_coord.push_back(temp_texture_coord);
    }
    // store texture faces in a vector
    for (octave_idx_type i = 0; i < FT_rows; i++)
    {
      tmpa = FT(i,0);
      tmpb = FT(i,1);
      tmpc = FT(i,2);
      Faces temp_texture_face = {tmpa, tmpb, tmpc};
      face_texture.push_back(temp_texture_face);
    }
    // store vertex normals in a vector
    for (octave_idx_type i = 0; i < VN_rows; i++)
    {
      tmpx = VN(i,0);
      tmpy = VN(i,1);
      tmpz = VN(i,2);
      Coord temp_vertex_normals = {tmpx, tmpy, tmpz};
      vertex_normals.push_back(temp_vertex_normals);
    }
    // store face normals in a vector
    for (octave_idx_type i = 0; i < FN_rows; i++)
    {
      tmpa = FN(i,0);
      tmpb = FN(i,1);
      tmpc = FN(i,2);
      Faces temp_face_normals = {tmpa, tmpb, tmpc};
      face_normals.push_back(temp_face_normals);
    }
    // check that matrices for faces, texture faces and face normals have
    // the same length to ensure that the iterator will not break when
    // writing to the file
    if (F_rows != FT_rows && F_rows != FN_rows)
    {
      std::cout << "Faces, texture faces and face normals should contain\n"
                << "the same number of values.\n";
      return octave_value_list();
    }
    ////
    ////
    // code for creating file and storing elements goes here!!!
    // vertices + faces + texture coordinates + texture faces
    // + vertex normals + face normals
    ////
    ////
    // check if filename exists
    bool filename_exists = std::ifstream(filename.c_str()).good();
    if (filename_exists)
    {
      std::cout << "Filename already exists.\n";
      std::cout << "Do you want to replace? (yes or no)\n";
      std::string yes_or_no;
      getline(std::cin, yes_or_no);
      while (yes_or_no.compare("yes") && yes_or_no.compare("no"))
      {
        std::cout << "Please answer yes or no! ";
        getline(std::cin, yes_or_no);
      }
      if (yes_or_no.compare("yes"))
      {
        std::string newfilename;
        std::cout << "Please enter new filename: ";
        getline(std::cin, newfilename);
        filename = newfilename.c_str();
      }
    }
    std::ofstream outputFile(filename.c_str());
    if (!outputFile.is_open())
    {
      std::cout << "Error opening " << filename.c_str() << "for write\n";
      return octave_value_list();
    }
    else
    {
      // writing header to file
      outputFile << "#\n# OBJ File generated by GNU Octave\n# using 'writeObj' function\n";
      outputFile << "#\n# Object " << filename.c_str() << "\n#\n";
      outputFile << "# Vertices: " << V_rows << "\n";
      outputFile << "# Faces: " << F_rows << "\n#\n#\n";
      int i = filename.length() - 3;
      std::string mtlfilename = filename.c_str();
      outputFile << "mtllib ./" << mtlfilename.replace(i,3, "mtl") <<"\n\n";
      std::cout << "Writing to file... ";
      // write vertices to file
      for(std::vector<Coord>::iterator v_it = vertex.begin(); v_it != vertex.end(); ++v_it)
      {
        outputFile << "v " << v_it->x <<" "<< v_it->y <<" "<< v_it->z <<"\n";
      }
      // write texture coordinates to file
      for(std::vector<TCoord>::iterator vt_it = texture_coord.begin(); 
                  vt_it != texture_coord.end(); ++vt_it)
      {
        outputFile << "vt " << vt_it->u <<" "<< vt_it->v <<"\n";
      }
      // write vertex normals to file
      for(std::vector<Coord>::iterator vn_it = vertex_normals.begin(); 
                  vn_it != vertex_normals.end(); ++vn_it)
      {
        outputFile << "vn " << vn_it->x <<" "<< vn_it->y <<" "<< vn_it->z <<"\n";
      }
      // write faces and face normals to file
      std::vector<Faces>::const_iterator f_it;
      std::vector<Faces>::const_iterator ft_it;
      std::vector<Faces>::const_iterator fn_it;
      for(f_it = face.begin(), ft_it = face_texture.begin(),
          fn_it = face_normals.begin(); f_it != face.end(),
          ft_it != face_texture.end(), fn_it != face_normals.end();
          ++f_it, ++ft_it, ++fn_it)
      {
        outputFile << "f " << f_it->a <<"/"<< ft_it->a <<"/"<< fn_it->a <<" "
                           << f_it->b <<"/"<< ft_it->b <<"/"<< fn_it->b <<" "
                           << f_it->c <<"/"<< ft_it->c <<"/"<< fn_it->c <<"\n";
      }
      outputFile.close();
      std::cout << "done!\n";
    }
    ////
    ////
    ////
    ////
    std::cout << "Mesh filename is " << filename.c_str() << "\n";
    std::cout << "Mesh has " << V_rows << " vertices.\n";
    std::cout << "Mesh has " << F_rows << " faces.\n";
    return octave_value_list();
  }
}
