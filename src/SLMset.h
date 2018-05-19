#ifndef SLMSET_H
#define SLMSET_H

#include <vector>
#include <string>
#include <map>
#include <math.h>
#include <libxml/tree.h>
#include "Data.h"

typedef struct{
  long long int point_start_x, point_start_y;
  long int point_range_x, point_range_y;
  long int point_total, point_mh;
  std::string pref, city_number, j_pref, j_city;
  int loaded;
} SLM;

class SLMset
{
 private:
  std::map<int,SLM> slmSet;
  int loadSLMDAT(std::string filename);
  void registerSLM(int city_number, SLM slm);
  void registerSLM(SLM slm);
  int isOverlapped(double amin, double amax, double bmin, double bmax);
 public:
  SLMset();
  SLMset(char * dataname);
  int cityNumber(char * city_number);
  void slmLoaded(int city_number);
  void slmUnloaded(int city_number);
  int isSLMLoaded(int city_number);
  SLM * searchCity(int city_number);
  std::vector<SLM> searchSLM(double leftLongitude, double rightLongitude,
			     double topLatitude, double bottomLatitude);
};

#endif
