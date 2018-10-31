% Copyright (C) 2018 Andreas Bertsatos <andreas.bertsatos@gmail.com>
%
% This program is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free Software
% Foundation; either version 3 of the License, or (at your option) any later
% version.
%
% This program is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
% FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
% details.
%
% You should have received a copy of the GNU General Public License along with
% this program; if not, see <http://www.gnu.org/licenses/>.
%
function [varargout] = longbone_Scaling(varargin)
  % -*- texinfo -*-
  % @deftypefn  {} longbone_Scaling
  % @deftypefnx  {} longbone_Scaling(@var{filename})
  % @deftypefnx  {} {@var{scale} = longbone_Scaling
  % @deftypefnx  {} {@var{scale} = longbone_Scaling(@var{filename})
  %
  % This function reads the available triangular meshes stored in .obj format that
  % are present in the working directory and utilizes the 'longbone_maxDistance.m'
  % function to measure their maximum distance used for scaling and its corresponding
  % points, which are stored in a Meshlab .pp file for visualization purposes.
  %
  % The function may be used for scaling a single mesh model when input argument is
  % parsed into the function as a string variable. If no input argument is provided,
  % the function will scan the working directory for all .obj files and will scale
  % them iteratively in batch mode. For each file, the user will be prompted for the
  % real maximum distance of each corresponding long bone and it will subsequently
  % scale the model to the real world dimensions with units in mm. Scaling is performed
  % about the model's barycentric coordinates, which are translated to origin.
  %
  % Prior to scaling, when prompted for manual measurement value, the user may
  % examine the model's maximum distance corresponding points by opening the triangular
  % mesh in Meshlab and utilizing the corresponding .pp file, which is created
  % automatically by the present function. After scaling is performed for a given
  % triangular mesh, the user may re-examine the scaled model by reloading the mesh
  % and its respective .pp file, which is automatically updated with the new points
  % of its maximum distance.
  %
  % The user may also declare an output argument, which will be returned as a cell array
  % containing each model's filename and scale related measurements in the following order:
  % "filename", "ratio", "oldMaxD", "newMaxD". If no output argument is declared, the
  % function will prompt the user to specify a .csv file (new or existing, which will
  % be overwritten) that all scaling measurements will be saved. The first row in the
  % csv file will contain the column labels "filename", "ratio", "oldMaxD", "newMaxD",
  % which are also saved in @var{scale(1,[1:4])}, when output variable is declared by
  % calling the function as @var{scale} = longbone_Scaling.
  %
  % The present function

  % load required packages
  pkg load statistics
  pkg load geometry
  pkg load io;
  
  % check for valid number of input variables
  if nargin ~= 0 && nargin ~= 1
    printf("invalid number of input arguments\n");
    return;
  endif
  % check if input argument is provided. It must be a string
  if nargin == 1
    % it must be string
    if !ischar(varargin{1}(:)')
      printf("Mesh object filename must be a string\n");
      return;
    endif
    filenames(1).name = varargin{1}(:)';
  else
    % list the filenames with .obj extension in the working folder
    filenames = dir("*.obj");
  endif
  
  % initialize header for the cell array
  scale = {"filename", "ratio", "oldMaxD", "newMaxD"};
  % perform scaling for each mesh object present in the working directory
  for i = 1:length(filenames)
    % store filename of current mesh
    filename = strcat(filenames(i).name);
    % read obj elements
    [v,f,vt,ft,vn,fn,name] = readObj(filename);
    % calculate maximum distance and corresponding points
    [maxD, p1, p2] = longbone_maxDistance(v);
    % save maxD points to Meshlab .pp file using the name convention of the
    % original mesh obj file
    filenamePP = filename([1:length(filename)-4]);
    extension = ".pp";
    filenamePP = strcat(filenamePP, extension);
    write_MeshlabPoints(filenamePP, filename, [p1; p2]);
    % ask user for real maximum distance value
    realMaxD = inputdlg ("maximum distance in mm", filename, [1,20]);
    RmaxD = str2num(cell2mat(realMaxD));
    % calculate scaling ratio
    ratio = RmaxD / maxD;
    % find barycentric coordinates and translate mesh' origin
    origin = meshBarycenter(v, f);
    v = v - origin;
    % scale mesh
    v = v * ratio;
    % save scaled model
    writeObj(v,f,vt,ft,vn,fn,filename);
    mtl = readMtl(name);
    filenameMTL = filename([1:length(filename)-4]);
    extension = ".mtl";
    filenameMTL = strcat(filenameMTL, extension);
    writeMtl(mtl,filenameMTL);
    % calculate scaled model's max distance and save its points in .pp file
    [maxDnew, p1, p2] = longbone_maxDistance(v);
    write_MeshlabPoints(filenamePP, filename, [p1; p2]);
    % save scaling ratio and related measurements for each mesh object in
    % a data structure
    scale{i+1,1} = filename;
    scale{i+1,2} = ratio;
    scale{i+1,3} = maxD;
    scale{i+1,4} = maxDnew;
    % flush the screen output to display the results during iterations through
    % multiple mesh files
    page_screen_output(0);
    page_output_immediately(1);
    % display ratio and measurements for the current mesh object
    printf("Model %s was scaled by %f.\n", scale{i+1,1}, scale{i+1,2});
    printf("Old max distance was %f. New max distance is %f.\n\n", ...
            scale{i+1,3}, scale{i+1,4});
  endfor
  if (nargout==0)
    % ask user for filename to save scaling values to csv file
    csv_filename = uiputfile({'*.csv', 'Supported Formats'});
    cell2csv(csv_filename, scale);
  elseif (nargout==1)
    varargout{1} = scale;
  endif
endfunction




