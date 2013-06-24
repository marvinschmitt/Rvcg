#include <string.h>
#include <vector>
#include <stdio.h>
#include <cstddef>
#include <limits>

// VCG headers for triangular mesh processing
#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/quality.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/intersection.h>
#include<vcg/complex/allocate.h>
#include <wrap/callback.h>
#include <vcg/complex/append.h>

// VCG File Format Importer/Exporter
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/export_ply.h>
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/algorithms/update/curvature.h>
#include <vcg/complex/algorithms/update/normal.h>

#include <../RvcgIO.h>
//#include <wrap/ply/plylib.cpp>
#include <Rcpp.h>

using namespace vcg;
using namespace Rcpp;
using namespace std;

class CurvFace;
class CurvEdge;
class CurvVertex;
struct CurvUsedTypes : public UsedTypes<Use<CurvVertex>::AsVertexType,
					Use<CurvEdge>::AsEdgeType,
					Use<CurvFace>::AsFaceType>{};
class CurvEdge : public Edge<CurvUsedTypes>{};
class CurvVertex  : public Vertex< CurvUsedTypes,
				   vertex::Coord3f, 
				   vertex::BitFlags, 
				   vertex::Normal3f, 
				   vertex::Mark,
				   vertex::Color4b, 
				   vertex::VFAdj,
				   vertex::Curvaturef,
				   vertex::CurvatureDirf,
				   vertex::Qualityf> {};
class CurvFace    : public Face  <CurvUsedTypes, 
				  face::VertexRef,
				  face::BitFlags,
				  face::Mark, 
				  face::Normal3f,
				  face::VFAdj,
				  face::FFAdj>{};

class CurvMesh : public tri::TriMesh< vector<CurvVertex>, vector<CurvFace > >{};
typedef CurvMesh::ScalarType ScalarType;
typedef CurvMesh::VertexIterator VertexIterator;
typedef CurvMesh::VertexPointer VertexPointer;
typedef CurvMesh::FaceIterator FaceIterator;
typedef CurvMesh::FacePointer FacePointer;
typedef CurvMesh::CoordType CoordType;
typedef CurvMesh::ScalarType ScalarType;
typedef CurvMesh::ConstVertexIterator ConstVertexIterator;
typedef CurvMesh::ConstFaceIterator   ConstFaceIterator;
  
RcppExport SEXP Rcurvature( SEXP _vb, SEXP _it)
{
  // declare Mesh and helper variables
  int i, j;
  CurvMesh m;
  VertexIterator vi;
  FaceIterator fi;
 
  Rvcg::IOMesh<CurvMesh>::RvcgReadR(m,_vb,_it);
  tri::UpdateTopology<CurvMesh>::FaceFace(m);
  tri::UpdateTopology<CurvMesh>::VertexFace(m);
  tri::UpdateBounding<CurvMesh>::Box(m);
  tri::Allocator<CurvMesh>::CompactVertexVector(m);
  tri::UpdateCurvature<CurvMesh>::MeanAndGaussian(m);
  tri::UpdateQuality<CurvMesh>::VertexFromRMSCurvature(m);
   
  //Bordersearch
  tri::UpdateFlags<CurvMesh>::FaceBorderFromNone(m);
  tri::UpdateSelection<CurvMesh>::FaceFromBorderFlag(m);
  tri::UpdateFlags<CurvMesh>::VertexBorderFromNone(m);
  tri::UpdateSelection<CurvMesh>::VertexFromBorderFlag(m);
  
  std::vector<float> gaussvb, meanvb, gaussitmax, meanitmax;
  std::vector<float> RMSvb;
  std::vector<int> bordervb, borderit;
  vi=m.vert.begin();
  //for(i=0; i < m.vn; i++)
  for(i=0; i < m.vn; i++)
    {
      gaussvb.push_back(vi->Kg());
      meanvb.push_back(vi->Kh());
      RMSvb.push_back(vi->Q());
      if ((*vi).IsS())
	bordervb.push_back(1);
      else
	bordervb.push_back(0);
      ++vi;    
    }
   
  fi=m.face.begin();
  float tmpg, tmpm;
  for(i=0; i < m.fn; i++)
    {// get max curvature of vertices per face
      tmpg = (*fi).V(0)->Kg();
      tmpm = (*fi).V(0)->Kh();

      for (j = 1; j < 3; j++)
	{
	  if (abs(tmpg) < (*fi).V(j)->Kg())
	    tmpg = (*fi).V(j)->Kg();
	  if (abs(tmpm) < (*fi).V(j)->Kh())
	    tmpm = (*fi).V(j)->Kh();
	}
      //write borderinfo
      if ((*fi).IsS())
	borderit.push_back(1);
      else
	borderit.push_back(0);
      gaussitmax.push_back(tmpg);
      meanitmax.push_back(tmpm);
      ++fi;    
    }
   
  //return(wrap(curvevb));
  return Rcpp::List::create(Rcpp::Named("gaussvb") = gaussvb,
			    Rcpp::Named("meanvb") = meanvb,
			    Rcpp::Named("RMSvb") = RMSvb,
			    Rcpp::Named("gaussitmax") = gaussitmax,
			    Rcpp::Named("borderit") = borderit,
			    Rcpp::Named("bordervb") = bordervb,
			    Rcpp::Named("meanitmax") = meanitmax
			    );

}