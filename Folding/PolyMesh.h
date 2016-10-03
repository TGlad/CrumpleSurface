#pragma once
#include "/Code/Eigen/Eigen"
#include "/Code/Eigen/StdVector"
#include <vector>
#include <iostream>
using namespace Eigen;
using namespace std;

struct Node
{
  Node(const Vector3d &p, const Vector2d &uv) : pos(p), uv(uv){}
  Node(){}
  Vector3d pos;
  Vector2d uv;
  vector<Vector2i> newNodesAdded;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

struct Face
{
  struct FaceNode
  {
    int nodeID;
    struct FaceNode *next;
  } *head;
};

struct PolyMesh
{
  void reflectFace(int faceID, const Vector3d &pos, const Vector3d &normal);
  bool reflect(const Vector3d &pos, const Vector3d &normal); // returns if reflection happened
  void reflectNode(int i, const Vector3d &pos, const Vector3d &normal);

  vector<Node, Eigen::aligned_allocator<Node> > nodes;
  vector<Face> faces; // this is a list of edge indices per face
  bool didReflect;

  PolyMesh();

  void save(const string &filename, const Vector3d &offset);
  static void openSVG(const string &fileName, int number);
  void saveSVG(const Vector3d &offset, double shade);
  static void closeSVG();
  void checkFace(const Face &face);
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};