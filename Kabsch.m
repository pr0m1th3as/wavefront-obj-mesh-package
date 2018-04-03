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
function [R, T, LRMSD, point_dev] = Kabsch(A, B)
  % function [U, T, LRMSD, point_dev] = Kabsch(A, B)
  %
  % This function computes the translation vector T required to translate both
  % sets of coordinates so that centroid coincides with the origin of the
  % coordinate system. It subsequently applies the Kabsch algorithm to calculate
  % the optimal rotation matrix that minimizes RMSD (root mean squared deviation).
  %
  % The function requires two sets of coordinates, namely A and B, where rows
  % correspond to points and columns to dimensions. There must be at least three
  % points and two dimensions in the data sets, whose points should be paired.
  % That is all corresponding points must have the same row index between the
  % two input matrices.
  %
  % The function returns a rotation matrix U, a translation vector T and the
  % least root mean squared deviation LRMSD. It also returns the distances
  % between each paired points after the transformation of the two point
  % configurations has been applied. Rotation matrix U is DxD sized and translation
  % vector is 1xD sized, where D is the dimensions of the two original point
  % configurations. LRMSD is a scalar, whereas individual point deviations is
  % returned as a column vector sized according to the number of points of the
  % original point configurations.
  %
  % The rotation and translations matrices are foolowing the tranformation of
  % point configuration A to configuration B and not the opposite. So, for
  % example, let A and B be Nx3 matrices of two identical 3-dimensional
  % configurations of N points, which have different orientation and position
  % in space. Apply,
  %
  % [R, T, LRMSD, point_dev] = Kabsch(A, B);
  %
  % Then A * R + T will produce B, while LRMSD and point_dev will equal 0.
  
  % check that input data sets contain at least 3 points in 2 dimensions
  % and share a common size
  if (length(size(A)) ~= 2 || length(size(B)) ~= 2)
		error 'A and B must be matrices' ;
	endif
	if (any(size(A) ~= size(B)))
		error 'A and B must be of same size' ;
	endif
  D = length(A(1,:));   % number of dimensions
  N = length(A(:,1));   % number of points
  if (D<2)
    error 'There should be at least two dimensional space';
  endif
  if (N<D)
    error 'There should be at least as many paired points as space dimensions';
  endif
  
  % calculate the centroids of both configurations
  A_centroid = mean(A);
  B_centroid = mean(B);
  % translate both configurations to space origin
  A_origin = A - ones(N,1) * A_centroid;
  B_origin = B - ones(N,1) * B_centroid;
  % calculate the variance-covariance matrix of the two point configurations
  % and apply singular value decomposition to obtain rotation and translation
  % matrices
  CovMat = A_origin' * B_origin;
  [V, S, W] = svd(CovMat);
  R = V * W';
  if (det(R) < 0)       % correct for reflections
    I = eye(D);
    I(D,D) = -1;
    R = V * I * W';
  endif
  T = B_centroid - A_centroid * R;      % find translation vector
  
  % apply transformation to measure LRMSD and paired point deviations
  % betweeen the two point configurations. Since both configuration have
  % been centered to space origin no translation is required for this step
  error = A_origin * R - B_origin;
  point_dev = sqrt(sum(error.^2,2));
  LRMSD = mean(point_dev);
endfunction




  