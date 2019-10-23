// Author: Stefan Schlager
// This is based on code from 
// of trimeshinfo
// included in the vcglib sources
// to work with R
#include "typedefTopo.h"
#include "RvcgIO.h"
#include <RcppArmadillo.h>

using namespace vcg;
using namespace tri;
using namespace Rcpp;

RcppExport SEXP Rmeshvol(SEXP mesh_) {
  try {
    TopoMyMesh m;
    int check = Rvcg::IOMesh<TopoMyMesh>::mesh3d2Rvcg(m,mesh_);
    bool VManifold, FManifold, Orientable, Oriented = false;
    float Volume = 0;
    int numholes, BEdges = 0;
    //check manifoldness
    UpdateTopology<TopoMyMesh>::FaceFace(m);
    VManifold = Clean<TopoMyMesh>::CountNonManifoldVertexFF(m) == 0;
    FManifold = Clean<TopoMyMesh>::CountNonManifoldEdgeFF(m) == 0;
    
    if ((!VManifold) || (!FManifold))
      ::Rf_error("Mesh is no manifold\n");
     
       
    tri::Inertia<TopoMyMesh> mm(m);
    mm.Compute(m);
    Volume = mm.Mass();

    // the sign of the volume depend on the mesh orientation
    if (Volume < 0.0)
      Volume = -Volume;
  

  return wrap(Volume);

} catch (std::exception& e) {
  ::Rf_error( e.what());
 } catch (...) {
  ::Rf_error("unknown exception");
 }
}
