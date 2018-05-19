// Read .e00 file

#include "GISFile.h"
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

GISFile::GISFile(char * filename)
{
  if(loadFile(filename) != 0)
    exit(-1);
  return;
}

int GISFile::loadFile(char * filename)
{
  char line[512];
  int count = 0;
  FILE * fp;
  struct stat file_status;
  ReadStatus readStatus = OUT, dataStatus = OUT;
  Precision precision = DOUBLE;
  int inData = 0;

  fprintf(stderr, "GISFile::loadFile(): %s", filename);
  
  if((fp = fopen(filename, "rt")) == NULL)
    return -1;
  
  while (1)
    {
      if (fgets(line, sizeof(line), fp) == NULL)
	break;
      readStatus = OUT;

      // determine precision (no error check)
      if(line[5] == '2')
	precision = SINGLE;
      else if (line[5] == '3')
	precision = DOUBLE;
      
      if(strlen(line) < 3)
	;
      else if(strncmp(line, "EXP", 3) == 0)
	{
	  readStatus = EXP;
	  fprintf(stderr, "[EXP]");
	}
      else if(strncmp(line, "ARC", 3) == 0)
	{
	  readStatus = ARC;
	  fprintf(stderr, "[ARC]");
	  while(1)
	    {
	      int i0, i1, i2, i3, i4, i5, i6;
	      ARCPolygon arcpolygon;
	      
	      //read texture head
	      if (fgets(line, sizeof(line), fp) == NULL)
		return -1;
	      sscanf(line, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
		     &i0, &i1, &i2, &i3, &i4, &i5, &i6);
	      //fprintf(stderr,"%d|%d|%d|%d|%d|%d|%d\n",
	      // i0, i1, i2, i3, i4, i5, i6);
	      arcpolygon.coverage = i0; arcpolygon.coverage_ID = i1;
	      arcpolygon.from_node = i2; arcpolygon.to_node = i3;
	      arcpolygon.left_polygon = i4; arcpolygon.right_polygon = i5;
	      arcpolygon.number_of_coordinates = i6;
	      
	      if (arcpolygon.coverage == -1)
		{
		  goto ARC_END;
		}
	      
	      // read vertex...
	      int i;
	      for(i = 0;i < i6;i ++)
		{
		  double x, y, x1, y1;
		  if (fgets(line, sizeof(line), fp) == NULL)
		    return -1;
		  if(precision == DOUBLE)
		    {
		      sscanf(line, " %le %le", &x, &y);
		      //fprintf(stderr, "%lf, %lf\n", x, y);
		      arcpolygon.vVertexX.push_back(x);
		      arcpolygon.vVertexY.push_back(y);
		    }
		  else if(precision == DOUBLE)
		    {
		      sscanf(line, " %e %e %e %e", &x, &y, &x1, &y1);
		      //fprintf(stderr, "%le_%le\n", x, y);
		      arcpolygon.vVertexX.push_back(x);
		      arcpolygon.vVertexY.push_back(y);
		      arcpolygon.vVertexX.push_back(x1);
		      arcpolygon.vVertexY.push_back(y1);
		    }
		}
	      vARC.push_back(arcpolygon);
	    }
	ARC_END:
	  ;
	}
      else if(strncmp(line, "CNT", 3) == 0)
	readStatus = CNT;
      else if(strncmp(line, "LAB", 3) == 0)
	readStatus = LAB;
      else if(strncmp(line, "PAL", 3) == 0)
	readStatus = PAL;
      else if(strncmp(line, "TOL", 3) == 0)
	readStatus = TOL;
      else if(strncmp(line, "SIN", 3) == 0)
	readStatus = SIN;
      else if(strncmp(line, "LOG", 3) == 0)
	readStatus = LOG;
      else if(strncmp(line, "PRJ", 3) == 0)
	readStatus = PRJ;
      else if(strncmp(line, "IFO", 3) == 0)
	readStatus = IFO;
      else
	{
	  dataStatus = readStatus;
	  readStatus = DATA;
	}
      
      readStatus = OUT;      
      count ++;
    }
  fclose(fp);

  fprintf(stderr, "\n");
  return 0;
}
