// Folding.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PolyMesh.h"
static const double pi = 3.14159265;

void generateCrumple(const string &fileName, int yawPiDivide, int yawPiMult, double yawPiIrrationalDivide, double angle)
{
  PolyMesh polyMesh;
  double halfang = tan(0.5*angle);
  int maxCorrugations = 256; // a power of two ideally. 128
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

  int k = 0;
  int folds = 0;
  do
  {
    Matrix3d mat;
    mat = AngleAxis<double>(angle, Vector3d::UnitY());
    mat *= mult;
    double x = 0.5; // (double)(rand() % 1000) / 1000.0;
    double y = 0.5; // (double)(rand() % 1000) / 1000.0;
    Vector3d offset(x, y, 0);
    for (int j = 0; j < (int)polyMesh.nodes.size(); j++)
      polyMesh.nodes[j].pos = offset + mat*rot*(polyMesh.nodes[j].pos - offset);

    double minHeight, maxHeight;
    folds = 0;
    do
    {
      minHeight = 1e10;
      maxHeight = 0;
      polyMesh.reflect(Vector3d(0, 0, 0.5*scale), Vector3d(0, 0, -1));
      polyMesh.reflect(Vector3d(0, 0, -0.5*scale), Vector3d(0, 0, 1));
      for (int j = 0; j<(int)polyMesh.nodes.size(); j++)
      {
        minHeight = min(minHeight, polyMesh.nodes[j].pos[2]);
        maxHeight = max(maxHeight, polyMesh.nodes[j].pos[2]);
      }
      folds++;
    } while (maxHeight > 0.5*scale + 0.0002 || minHeight < -0.5*scale - 0.0002);
    scale *= pow(2.0, yaw / pi);
    k++;
  } while (folds > 1);

  polyMesh.save(fileName, Vector3d(0, 0, 0));
}

int _tmain(int argc, _TCHAR* argv[])
{
  double bendAngle = pi / 12.0; // /16
  int varyMode = 0; // 0 is nothing, 1 is vary bend angle, 2 is vary yaw angle
  if (varyMode == 0)
    generateCrumple("testply.ply", 2, 1, 0, bendAngle);
  else if (varyMode == 1)
  {
    for (int bend = 1; bend <= 4; bend++)
    {
      int a = 2;
      int b = 1;
      double c = 0;

      stringstream strstr;
      strstr << "bendangle" << bend << ".ply";
      generateCrumple(strstr.str(), 2, 1, 0, (double)bend * pi/24.0);
    }
  }
  else if (varyMode == 2)
  {
    for (int a = 1; a <= 5; a++)
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
      generateCrumple(strstr.str(), a, b, c, bendAngle);
    }
  }
  return 0;
}
