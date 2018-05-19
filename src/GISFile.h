#ifndef GISFILE_H
#define GISFILE_H

#include <iostream>
#include <vector>
#include <string>
#include "Data.h"

enum ReadStatus{OUT, DATA, EXP,
		ARC, CNT, LAB, PAL, TOL, SIN, LOG, PRJ, IFO};
enum Precision{SINGLE, DOUBLE};

typedef struct{
  int coverage, coverage_ID, from_node, to_node,
    left_polygon, right_polygon, number_of_coordinates;
  std::vector<double> vVertexX;
  std::vector<double> vVertexY;
} ARCPolygon;

class GISFile
{
 private:
 public:
  GISFile(char * filename);
  int loadFile(char * filename);
  std::vector<ARCPolygon> vARC;
};

#endif
