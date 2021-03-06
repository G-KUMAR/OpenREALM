/**
* This file is part of OpenREALM.
*
* Copyright (C) 2018 Alexander Kern <laxnpander at gmail dot com> (Braunschweig University of Technology)
* For more information see <https://github.com/laxnpander/OpenREALM>
*
* OpenREALM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* OpenREALM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with OpenREALM. If not, see <http://www.gnu.org/licenses/>.
*/

#include <realm_io/realm_import.h>

using namespace realm;

camera::Pinhole io::loadCameraFromYaml(const std::string &directory, const std::string &filename)
{
  return loadCameraFromYaml(directory + "/" + filename);
}

camera::Pinhole io::loadCameraFromYaml(const std::string &filepath)
{
  // Identify camera model
  CameraSettings::Ptr settings = CameraSettingsFactory::load(filepath);

  // Load camera informations depending on model
  if (settings->get<std::string>("type") == "pinhole")
  {
    // Create pinhole model
    camera::Pinhole cam(settings->get<double>("fx"), settings->get<double>("fy"),
                        settings->get<double>("cx"), settings->get<double>("cy"),
                        (uint32_t)settings->get<int>("width"), (uint32_t)settings->get<int>("height"));
    cam.setDistortionMap(settings->get<double>("k1"), settings->get<double>("k2"),
                         settings->get<double>("p1"), settings->get<double>("p2"), 0.0);
    return cam;
  }
}

std::unordered_map<uint64_t, cv::Mat> io::loadTrajectoryFromTxtTUM(const std::string &directory,
                                                                   const std::string &filename)
{
  return io::loadTrajectoryFromTxtTUM(directory + "/" + filename);
};

std::unordered_map<uint64_t, cv::Mat> io::loadTrajectoryFromTxtTUM(const std::string &filepath)
{
  // Prepare result
  std::unordered_map<uint64_t, cv::Mat> result;

  // Open file
  std::ifstream file(filepath);
  if (!file.is_open())
    throw(std::runtime_error("Error loading trajectory file from '" + filepath + "': Could not open file!"));

  // Iterating through every line
  std::string str;
  while (std::getline(file, str))
  {
    // Tokenize input line
    std::vector<std::string> tokens = io::split(str.c_str(), ' ');
    if (tokens.size() < 7)
      throw(std::runtime_error("Error loading trajectory file from '\" + (directory+filename) + \"': Not enough arguments in line!"));

    // Convert all tokens to values
    uint64_t timestamp = std::stoul(tokens[0]);
    double x = std::stod(tokens[1]);
    double y = std::stod(tokens[2]);
    double z = std::stod(tokens[3]);
    double qx = std::stod(tokens[4]);
    double qy = std::stod(tokens[5]);
    double qz = std::stod(tokens[6]);
    double qw = std::stod(tokens[7]);

    // Convert Quaternions to Rotation matrix
    Eigen::Quaterniond quat(qw, qx, qy, qz);
    Eigen::Matrix3d R_eigen = quat.toRotationMatrix();

    // Pose as 3x4 matrix
    cv::Mat pose = (cv::Mat_<double>(3, 4) << R_eigen(0, 0), R_eigen(0, 1), R_eigen(0, 2), x,
                                              R_eigen(1, 0), R_eigen(1, 1), R_eigen(1, 2), y,
                                              R_eigen(2, 0), R_eigen(2, 1), R_eigen(2, 2), z);
    result[timestamp] = pose;
  }
  return result;
};

cv::Mat io::loadSurfacePointsFromTxt(const std::string &filepath)
{
  // Prepare result
  cv::Mat points;

  // Open file
  std::ifstream file(filepath);
  if (!file.is_open())
    throw(std::runtime_error("Error loading surface point file from '" + filepath + "': Could not open file!"));

  // Iterating through every line
  std::string str;
  while (std::getline(file, str))
  {
    // Tokenize input line
    std::vector<std::string> tokens = io::split(str.c_str(), ' ');
    if (tokens.size() < 2)
      throw(std::runtime_error("Error loading surface point file from '" + filepath + "': Not enough arguments in line!"));

    // Convert all tokens to values
    double x = std::stod(tokens[0]);
    double y = std::stod(tokens[1]);
    double z = std::stod(tokens[2]);

    // Point as 1x3 mat
    cv::Mat pt = (cv::Mat_<double>(1, 3) << x, y, z);
    points.push_back(pt);
  }
  return points;
}