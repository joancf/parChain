// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2011 Guy Blelloch and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#ifndef _BENCH_GEOMETRY_IO
#define _BENCH_GEOMETRY_IO
#include <string>
#include "IO.h"
#include "parallel.h"
#include "geometry.h"
using namespace benchIO;

inline int xToStringLen(point2d a) { 
  return xToStringLen(a.x) + xToStringLen(a.y) + 1;
}

inline void xToString(char* s, point2d a) { 
  int l = xToStringLen(a.x);
  xToString(s, a.x);
  s[l] = ' ';
  xToString(s+l+1, a.y);
}

inline int xToStringLen(point3d a) { 
  return xToStringLen(a.x) + xToStringLen(a.y) + xToStringLen(a.z) + 2;
}

inline void xToString(char* s, point3d a) { 
  int lx = xToStringLen(a.x);
  int ly = xToStringLen(a.y);
  xToString(s, a.x);
  s[lx] = ' ';
  xToString(s+lx+1, a.y);
  s[lx+ly+1] = ' ';
  xToString(s+lx+ly+2, a.z);
}

inline int xToStringLen(triangle a) { 
  return xToStringLen(a.C[0]) + xToStringLen(a.C[1]) + xToStringLen(a.C[2]) + 2;
}

inline void xToString(char* s, triangle a) { 
  int lx = xToStringLen(a.C[0]);
  int ly = xToStringLen(a.C[1]);
  xToString(s, a.C[0]);
  s[lx] = ' ';
  xToString(s+lx+1, a.C[1]);
  s[lx+ly+1] = ' ';
  xToString(s+lx+ly+2, a.C[2]);
}

namespace benchIO {
  using namespace std;

  string HeaderPoint2d = "pbbs_sequencePoint2d";
  string HeaderPoint3d = "pbbs_sequencePoint3d";
  string HeaderPoint4d = "pbbs_sequencePoint4d";
  string HeaderPoint5d = "pbbs_sequencePoint5d";
  string HeaderPoint6d = "pbbs_sequencePoint6d";
  string HeaderPoint7d = "pbbs_sequencePoint7d";
  string HeaderPoint8d = "pbbs_sequencePoint8d";
  string HeaderPoint9d = "pbbs_sequencePoint9d";
  string HeaderTriangles = "pbbs_triangles";

  inline string headerPoint(int dim) {
    if (dim == 2) return HeaderPoint2d;
    else if (dim == 3) return HeaderPoint3d;
    else if (dim == 4) return HeaderPoint4d;
    else if (dim == 5) return HeaderPoint5d;
    else if (dim == 6) return HeaderPoint6d;
    else if (dim == 7) return HeaderPoint7d;
    else if (dim == 8) return HeaderPoint8d;
    else if (dim == 9) return HeaderPoint9d;
    else {
      cout << "headerPoint unsupported dimension, abort" << dim << endl; abort();
    }
  }

  template <class pointT>
  int writePointsToFile(pointT* P, intT n, char* fname) {
    string Header = (pointT::dim == 2) ? HeaderPoint2d : HeaderPoint3d;
    int r = writeArrayToFile(Header, P, n, fname);
    return r;
  }

  template <class pointT>
  void parsePoints(char** Str, pointT* P, intT n) {
    int d = pointT::dim;
    double* a = newA(double,n*d);
    {parallel_for (long i=0; i < d*n; i++) 
  a[i] = atof(Str[i]);}
    {parallel_for (long i=0; i < n; i++) 
  P[i] = pointT(a+(d*i));}
    free(a);
  }

  void parseNdPoints(char** Str, pointNd* P, long n, int dim) {
    // double* a = newA(double,n*dim);
    // {parallel_for (long i=0; i < dim*n; i++) {
    //   a[i] = atof(Str[i]);
    //   // cout << a[i] << ' ';
    // }}
    {parallel_for (long i=0; i < n; i++) {
      // P[i].init(a+(dim*i), dim);
      P[i].m_dim = dim;
      for (intT d = 0; d < dim; ++ d) {
        // P[i].m_data[d] = *(a+(dim*i) + d);
        P[i].m_data[d] = atof(Str[dim * i + d]);
        // P[i].m_data[d] = 1;
        // cout << P[i].m_data[d] << ' ';
      }
      // cout << endl;
    }}
    // cout << '\n' << "!!!" << endl;
    // free(a);
  }

  inline int readPointsDimensionFromFile(char* fname) {
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWords(S.A, S.n);
    return (int)(W.Strings[0][18])-48;
  }

  template <class pointT>
  _seq<pointT> readPointsFromFile(char* fname) {
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWords(S.A, S.n);
    int d = pointT::dim;
    if (W.m == 0 || W.Strings[0] != headerPoint(d)) {
      cout << "readPointsFromFile wrong file type" << endl;
      abort();
    }
    long n = (W.m-1)/d;
    pointT *P = newA(pointT, n);
    parsePoints(W.Strings + 1, P, n);
    return _seq<pointT>(P, n);
  }

  inline bool isNumber(char myChar) {
    if (myChar == '0' || myChar == '1' || myChar == '2' ||
	myChar == '3' || myChar == '4' || myChar == '5' ||
	myChar == '6' || myChar == '7' || myChar == '8' ||
	myChar == '9') {
      return true;
    } else {
      return false;
    }
  }
  
  inline intT extractDim(words *W) {
    int d;
    char *targetString = W->Strings[0];
    intT myPt = 18;
    while (isNumber(targetString[myPt])) myPt ++;
    targetString[myPt] = '\0'; // TODO Support 10+
    d = atoi(&targetString[18]);
    return d;
  }

  _seq<pointNd> readNdPointsFromFile(char* fname, int *dim) {
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWords(S.A, S.n);
    /*
    if (W.m == 0 || W.Strings[0] != (d == 2 ? HeaderPoint2d : HeaderPoint3d)) {
      cout << "readPointsFromFile wrong file type" << endl;
      abort();
    }
    */
    // Parse dimension and put into d
    int d = extractDim(&W);
    long n = (W.m-1)/d;
    // cout << n << endl;
    pointNd *P = PointNDArrayAlloc(n, d);
    parseNdPoints(W.Strings + 1, P, n, d);
    *dim = d;
    return _seq<pointNd>(P, n);
  }

  _seq<pointNd> readNdPointsFromCSVFast(char* fname, int d) {
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWordsCSV(S.A, S.n);
    /*
    if (W.m == 0 || W.Strings[0] != (d == 2 ? HeaderPoint2d : HeaderPoint3d)) {
      cout << "readPointsFromFile wrong file type" << endl;
      abort();
    }
    */
    // Parse dimension and put into d
    // int d = extractDim(&W);
    long n = W.m/d;
    // cout << n << endl;
    pointNd *P = PointNDArrayAlloc(n, d);
    parseNdPoints(W.Strings, P, n, d);
    // *dim = d;
    W.del();
    return _seq<pointNd>(P, n);
  }

  // also need to supply the total number of points read
  // this version is not parallel at all, but does not have overflow problem
  // when reading very large datasets, the fast version of this function potentially overflows long int
  _seq<pointNd> readNdPointsFromCSVSlow(char* fname, long n, int dim) {
    // https://stackoverflow.com/questions/26443492/read-comma-separated-values-from-a-text-file-in-c

    pointNd *P = PointNDArrayAlloc(n, dim);
    NDGCTYPE *pointData = P->m_data;

    FILE *fp = fopen(fname, "r");
    const char s[3] = ", ";
    char *token;
    long i;
    long count = 0;
    if(fp != NULL) {
      char line[300];
      while(fgets(line, sizeof line, fp) != NULL) {
	//cout << "line: " << line << endl;
	token = strtok(line, s);
	if (token[0] == '\n') {
	  continue;
	}
	//cout << "line ok, first token " << token << endl;
	for(i=0;i<dim;i++) {
	  //cout << "dim " << i << endl;
	  //cout << pointData[count * dim + i] << endl;
	  //cout << atof(token) << endl;
	  P[count].m_data[i] = atof(token);
	  token = strtok(NULL,s);
	}
	count ++;
	//cout << "count " << count << endl;
	if (count >= n) {
	  break;
	}
	if (count % 10000000 == 0) {
	  cout << "read " << count << endl;
	}
      }

      fclose(fp);
    } else {
      perror(fname);
    }
    cout << "csv read " << n << " finish" << endl;
    cout << "csv data count " << count << endl;
    return _seq<pointNd>(P, count);
  }

  _seq<pointNd> read2dNdPointsFromCSV(char* fname, long n, int dim) {
    // https://stackoverflow.com/questions/26443492/read-comma-separated-values-from-a-text-file-in-c
    pointNd *P = PointNDArrayAlloc(n, dim);
    NDGCTYPE *pointData = P->m_data;

    FILE *fp = fopen(fname, "r");
    const char s[3] = ", ";
    char *token;
    long i;
    long count = 0;
    if(fp != NULL)
    {
      char line[100];
      while(fgets(line, sizeof line, fp) != NULL)
      {
        token = strtok(line, s);
        if (token[0] == '\n') {
          continue;
        }
        for(i=0;i<dim;i++)
        {
          if(i==0)
          {  
            pointData[count * dim + i] = atof(token);
            //cout << P[count].m_data[i] << ' ';
            token = strtok(NULL,s);
          } else {
            pointData[count * dim + i] = atof(token);
            //cout << P[count].m_data[i] << endl;
            count ++;
          }       
        }
	if (count >= n) {
	  break;
	}
	if (count % 10000000 == 0) {
	  cout << "read " << count << endl;
	}
      }
      fclose(fp);
    } else {
      perror(fname);
    }
    cout << "csv read " << n << " finish" << endl;
    cout << "csv data count " << count << endl;
    return _seq<pointNd>(P, count);
  }

  triangles<point2d> readTrianglesFromFileNodeEle(char* fname) {
    string nfilename(fname);
    _seq<char> S = readStringFromFile((char*)nfilename.append(".node").c_str());
    words W = stringToWords(S.A, S.n);
    triangles<point2d> Tr;
    Tr.numPoints = atol(W.Strings[0]);
    if (W.m < 4*Tr.numPoints + 4) {
      cout << "readStringFromFileNodeEle inconsistent length" << endl;
      abort();
    }

    Tr.P = newA(point2d, Tr.numPoints);
    for(intT i=0; i < Tr.numPoints; i++) 
      Tr.P[i] = point2d(atof(W.Strings[4*i+5]), atof(W.Strings[4*i+6]));

    string efilename(fname);
    _seq<char> SN = readStringFromFile((char*)efilename.append(".ele").c_str());
    words WE = stringToWords(SN.A, SN.n);
    Tr.numTriangles = atol(WE.Strings[0]);
    if (WE.m < 4*Tr.numTriangles + 3) {
      cout << "readStringFromFileNodeEle inconsistent length" << endl;
      abort();
    }

    Tr.T = newA(triangle, Tr.numTriangles);
    for (long i=0; i < Tr.numTriangles; i++)
      for (int j=0; j < 3; j++)
	Tr.T[i].C[j] = atol(WE.Strings[4*i + 4 + j]);

    return Tr;
  }

  template <class pointT>
    triangles<pointT> readTrianglesFromFile(char* fname, intT offset) {
    int d = pointT::dim;
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWords(S.A, S.n);
    if (W.Strings[0] != HeaderTriangles) {
      cout << "readTrianglesFromFile wrong file type" << endl;
      abort();
    }

    int headerSize = 3;
    triangles<pointT> Tr;
    Tr.numPoints = atol(W.Strings[1]);
    Tr.numTriangles = atol(W.Strings[2]);
    if (W.m != headerSize + 3 * Tr.numTriangles + d * Tr.numPoints) {
      cout << "readTrianglesFromFile inconsistent length" << endl;
      abort();
    }

    Tr.P = newA(pointT, Tr.numPoints);
    parsePoints(W.Strings + headerSize, Tr.P, Tr.numPoints);

    Tr.T = newA(triangle, Tr.numTriangles);
    char** Triangles = W.Strings + headerSize + d * Tr.numPoints;
    for (long i=0; i < Tr.numTriangles; i++)
      for (int j=0; j < 3; j++)
	Tr.T[i].C[j] = atol(Triangles[3*i + j])-offset;
    return Tr;
  }

  template <class pointT>
  triangles<pointT> readTrianglesFromFileNoHull(char* fname, intT offset) {
    int d = pointT::dim;
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWords(S.A, S.n);
    if (W.Strings[0] != HeaderTriangles) {
      cout << "readTrianglesFromFile wrong file type" << endl;
      abort();
    }

    int headerSize = 3;
    triangles<pointT> Tr;
    Tr.numPoints = atol(W.Strings[1]);
    Tr.numTriangles = atol(W.Strings[2]);
    if (W.m != headerSize + 3 * Tr.numTriangles + d * Tr.numPoints) {
      cout << "readTrianglesFromFile inconsistent length" << endl;
      abort();
    }

    Tr.P = newA(pointT, Tr.numPoints);
    parsePoints(W.Strings + headerSize, Tr.P, Tr.numPoints);

    Tr.T = newA(triangle, Tr.numTriangles);
    char** Triangles = W.Strings + headerSize + d * Tr.numPoints;
    intT actualPoints = Tr.numPoints - 10;
    intT actualTriangs = 0;
    for (long i=0; i < Tr.numTriangles; i++) {
      bool valid = true;
      for (int j=0; j < 3; j++) {
        intT tmp = atol(Triangles[3*i + j])-offset;
        if (tmp < actualPoints) {
          Tr.T[actualTriangs].C[j] = tmp;
        } else {
          valid = false; break;
        }
      }
      if (valid) {
        actualTriangs ++;
      }
    }
    Tr.numPoints = actualPoints;
    Tr.numTriangles = actualTriangs;
    cout << "read complete" << endl;
    return Tr;
  }

  template <class pointT>
  int writeTrianglesToFile(triangles<pointT> Tr, char* fileName) {
    ofstream file (fileName, ios::binary);
    if (!file.is_open()) {
      std::cout << "Unable to open file: " << fileName << std::endl;
      return 1;
    }
    file << HeaderTriangles << endl;
    file << Tr.numPoints << endl; 
    file << Tr.numTriangles << endl; 
    writeArrayToStream(file, Tr.P, Tr.numPoints);
    writeArrayToStream(file, Tr.T, Tr.numTriangles);
    file.close();
    return 0;
  }

};

#endif // _BENCH_GEOMETRY_IO
