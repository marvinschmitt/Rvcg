// stuff to define the mesh
#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/flag.h>
//#include <vcg/math/quadric.h>
#include <vcg/complex/algorithms/clean.h>
// update
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/container/simple_temporary_data.h>
#include<vcg/complex/allocate.h>
#include <wrap/callback.h>
#include <vcg/complex/append.h>
#include <vcg/simplex/face/pos.h>
#include <../typedef.h>
#include <../RvcgIO.h>
#include <Rcpp.h>

using namespace vcg;
using namespace tri;
using namespace Rcpp;


typedef UpdateTopology<MyMesh>::PEdge SimpleEdge;
  
RcppExport SEXP Rmeshres(SEXP _vb , SEXP _it)
  {
    // declare Mesh and helper variables
    int i, j;
    MyMesh m;
    VertexIterator vi;
    FaceIterator fi;
    
    Rvcg::IOMesh<MyMesh>::RvcgReadR(m,_vb,_it);
    std::vector<SimpleEdge> Edges;
    std::vector< SimpleEdge >::iterator ei;
    std::vector< SimpleEdge >::size_type size;
    tri::UpdateTopology<MyMesh>::FaceFace(m);
    tri::UpdateTopology<MyMesh>::FillUniqueEdgeVector(m,Edges,true);
    size=Edges.size();
    Rcpp::NumericVector edgelength(size);
    double res = 0;
    double tmp1;
    Point3f tmp0;
    VertexPointer vp , vp1;
    for (i = 0;i < size;i++)
    {
      vp=Edges[i].v[0];
      vp1=Edges[i].v[1];
      tmp0 = vp->P()-vp1->P();
      tmp1 = sqrt(tmp0.dot(tmp0));
      res = res + tmp1;
      edgelength[i] = tmp1;
    }
    res = res/size;
    return Rcpp::List::create(Rcpp::Named("res") = res,
			      Rcpp::Named("edgelength") = edgelength
			      );
    //return(wrap(res));
  }

