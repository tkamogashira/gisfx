#ifndef SALFILE_H
#define SALFILE_H

#include <iostream>
#include <vector>
#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include "Data.h"

class SalFile
{
 private:
  // normal slp, mesh slp
  int available;
  
  int loadSLM(std::string filename);
  int loadSLP(std::string filename, Point * slpList, int max);
  int loadSAL(std::string filename, std::vector<Element> * element, Point * slpList);
  int buildNode(char * str, xmlNode * node);
  int searchAttribute(char * str, xmlNode * node);

 public:
  // slm
  long long int point_start_x, point_start_y;
  long int point_range_x, point_range_y;
  long int point_total, point_mh;
  
  std::vector<Element> mhsal, cmsal;
  Point * slp_;
  Point * mhslp_;

  xmlDocPtr doc;

  SalFile(char * headername);
  ~SalFile();
  int load(char * headername);
};

#endif
