//
// Created by tlou on 05.03.17.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>
#include <string>

#include "CommonVisualizer.hpp"

union Buf4Byte
{
  char _char[4];
  unsigned char _uchar[4];
  int _int;
  float _float;
};

float byte_to_float(const std::vector<unsigned char> &bytes, const int offset, const bool is_inv = true)
{
  assert(offset + 4 <= bytes.size());
  Buf4Byte buf;
  if(is_inv)
  {
    for(int i = 0; i < 4; ++i)
    {
      buf._uchar[3 - i] = bytes.at(offset + i);
    }
  }
  else
  {
    for(int i = 0; i < 4; ++i)
    {
      buf._uchar[i] = bytes.at(offset + i);
    }
  }
  return buf._float;
}

int byte_to_int(const std::vector<unsigned char> &bytes, const int offset, const bool is_inv = true)
{
  assert(offset + 4 <= bytes.size());
  Buf4Byte buf;
  if(is_inv)
  {
    for(int i = 0; i < 4; ++i)
    {
      buf._uchar[3 - i] = bytes.at(offset + i);
    }
  }
  else
  {
    for(int i = 0; i < 4; ++i)
    {
      buf._uchar[i] = bytes.at(offset + i);
    }
  }
  return buf._int;
}

std::string get_property(const std::string &line, const std::string &name)
{
  if(line.find(name) == std::string::npos)
  {
    return std::string("");
  }
  else
  {
    return line.substr(name.size());
  }
}

std::pair <std::vector<float>, std::vector<int>>
load_ply(const std::string &filename)
{
  std::vector<float> vertex;
  std::vector<int> indices;
  size_t num_vertex;
  size_t num_triangle;

  std::ifstream in;
  in.open(filename);
  if(!in.is_open())
  {
    return std::pair < std::vector < float > , std::vector < int >> (vertex, indices);
  }

  for(int i = 0; i < 100; ++i)
  {
    std::string str;
    std::getline(in, str);
    if(str == "end_header")
    {
      break;
    }
    else if(str.find("element vertex ") != std::string::npos)
    {
      num_vertex = std::stoi(get_property(str, "element vertex "));
    }
    else if(str.find("element face ") != std::string::npos)
    {
      num_triangle = std::stoi(get_property(str, "element face "));
    }
  }

  vertex.reserve(num_vertex);
  const int size_point = 4 * 4 + 3 * 1;
  std::vector<unsigned char> buffer(num_vertex * size_point, 0);
  in.read((char *) &buffer.front(), buffer.size());
  for(size_t i_ver = 0; i_ver < num_vertex; ++i_ver)
  {
    const size_t offset = i_ver * size_point;
    for(size_t i = 0; i < 3; ++i)
    {
      vertex.push_back(byte_to_float(buffer, offset + i * 4, false));
    }
  }

  indices.reserve(num_triangle);
  buffer.resize(num_triangle * 13);
  in.read((char *) &buffer.front(), buffer.size());
  for(size_t i_tri = 0; i_tri < num_triangle; ++i_tri)
  {
    const size_t offset = i_tri * 13 + 1;
    for(size_t i = 0; i < 3; ++i)
    {
      indices.push_back(byte_to_int(buffer, offset + i * 4, false));
    }
  }

  in.close();

  return std::pair < std::vector < float > , std::vector < int > > (vertex, indices);
}

loco::Vec get_minus_center(const std::vector<float> &positions)
{
  loco::Vec center{0.0f, 0.0f, 0.0f};
  for(size_t id = 0; id < positions.size(); id += 3)
  {
    center._x += positions.at(id);
    center._y += positions.at(id + 1);
    center._z += positions.at(id + 2);
  }
  for(int i = 0; i < 3; ++i)
  {
    center._data[i] /= -(float) (positions.size() / 3);
  }
  return center;
}

int main(int argc, char **argv)
{
  std::pair <std::vector<float>, std::vector<int>> mesh;
  mesh = load_ply("/home/tlou/workspace/navvis_data/poisson_trim_7/56_-24.ply");
  loco::CommonVisualizer vis;
  vis.setDistance(10.0f);
  vis.setLightSource(loco::Vec{0.0f, 0.0f, 200.0f}, loco::color::WHITE);
  vis.addMesh(mesh.first, mesh.second, loco::color::WHITE);
  vis.setTransform(loco::Transform{get_minus_center(mesh.first), loco::Vec{0.0f, 0.0f, 0.0f, 1.0f}});
  vis.play();
  return 0;
}