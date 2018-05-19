#ifndef MESH50M_H
#define MESH50M_H

#include <stdio.h>
#include <new>
#include <math.h>
#include <vector>
#include <map>
#include <string>

typedef struct{
  // height[longitude(x)][latitude(y)]
  double ** height;
  double base_longitude, base_latitude;
  // == x*y then fully registered
  int count;
  char meshID[256];
} Mesh50M_E;

// Mesh50M_E�Υ�å��奵����(size_t) ���ޤ��ͤ�Ŭ���˸��Ĥ���
#define size_longitude 20
#define size_latitude 20

// 1��å��岿�ä�
// 50m ~ 2��...1�٤�1800��ʬ
#define mesh_longitude 2
#define mesh_latitude 2

typedef std::vector<std::map<std::pair<double,double>, Mesh50M_E>::iterator> MeshIV;
typedef MeshIV::iterator MeshIVI;

class Mesh50M
{
 private:
  // std::pair<double,double>(base_longitude,base_latitude)��key
  int meshcode1(double longitude, double latitude);
  
  double predict_base_longitude(double longitude);
  double predict_base_latitude(double latitude);
  void register_point(double * key, double height, double longitude, double latitude);
  
 public:
  std::map<std::pair<double,double>, Mesh50M_E> meshes;
  int new_mesh(Mesh50M_E * msh);
  void delete_mesh(Mesh50M_E * msh);
  int is_complete_mesh(Mesh50M_E * msh);
  void add_point(double height, double longitude, double latitude);
  MeshIV get_meshes(double left_longitude, double right_latitude,
		   double top_longitude, double bottom_latitude);
  
};

#endif
