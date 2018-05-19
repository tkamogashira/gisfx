#include "SLMset.h"

#define MAX(x,y) (x>y?x:y)
#define MIN(x,y) (x<y?x:y)

int SLMset::loadSLMDAT(std::string filename)
{
  char line[SIZE_OF_LINE];
  FILE * fp;
  if((fp = fopen(filename.c_str(), "rt")) == NULL)
    return -1;

  while (1)
    {
      SLM slm;
      char pref[SIZE_OF_LINE], city_number[SIZE_OF_LINE],
	j_pref[SIZE_OF_LINE], j_city[SIZE_OF_LINE];
      
      if(fgets(line, sizeof(line), fp) == NULL)
	break;
      sscanf(line, "%s %s %lld %lld %ld %ld %ld %ld %s %s",
	     pref, city_number,
	     &slm.point_start_x, &slm.point_start_y,
	     &slm.point_range_x, &slm.point_range_y,
	     &slm.point_total, &slm.point_mh,
	     j_pref, j_city);
      slm.pref = std::string(pref);
      slm.city_number = std::string(city_number);
      slm.j_pref = std::string(j_pref);
      slm.j_city = std::string(j_city);
      slm.loaded = 0;
      registerSLM(slm);
    }
  return 0;
}

void SLMset::registerSLM(int city_number, SLM slm)
{
  slmSet.insert(std::pair<int,SLM>(city_number,slm));
}

void SLMset::registerSLM(SLM slm)
{
  int city_number;
  sscanf(slm.city_number.c_str(), "%d", &city_number);
  registerSLM(city_number, slm);
}

std::vector<SLM> SLMset::searchSLM(double leftLongitude, double rightLongitude,
			     double topLatitude, double bottomLatitude)
{
  // leftLongitude < rightLongitude
  // topLatitude > bottomLatitude
  std::vector<SLM> result;
  for(std::map<int,SLM>::iterator i = slmSet.begin();i != slmSet.end();i ++)
    {
      if(isOverlapped(leftLongitude, rightLongitude,
		      (double)i->second.point_start_x/10000,
		      (double)i->second.point_start_x/10000 + (double)i->second.point_range_x/10000) == 1
	 &&
	 isOverlapped(bottomLatitude, topLatitude,
		      (double)i->second.point_start_y/10000,
		      (double)i->second.point_start_y/10000 + (double)i->second.point_range_y/10000) == 1)
	{
	  result.push_back(i->second);
	}
    }
  return result;
}

int SLMset::cityNumber(char * city_number)
{
  int c_number;
  sscanf(city_number, "%d", &c_number);
  return c_number;
}

SLM * SLMset::searchCity(int city_number)
{
  std::map<int,SLM>::iterator i = slmSet.find(city_number);
  if(i != slmSet.end())
    {
      return &(i->second);
    }
  return NULL;
}

void SLMset::slmLoaded(int city_number)
{
  SLM * slm;
  if((slm = searchCity(city_number)) != NULL)
    {
      slm->loaded = 1;
    }
}

void SLMset::slmUnloaded(int city_number)
{
  SLM * slm;
  if((slm = searchCity(city_number)) != NULL)
    {
      slm->loaded = 0;
    }
}

int SLMset::isSLMLoaded(int city_number)
{
  SLM * slm;
  if((slm = searchCity(city_number)) != NULL)
    {
      return slm->loaded;
    }
  return -1;
}

int SLMset::isOverlapped(double amin, double amax, double bmin, double bmax)
{
  if(MAX(amin, bmin) < MIN(amax, bmax))
    {
      return 1;
    }
  return 0;
}

SLMset::SLMset()
{
  ;
}

SLMset::SLMset(char * datname)
{
  SLMset();
  loadSLMDAT(std::string(datname));
}
