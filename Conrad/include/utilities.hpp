#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <sstream>

#include "StaticMesh.h"

#define X_coord 0
#define Y_coord 1
#define Z_coord 2

#define VERTEX  0
#define TEXTURE 1
#define NORMAL  2
#define FACE    3
#define OBJECT  4

#include "AbstractMaterial.h"

#define AMBIENT     0
#define DIFFUSE     1
#define SPECULAR    3
#define EMIT        4
#define SPEC_EXP    5
#define ALPHA       6
#define NEWMTL      7

using namespace std;
using coordinate3d = tuple<float, float, float>;
using coordinate2d = tuple<float, float>;

/* Definitions */
static vector<StaticMesh*> loadOBJ_static(string, bool);
static StaticMesh *StaticMeshFromArrays(vector<coordinate3d>*, vector<coordinate2d>*, vector<coordinate3d>*, vector<int>*, vector<int>*, vector<int>*);


/* ############# PARSING .OBJ ############# */

/*!
 *  \file utilities.hpp
 *  \brief This file is used to define useful static functions for (for now) the rendering engine
 */

/*!
 *  \brief Loads a .obj mesh file treated as static mesh each (environment, terrain, ...)
 *  \return (StaticMesh) The mesh, loaded or not depending on the load parameter value.
 *  \warning Only accepts triangulated meshes ! In other cases, the function behavior is not determined.
 *  \warning Meshes MUST HAVE UV TEXTURE INFOS. This is crucial : the engine works based on textures.
 */
static vector<StaticMesh*> loadOBJ_static(string filepath, bool load = true)
{
    vector<StaticMesh*> meshes;

    ifstream file(filepath.c_str());
    if(!file) {
        cout << "Can't load (" << filepath << ")" << endl;
        return meshes; // empty
    }

    /* Getting the file in memory */
    string line;
    vector<string> lines;
    while(getline(file, line)) {
        lines.push_back(line);
    } file.close();

    /* Parsing the .obj lines */
    vector<coordinate3d> vertices;
    vector<coordinate2d> tex;
    vector<coordinate3d> normals;

    vector<int> faces_vertex_index;
    vector<int> faces_tex_index;
    vector<int> faces_normal_index;

    bool firstO = true; // Checks if the first "o.." has been read (in order not to create an invalid mesh with the infos above the first object). true = we are still at the first one
    for(vector<string>::iterator it = lines.begin(); it != lines.end(); it++) {
        line = (*it);
        stringstream stream(line);
        string c; stream >> c; // First char of the string

        int index = -1;
        if(c == "v")        index = VERTEX;
        else if(c == "vt")  index = TEXTURE;
        else if(c == "vn")  index = NORMAL;
        else if(c == "f")   index = FACE;
        else if(c == "o")   index = OBJECT;

        switch(index) { // First char of the line
            case OBJECT: // Next object has been detected
            {
                if(firstO)  { // We don't want to create "last" object if it's the first one
                    firstO = false;
                    break;
                }

                // Creating last object
                StaticMesh *mesh = StaticMeshFromArrays(&vertices, &tex, &normals, &faces_vertex_index, &faces_tex_index, &faces_normal_index);
                if(load) mesh->load();

                meshes.push_back(mesh);

                /* Reseting everything for next mesh */
                faces_vertex_index.clear();
                faces_tex_index.clear();
                faces_normal_index.clear();

                break;
            }

            case VERTEX: // Vertex
            {
                float x, y, z;
                stream >> x >> y >> z;
                vertices.push_back(make_tuple(x, y, z));

                break;
            }

            case TEXTURE:  // Texture coordinate
            {
                float u, v;
                stream >> u >> v;
                tex.push_back(make_tuple(u, v));

                break;
            }

            case NORMAL: // Normal
            {
                float x, y, z;
                stream >> x >> y >> z;
                normals.push_back(make_tuple(x, y, z));

                break;
            }

            case FACE: // Face
            {
                string vert[3];
                stream >> vert[0] >> vert[1] >> vert[2];

                for(int i = 0;i < 3;i++) { // One vertex
                    stringstream sub_stream(vert[i]);
                    int indexes[3];

                    /* Parsing "1/2/3" */
                    int value, j = 0;
                    while(sub_stream >> value) {
                        indexes[j] = value;

                        if(sub_stream.peek() == '/') sub_stream.ignore();
                        j++;
                    }
                    /* Now indexes[0] is vertex ; indexes[1] is texture ; indexes[2] is normal */

                    // Indexes start at 1 on .obj. The indexes now stored are the real indexes ready to be accessed from.

                    faces_vertex_index.push_back(indexes[0] - 1);   // One vertex  (3 coords)
                    faces_tex_index.push_back(indexes[1] - 1);      // One texture (2 coords)
                    faces_normal_index.push_back(indexes[2] - 1);   // One normal  (3 coords)
                }

                break;
            }

            default: break;
        }
    }

    /* Don't forget the very last one ! (there is no "o" to trigger it ! */
    StaticMesh *mesh = StaticMeshFromArrays(&vertices, &tex, &normals, &faces_vertex_index, &faces_tex_index, &faces_normal_index);
    if(load) mesh->load();

    meshes.push_back(mesh);

    return meshes;
}

static StaticMesh *StaticMeshFromArrays(vector<coordinate3d> *vertices, vector<coordinate2d> *textures, vector<coordinate3d> *normals,
                                        vector<int> *faces_vertex_index, vector<int> *faces_tex_index, vector<int> *faces_normal_index)
{
    /* Creating the mesh */
    float *vertices_array;
    float *colors_array;
    float *tex_array;
    vertices_array = (float*) malloc(faces_vertex_index->size() * 3 * sizeof(float));
    colors_array = (float*) malloc(faces_vertex_index->size() * 3 * sizeof(float));
    tex_array = (float*) malloc(faces_tex_index->size() * 2 * sizeof(float));

    if(vertices_array == 0 || colors_array == 0 || tex_array == 0) {
        std::cout << "Error parsing .obj : out of memory" << std::endl;
        return nullptr;
    }

    std::fill_n(colors_array, faces_vertex_index->size() * 3, 1.0); // Colors aren't used for textured materials

    /* Filling arrays with faces order */
    for(int i = 0;i < faces_vertex_index->size();i++) { // One vertex
        coordinate3d vertex_coords = vertices->at(faces_vertex_index->at(i));

        vertices_array[3*i]     = get<X_coord>(vertex_coords);
        vertices_array[3*i + 1] = get<Y_coord>(vertex_coords);
        vertices_array[3*i + 2] = get<Z_coord>(vertex_coords);
    }

    for(int i = 0;i < faces_tex_index->size();i++) { // One texture
        coordinate2d tex_coords = textures->at(faces_tex_index->at(i));

        tex_array[2*i]      = get<X_coord>(tex_coords); // U
        tex_array[2*i + 1]  = get<Y_coord>(tex_coords); // V
    }

    /* Normals not supported for now */

    StaticMesh *mesh = new StaticMesh(faces_vertex_index->size(), vertices_array, colors_array, tex_array);
    return mesh;
}

/* ############# MATERIALS ############# */

/*!
 *  \brief Loads a .MTL file into an AbstractMaterial array
 *  \return (vector<AbstractMaterial>) loaded materials
 */
static vector<AbstractMaterial> loadMTL(string filepath)
{
    vector<AbstractMaterial> materials;
    ifstream file(filepath.c_str());
    if(!file) {
        cout << "Can't load (" << filepath << ")" << endl;
        return materials; // empty
    }

    /* Getting the file in memory */
    string line;
    vector<string> lines;
    while(getline(file, line)) {
        lines.push_back(line);
    } file.close();

    RGB Ka, Kd, Ks, Ke;
    float Ns, d;
    bool first(true); // Are we still at the first one ?

    for(vector<string>::iterator it = lines.begin(); it != lines.end(); it++) {
        line = (*it);
        stringstream stream(line);
        string c; stream >> c; // identifier

        int index = -1;
        if(c == "Ka")           index = AMBIENT;
        else if(c == "Kd")      index = DIFFUSE;
        else if(c == "Ks")      index = SPECULAR;
        else if(c == "Ke")      index = EMIT;
        else if(c == "Ns")      index = SPEC_EXP;
        else if(c == "d")       index = ALPHA;
        else if(c == "newmtl")  index = NEWMTL;

        switch(index) {
            case NEWMTL:
            {
                if(first) {
                    first = false;
                    break;
                }

                // Treating last material
                AbstractMaterial material(Ka, Kd, Ks, Ke, Ns, d);
                materials.push_back(material);

                // No need to reset : the values will be overwritten

                break;
            }

            case AMBIENT:
            {
                float x, y, z;
                stream >> x >> y >> z;

                Ka = {x, y, z};
                break;
            }

            case DIFFUSE:
            {
                float x, y, z;
                stream >> x >> y >> z;

                Kd = {x, y, z};
                break;
            }

            case SPECULAR:
            {
                float x, y, z;
                stream >> x >> y >> z;

                Ks = {x, y, z};
                break;
            }

            case EMIT:
            {
                float x, y, z;
                stream >> x >> y >> z;

                Ke = {x, y, z};
                break;
            }

            case SPEC_EXP:
            {
                float value;
                stream >> value;

                Ns = value;
                break;
            }

            case ALPHA:
            {
                float value;
                stream >> value;

                d = value;
                break;
            }
        }
    }

    // Very last material
    AbstractMaterial material(Ka, Kd, Ks, Ke, Ns, d);
    materials.push_back(material);

    return materials;
}

#endif // UTILITIES_HPP_INCLUDED
