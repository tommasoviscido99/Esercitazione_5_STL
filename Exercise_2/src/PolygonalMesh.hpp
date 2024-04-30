#pragma once

#include <iostream>
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;
namespace PolygonalLibrary
{

struct PolygonalMesh
{
    unsigned int NumberCell0D = 0;
    vector<unsigned int> Cell0DId = {};
    vector<Vector2d> Cell0DCoordinates = {};
    map<unsigned int, list<unsigned int>> Cell0DMarkers = {};

    unsigned int NumberCell1D = 0;
    vector<unsigned int> Cell1DId = {};
    vector<Vector2i> Cell1DVertices = {};
    map<unsigned int, list<unsigned int>> Cell1DMarkers = {};

    unsigned int NumberCell2D = 0;
    vector<unsigned int> Cell2DId = {};
    vector<unsigned int> Cell2DNumVertices = {};
    vector<vector<unsigned int>> Cell2DVertices = {};
    vector<unsigned int> Cell2DNumEdges = {};
    vector<vector<unsigned int>> Cell2DEdges = {};
    map<unsigned int, vector<unsigned int>> Cell2DMarkers = {};
};

};
