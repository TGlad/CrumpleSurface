// Folding.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PolyMesh.h"
static const double pi = 3.14159265;
static int maxK = 0;

void generateCrumple(const string &fileName, int maxCorrugations, int yawPiDivide, int yawPiMult, double yawPiIrrationalDivide, double angle)
{
  PolyMesh polyMesh;
  double scale = (0.5 / (double)maxCorrugations) * sin(angle);

  double yaw = 0;
  double exponent = 0;
  if (yawPiIrrationalDivide)
  {
    yaw = pi / yawPiIrrationalDivide;
    exponent = 2.0 / pi;
  }
  else
  {
    yaw = pi*(double)yawPiMult / (double)yawPiDivide;
    for (int d = 0; d < yawPiDivide; d++)
      exponent += abs(cos((double)d*yaw));
    exponent /= (double)yawPiDivide;
  }
  double mult = 1.0 / pow(cos(angle), exponent);
  Matrix3d rot;
  rot = AngleAxis<double>(yaw, Vector3d::UnitZ());

//  Vector3d offset(0.5, 0.5, 0);
//  for (int j = 0; j < (int)polyMesh.nodes.size(); j++)
//    polyMesh.nodes[j].pos[0] = offset[0] + (polyMesh.nodes[j].pos[0] - offset[0])/mult;

  Matrix3d mat;
  mat = AngleAxis<double>(angle, Vector3d::UnitY());
  //   if (k == 0)
  //     mat /= mult; // so all bend angles start with same number of folds...
  mat *= mult;
  Matrix3d matrot = mat*rot;
  int k = 0;
  int folds = 0;
  double x = 0.5; // (double)(rand() % 1000) / 1000.0;
  double y = 0.5; // (double)(rand() % 1000) / 1000.0;
  Vector3d offset(x, y, 0);
  vector <Node, Eigen::aligned_allocator<Node> > oldNodes;
  vector<Vector2d, Eigen::aligned_allocator<Vector2d> > edge0, edge1;
  do
  {
    oldNodes = polyMesh.nodes;
    for (int j = 0; j < (int)polyMesh.nodes.size(); j++)
      polyMesh.nodes[j].pos = offset + matrot*(polyMesh.nodes[j].pos - offset);

    double minHeight, maxHeight;
    folds = 0;
    cout << "k: " << k << endl;
    do
    {
      minHeight = 1e10;
      maxHeight = 0;
      polyMesh.findEdge = folds==0;
      bool reflect1 = polyMesh.reflect(Vector3d(0, 0, 0.5*scale), Vector3d(0, 0, -1));
      if (polyMesh.findEdge && reflect1 && k == 15)
      {
        cout << "setting edge 0 " << polyMesh.firstEdges0.size() << endl;
        edge0 = polyMesh.firstEdges0;
        edge1 = polyMesh.firstEdges1;
      }
      bool reflect2 = polyMesh.reflect(Vector3d(0, 0, -0.5*scale), Vector3d(0, 0, 1));
      if (polyMesh.findEdge && edge0.size() == 0 && reflect2 && k == 15)
      {
        cout << "setting edge 1 " << polyMesh.firstEdges0.size() << endl;
        edge0 = polyMesh.firstEdges0;
        edge1 = polyMesh.firstEdges1;
      }
      if (!reflect1 && !reflect2)
        break;
      for (int j = 0; j<(int)polyMesh.nodes.size(); j++)
      {
        minHeight = min(minHeight, polyMesh.nodes[j].pos[2]);
        maxHeight = max(maxHeight, polyMesh.nodes[j].pos[2]);
      }
      folds++;
    } while (maxHeight > 0.5*scale + 0.00001 || minHeight < -0.5*scale - 0.00001);
    scale *= pow(2.0, yaw / pi);
    k++;
  } while /*(k < maxK); */ (folds > 0);
  polyMesh.nodes = oldNodes;

  polyMesh.save(fileName, Vector3d(0, 0, 0));
//  polyMesh.saveSVG(Vector3d(1.05*(double)(k - 1), 0, 0), 8);
//  polyMesh.saveSVG(Vector3d(0, 0, 0), angle / (pi / 4.0));
//  polyMesh.saveSVG(Vector3d(angle > pi/30.0 ? 1.05 : 0.0, 0, 0), 1.0);
  polyMesh.saveSVGEdge(fileName, edge0, edge1);
}

int _tmain(int argc, _TCHAR* argv[])
{
  double bendAngle = pi / 6.0; // /16
  int varyMode = 0; // 0 is nothing, 1 is vary bend angle, 2 is vary yaw angle
  maxK = 7;
  if (varyMode == 0)
    generateCrumple("div6128-15.svg", 128, 2, 1, 0, bendAngle);
  else if (varyMode == 1)
  {
//    PolyMesh::openSVG("bendangles.svg", 2);
    for (int bend = 1; bend <= 4; bend++)
    {
      int a = 2;
      int b = 1;
      double c = 0;

  //    double angle = bend == 1 ? pi / 36.0 : 8.0 * pi / 36.0;
      stringstream strstr;
      strstr << "bendangle" << bend << ".ply";
      generateCrumple(strstr.str(), 128, 2, 1, 0, (double)bend * pi / 24.0); 
      maxK++;
    }
 //   PolyMesh::closeSVG();
  }
  else if (varyMode == 2)
  {
    for (int a = 1; a <= 6; a++)
    {
      int b = 1;
      double c = 0;
      if (a == 4)
      {
        a = 3;
        b = 2;
      }
      if (a == 5)
        c = (sqrt(5.0) + 1.0) / 2.0;

      stringstream strstr;
      strstr << "yawangle" << a << ".ply";
      generateCrumple(strstr.str(), 128, a, b, c, bendAngle);
    }
  }
  return 0;
}
