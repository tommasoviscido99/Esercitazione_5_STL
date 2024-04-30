#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;

int main()
{
    PolygonalMesh mesh;

    string filepath = "PolygonalMesh";
    if (!ImportMesh(filepath, mesh))
    {
        return 1;
    }

    if (!CheckNonZeroSideLengths(mesh))
    {
        cerr << "Error: Some polygon edges have zero length" << endl;
        return 1;
    }

    if (!CheckNonZeroArea(mesh))
    {
        cerr << "Error: Some polygons have zero area" << endl;
        return 1;
    }

    cout << "Mesh validation successful" << endl;

    return 0;
}
