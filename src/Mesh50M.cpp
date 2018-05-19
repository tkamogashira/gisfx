#include "Mesh50M.h"

// 経度緯度とも秒
int meshcode1(double longitude, double latitude)
{
  return (int)ceil(latitude/3600*1.5) * 100 + (int)(longitude/3600) % 100;
}

double Mesh50M::predict_base_longitude(double longitude)
{
  return floor(longitude/(size_longitude*mesh_longitude))
    *(size_longitude*mesh_longitude);
}

double Mesh50M::predict_base_latitude(double latitude)
{
  return floor(latitude/(size_latitude*mesh_latitude))
    *(size_latitude*mesh_latitude);
}

int Mesh50M::new_mesh(Mesh50M_E * msh)
{
  int i = 0;
  msh->count = 0;
  msh->height = new double *[size_longitude+1];
  try
    {
      for(i = 0;i < size_longitude+1;i ++)
	{
	  msh->height[i] = new double[size_latitude+1];
	  for(size_t j = 0;j < size_latitude+1;j ++)
	    {
	      msh->height[i][j] = 0.0;
	    }
	}
    }
  catch(std::bad_alloc)
    {
      for(i --;i >= 0;i --)
	{
	  delete[] msh->height[i];
	}
      delete[] msh->height;
      return -1;
    }
  return 0;
}

void Mesh50M::delete_mesh(Mesh50M_E * msh)
{
  for(int i = size_longitude+1;i >= 0;i --)
    {
      delete[] msh->height[i];
    }
  delete[] msh->height;
}

// keyを持つMesh50M_Eに格納
void Mesh50M::register_point(double * key,
		    double height, double longitude, double latitude)
{
  std::map<std::pair<double,double>, Mesh50M_E>::iterator mshi =
    meshes.find(std::pair<double,double>(key[0],key[1]));
  
  if(mshi == meshes.end())
    {
      fprintf(stderr, "New Mesh[%f|%f]\n", key[0], key[1]);
      fprintf(stderr, "point: [%d|%d]\n",
	      (int)((longitude-key[0])/mesh_longitude),
	      (int)((latitude-key[1])/mesh_latitude));
      
      Mesh50M_E msh;
      msh.base_longitude = key[0];
      msh.base_latitude = key[1];
      sprintf(msh.meshID, "[%f|%f]", key[0], key[1]);
      new_mesh(&msh);
      msh.height[(int)(longitude-key[0])/mesh_longitude]
	[(int)(latitude-key[1])/mesh_latitude] = height;
      msh.count ++;
      meshes.insert(std::pair<std::pair<double,double>,Mesh50M_E>
		    (std::pair<double,double>(key[0],key[1]), msh)
		    );
    }
  else
    {
      mshi->second.height[(int)(longitude-key[0])/mesh_longitude]
	[(int)(latitude-key[1])/mesh_latitude] = height;
      mshi->second.count ++;
    }
}

void Mesh50M::add_point(double height, double longitude, double latitude)
{
  // 座標はすべて奇数らしいので偶数に切り下げ
  longitude = floor(longitude/mesh_longitude)*mesh_longitude;
  latitude = floor(latitude/mesh_latitude)*mesh_latitude;
  
  double xy[2];
  xy[0] = predict_base_longitude(longitude);
  xy[1] = predict_base_latitude(latitude);
  
  register_point(xy, height, longitude, latitude);
  
  // 境界上の点は両方のMesh50M_Eに登録
  if(xy[0] == longitude)
    {
      double xyo[2];
      xyo[0] = predict_base_longitude(longitude-1);
      xyo[1] = predict_base_latitude(latitude);
      register_point(xyo, height, longitude, latitude);
    }
  
  if(xy[1] == latitude)
    {
      double xyo[2];
      xyo[0] = predict_base_longitude(longitude);
      xyo[1] = predict_base_latitude(latitude-1);
      register_point(xyo, height, longitude, latitude);
    }
  
  if(xy[0] == longitude && xy[1] == latitude)
    {
      double xyo[2];
      xyo[0] = predict_base_longitude(longitude-1);
      xyo[1] = predict_base_latitude(latitude-1);
      register_point(xyo, height, longitude, latitude);
    }
}

int Mesh50M::is_complete_mesh(Mesh50M_E * msh)
{
  //fprintf(stderr, "[%d]", msh->count);
  return (msh->count) - ((size_longitude+1)*(size_latitude+1));
}

MeshIV Mesh50M::get_meshes(double left_longitude, double right_longitude,
					double top_latitude, double bottom_latitude)
{
  std::vector<std::map<std::pair<double,double>, Mesh50M_E>::iterator> meshesInRange;
  double p_L_longitude = predict_base_longitude(left_longitude);
  double p_R_longitude = predict_base_longitude(right_longitude);
  double p_T_latitude = predict_base_latitude(top_latitude);
  double p_B_latitude = predict_base_latitude(bottom_latitude);
  
  for(double lo = p_L_longitude;lo <= p_R_longitude;lo += (size_longitude*mesh_longitude))
    {
      for(double la = p_B_latitude;la <= p_T_latitude;la += (size_latitude*mesh_latitude))
	{
	  std::map<std::pair<double,double>, Mesh50M_E>::iterator mshi =
	    meshes.find(std::pair<double,double>(lo,la));
	  // New Mesh[503000.000000|128800.000000]
	  if(mshi != meshes.end()) // exist
	    {
	      meshesInRange.push_back(mshi);
	    }
	}
    }
  return meshesInRange;
}
