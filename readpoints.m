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
%
function Point_coordinates = readpoints(samples,files)
  % function Point_coordinates = read_MeshlabPoints(filename)
  %
  % This function scans the working directory for any files associated with
  % Meshlab points and returns a NxM matrix containing all point 3D coordinates,
  % where N is the number of samples and M is the number of associated point
  % coordinates in the form of x1,y1,z1,x2,y2,z2,x3,y3,... 
  % Only files with a .pp extension are considered as Meshlabpoint files.
  % 
  % The user is expected to provide the number of samples and the number of
  % files associated per individual sample. The function will check that the
  % appropriate number of files exists before reading the point coordinates.
  % 
  % This function will only work if the point names saved in the .pp files
  % are integers. If the point names are alphanumeric, the user must use
  % the 'read_MeshlabPoints.m' function to read the files manually.

  % list the filenames of all the files in the working folder
  filenames = ls;

  % remove the files that do not contain any Meshlab points
  % only file with .pp extension are kept in th list
  for i = length(filenames):-1:1
    if isempty(strfind(filenames(i,:),".pp"))
      filenames(i,:) = [];
    endif
  endfor
  

  % Define the number of samples and the number of associated files per sample
  % samples = 20;
  % files = 10;

  % iterate over the files and store the point coorddinates in a table
  for i = 1:samples
    for j = 1:files
      MPL = read_MeshlabPoints(strcat(filenames(((i-1)*10)+j,:)));
      for k = 1:length(MPL)
        Point_number = MPL(k,1);
        Point_coordinates(i,[((Point_number-1) * 3) + 1:((Point_number-1) * 3) + 3]) = MPL(k,[2:4]);
      endfor
    endfor
  endfor
  
  % calculate the valid and missing landmarks and output a message
  valid_points = length(Point_coordinates(Point_coordinates!=0))/3;
  missing_points = length(Point_coordinates(Point_coordinates==0))/3;
  printf ("%d point 3D coordinates have been identified.\n", valid_points);
  printf ("%d point 3D coordinates are missing.\n", missing_points);
endfunction