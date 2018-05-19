#include "SalFile.h"

#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "libarc/arc.h"

// .slm
/*
  5024960000,1281240000(vertex)
  3760000,3360000(range)
  60162(.slp)
  18719(mh.slp)
  数値地図２５０００（空間データ基盤）
  国土交通省国土地理院
  20011001
  20011001
  kukandata.dtd
  2,2,s
*/

URL urlOpen(std::string filename)
{
  URL url;
  // std::cout << "urlOpen(" << filename << ")" << std::endl;
  if(strchr((char *)filename.c_str(), '#') != NULL) {
    /* archive file */
    url = url_arc_open((char *)filename.c_str());
  } else {
    /* normal file */
    url = url_open((char *)filename.c_str());
  }
  return url;
}

void urlClose(URL url)
{
  url_close(url);
}

char * urlGets(char *s, int size, URL stream)
{
  return url_gets(stream, s, size);
}

int SalFile::loadSLM(std::string filename)
{
  char line[10][SIZE_OF_LINE];
  //FILE * fp;
  URL url;
  
  //if((fp = fopen(filename.c_str(), "rt")) == NULL)
  //return -1;
  
  if((url = urlOpen(filename)) == NULL)
    return -1;
  
  for (int i = 0;i < 10;i ++)
    {
      //if(fgets(line[i], sizeof(line), fp) == NULL)
      //return -1;
      if(urlGets(line[i], sizeof(line), url) == NULL)
	return -1;
    }

  sscanf(line[0], "%lld,%lld", &point_start_x, &point_start_y);
  sscanf(line[1], "%ld,%ld", &point_range_x, &point_range_y);
  sscanf(line[2], "%ld", &point_total);
  sscanf(line[3], "%ld", &point_mh);
  
  urlClose(url);
  return 0;
}

// .slp

int SalFile::loadSLP(std::string filename, Point * slpList, int max)
{
  char line[SIZE_OF_LINE];
  //FILE * fp;
  URL url;
  int count = 0;
  
  //if((fp = fopen(filename.c_str(), "rt")) == NULL)
  if((url = urlOpen(filename)) == NULL)
    return -1;
  
  while (1)
    {
      Point LL;
      if(urlGets(line, sizeof(line), url) == NULL)
	break;
      // 0022920002642800
      // XXXXXXXXYYYYYYYY
      sscanf(line, "%08lf%08lf", &(LL.longitude), &(LL.latitude));
      LL.longitude += point_start_x;
      LL.latitude += point_start_y;
      slpList[count] = LL;
      count ++;
      if(count > max)
	return -1;
    }

  //fclose(fp);
  urlClose(url);
  return 0;
}

// .sal
/*
  DK(ID{DK002064}){
  JT{11}
  YU{1D}
  SB{13}
  FI{19}
  NM{}
  CV(ID{CV002197}){003310,003503}
  EG(ID{EG000001}){
  BD(IR{ND13112000420})
  BD(IR{ND13112000443})
  GM(IR{CV13112002197})
  }
  }
*/

int SalFile::searchAttribute(char * str, xmlNode * node)
{
  int i = 0, c = 0, id = 0;
  while(1)
    {
      // Shift-JIS
      if(((uchar)str[i] >= 0x81&&(uchar)str[i] <= 0x9f)
	 ||((uchar)str[i] >= 0xe0&&(uchar)str[i] <= 0xfc))
	{
	  i += 2;
	  continue;
	}
      if(str[i] == '{')
	{
	  id = i;
	}
      if(str[i] == '}')
	{
	  xmlSetProp(node,
		     (xmlChar*)std::string(str, c, (id-c)).c_str(),
		     (xmlChar*)std::string(str, id+1, (i-id-1)).c_str());
	  c = i; id = i;
	}
      if(str[i] == ')')
	{
	  i++;
	  break;
	}
      i ++;
    }
  return i;
}

int SalFile::buildNode(char * str, xmlNode * node)
{
  int i = 0, cursor = 0;
  int attron = 0; // 0... ID{} format 1... ID(){} format
  std::string name = std::string("");
  
  xmlNodePtr child;

  while(1)
    {
      // Shift-JIS
      if(((uchar)str[i] >= 0x81&&(uchar)str[i] <= 0x9f)
	 ||((uchar)str[i] >= 0xe0&&(uchar)str[i] <= 0xfc))
	{
	  i += 2;
	  continue;
	}
      
      // (内はID{}のフォーマットしか受け付けないはず。)
      if(str[i] == '(')
	{
	  attron = 1;
	  // '('以前まで -> Node名
	  child = xmlNewChild(node, NULL,
			      (xmlChar*)std::string(str,cursor,(i-cursor)).c_str(), NULL);
	  // ID{}を探索
	  i += (searchAttribute(&(str[i+1]), child) + 1);
	  cursor = i;
	  continue;
	}
      
      if(str[i] == '{')
	{
	  // ID(){}フォーマットではNode名、attrは処理済み。(attron=1)
	  // {}内は大きなTreeを作る可能性あり。
	  if(attron == 0)
	    {
	      child = xmlNewChild(node, NULL,
				  (xmlChar*)std::string(str,cursor,(i-cursor)).c_str(), NULL);
	    }
	  // {}内ツリー探索
	  i += (buildNode(&(str[i+1]), child) + 1);
	  cursor = i;
	  continue;
	}
      
      // ()や{}に出会わなかったらそのまま保存
      if(str[i] == '}'|| str[i] == '\0'||str[i] == '\r'||str[i] == '\n')
	{
	  std::string content = std::string(str, cursor, (i-cursor));
	  if(content.length() != 0)
	    {
	      xmlNodeAddContent(node,
				(xmlChar*)std::string(str, cursor, (i-cursor)).c_str());
	    }
	  i ++;
	  break;
	}      
      i ++;
    }
  return i;
}

int SalFile::loadSAL(std::string filename,
		     std::vector<Element> * element,
		     Point * slpList)
{
  char line[SIZE_OF_LINE];
  //FILE * fp;
  URL url;
  
  //if((fp = fopen(filename.c_str(), "rt")) == NULL)
  if((url = urlOpen(filename)) == NULL)
    return 0;
  
  while (1)
    {
      Element elm;
      if(urlGets(line, sizeof(line), url) == NULL)
	break;
      buildNode(line, doc->children);
    }
  
  urlClose(url);
  return 0;
}

SalFile::SalFile(char * headername)
{
  available = 0;

  doc = xmlNewDoc((xmlChar*)"1.0");
  doc->children = xmlNewDocNode(doc, NULL, (xmlChar*)"SAL", NULL);

  if(load(headername) != 0)
    exit(-1);

  //xmlDocDump(stdout, doc);

  return;
}

int SalFile::load(char * headername)
{
  
  fprintf(stderr, "SalFile::load(%s)\n", headername);
  fprintf(stderr, "[slm]");
  if(loadSLM(std::string(headername) + std::string(".slm")) < 0)
    return -1;
  fprintf(stderr, "%lld,%lld[%d,%d]%d(%d)\n",
	  point_start_x, point_start_y, point_range_x, point_range_y,
	  point_total, point_mh);
  
  slp_ = new Point[point_total];
  mhslp_ = new Point[point_mh];
  available = 1;

  // mesh
  fprintf(stderr, "[slp]");
  if(loadSLP(std::string(headername) + std::string("mh.slp"), mhslp_, point_mh) < 0)
    return -1;
  fprintf(stderr, "[mh]");
  if(loadSAL(std::string(headername) + std::string("mh.sal"), &mhsal, slp_) < 0)
    return -1;
  
  // general
  fprintf(stderr, "[slp]");
  if(loadSLP(std::string(headername) + std::string(".slp"), slp_, point_total) < 0)
    return -1;
  char salList[][3] = {"cm", "dk", "ds", "ek", "gd", "gk", "ha", "kj", "kk", "ko", "ks", "sk", "tk", "to", "ts", "yo", "\0"};
  // char salList[][3] = {"dk", "tk", "\0"};
  
  for(int list = 0;salList[list][0] != '\0';list ++)
    {
      fprintf(stderr, "[%s]", salList[list]);
      if(loadSAL(std::string(headername) + std::string(salList[list]) +
		 std::string(".sal"), &cmsal, mhslp_) < 0)
	return -1;
    }

  fprintf(stderr, ".\n");
  return 0;
}

SalFile::~SalFile()
{
  xmlFreeDoc(doc);	
  if(available != 0)
    {
      delete[] slp_;
      delete[] mhslp_;
    }
  return;
}
