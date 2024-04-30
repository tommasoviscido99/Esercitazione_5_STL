#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

vector<string> split(const string& str, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream iss(str);

    while (getline(iss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

unsigned int toUint(const string& str)
{
    unsigned int value;
    stringstream ss(str);
    if (!(ss >> value))
    {
        cerr << "Error: Invalid unsigned integer conversion for: " << str << endl;
        exit(1);
    }
    return value;
}

double toDouble(const string& str)
{
    double value;
    stringstream ss(str);
    if (!(ss >> value))
    {
        cerr << "Error: Invalid double conversion for: " << str << endl;
        exit(1);
    }
    return value;
}

Vector2d toEigenVec2d(const string& str)
{
    vector<string> coordinates = split(str, ';');
    if (coordinates.size() != 2)
    {
        cerr << "Error: Invalid format for 2D vector in: " << str << endl;
        exit(1);
    }

    double x = toDouble(coordinates[0]);
    double y = toDouble(coordinates[1]);
    return Vector2d(x, y);
}

namespace PolygonalLibrary
{

bool ImportMesh(const string& filepath, PolygonalMesh& mesh)
{
    if (!ImportCell0Ds(filepath + "/Cell0Ds.csv", mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell0D marker:" << endl;
        for (auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            cout << "key:\t" << it->first << "\t values:";
            for (const unsigned int id : it->second)
                cout << "\t" << id;
            cout << endl;
        }
    }

    if (!ImportCell1Ds(filepath + "/Cell1Ds.csv", mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell1D marker:" << endl;
        for (auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it->first << "\t values:";
            for (const unsigned int id : it->second)
                cout << "\t" << id;
            cout << endl;
        }
    }

    if (!ImportCell2Ds(filepath + "/Cell2Ds.csv", mesh))
    {
        return false;
    }
    else
    {
        for (unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            vector<unsigned int> edges = mesh.Cell2DEdges[c];

            for (unsigned int e = 0; e < edges.size(); e++)
            {
                const unsigned int origin = mesh.Cell1DVertices[edges[e]][0];
                const unsigned int end = mesh.Cell1DVertices[edges[e]][1];

                auto findOrigin = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), origin);
                if (findOrigin == mesh.Cell2DVertices[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return false;
                }

                auto findEnd = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), end);
                if (findEnd == mesh.Cell2DVertices[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return false;
                }

                cout << "c: " << c << ", origin: " << *findOrigin << ", end: " << *findEnd << endl;
            }
        }
    }

    return true;
}

bool ImportCell0Ds(const string& filename, PolygonalMesh& mesh)
{
    ifstream file(filename);
    if (file.fail())
    {
        cerr << "Error: Could not open file: " << filename << endl;
        return false;
    }

    string line;
    getline(file, line);

    while (getline(file, line))
    {
        vector<string> tokens = split(line, ';');
        if (tokens.size() != 4)
        {
            cerr << "Error: Invalid format in Cell0Ds.csv: " << line << endl;
            return false;
        }

        unsigned int id = toUint(tokens[0]);
        unsigned int marker = toUint(tokens[1]);
        Vector2d coordinates = toEigenVec2d(tokens[2] + ';' + tokens[3]);

        mesh.Cell0DId.push_back(id);
        mesh.Cell0DCoordinates.push_back(coordinates);
        mesh.NumberCell0D++;

        if (marker != 0)
        {
            auto ret = mesh.Cell0DMarkers.insert({marker, {id}});
            if (!ret.second)
            {
                (ret.first)->second.push_back(id);
                cout << "Added ID " << id << " to existing marker " << marker << endl;
            }
            else
            {
                cout << "Inserted ID " << id << " with new marker " << marker << endl;
            }
        }
    }

    file.close();
    return true;
}


bool ImportCell1Ds(const string& filename, PolygonalMesh& mesh)
{
    ifstream file(filename);
    if (file.fail())
    {
        cerr << "Error: Could not open file: " << filename << endl;
        return false;
    }

    string line;
    getline(file, line);

    while (getline(file, line))
    {
        vector<string> tokens = split(line, ';');
        if (tokens.size() != 4)
        {
            cerr << "Error: Invalid format in Cell1Ds.csv: " << line << endl;
            return false;
        }

        unsigned int id = toUint(tokens[0]);
        unsigned int marker = toUint(tokens[1]);
        unsigned int fromId = toUint(tokens[2]);
        unsigned int toId = toUint(tokens[3]);

        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(Vector2i(fromId, toId));


        if (marker != 0)
        {
            auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
            if(!ret.second)
            {
                (ret.first)->second.push_back(id);
                cout << "Added ID " << id << " to existing marker " << marker << endl;
            }
            else
            {
                cout << "Inserted ID " << id << " with new marker " << marker << endl;
            }
        }
    }

    return true;
}


bool ImportCell2Ds(const string& filename, PolygonalMesh& mesh)
{
    ifstream file(filename);
    if (file.fail())
    {
        cerr << "Error: Could not open file: " << filename << endl;
        return false;
    }

    string line;
    getline(file, line);

    while (getline(file, line))
    {
        vector<string> tokens = split(line, ';');

        unsigned int id = toUint(tokens[0]);
        unsigned int marker = toUint(tokens[1]);
        unsigned int numVertices = toUint(tokens[2]);

        vector<unsigned int> vertices;
        for (int i = 3; i < 3 + numVertices; ++i)
        {
            vertices.push_back(toUint(tokens[i]));
        }
        unsigned int numEdges = toUint(tokens[3 + numVertices]);

        vector<unsigned int> edges;
        for (int i = 4 + numVertices; i < 4 + numVertices + numEdges; ++i)
        {
            edges.push_back(toUint(tokens[i]));
        }

        mesh.Cell2DId.push_back(id);
        mesh.Cell2DMarkers[id].push_back(marker);
        mesh.Cell2DNumVertices.push_back(numVertices);
        mesh.Cell2DVertices.push_back(vertices);
        mesh.Cell2DNumEdges.push_back(numEdges);
        mesh.Cell2DEdges.push_back(edges);
        mesh.NumberCell2D++;
    }

    file.close();
    return true;
}

bool CheckNonZeroSideLengths(const PolygonalMesh& mesh)
{
    vector<bool> alreadyCheckedEdges(mesh.Cell1DVertices.size(), false);

    for (const auto& edges : mesh.Cell2DEdges)
    {
        for (const auto& edgeId : edges)
        {
            if (alreadyCheckedEdges[edgeId])
            {
                continue;
            }

            const Vector2d& vertex1 = mesh.Cell0DCoordinates[mesh.Cell1DVertices[edgeId][0]];
            const Vector2d& vertex2 = mesh.Cell0DCoordinates[mesh.Cell1DVertices[edgeId][1]];

            double length = (vertex2 - vertex1).norm();

            if (length == 0.0)
            {
                return false;
            }

            alreadyCheckedEdges[edgeId] = true;
        }
    }

    return true;
}

bool CheckNonZeroArea(const PolygonalMesh& mesh)
{
    for (const auto& vertices : mesh.Cell2DVertices)
    {
        double area = 0.0;
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const Vector2d& v1 = mesh.Cell0DCoordinates[vertices[i]];
            const Vector2d& v2 = mesh.Cell0DCoordinates[vertices[(i + 1) % vertices.size()]];
            area += v1.x() * v2.y() - v2.x() * v1.y();
        }
        area = abs(area)/2.0;
        if (area == 0.0)
        {
            return false;
        }
    }

    return true;
}
}
