#ifndef OBJ_STATIC_HANDLER_H
#define OBJ_STATIC_HANDLER_H

/*!
 *  \file OBJ_Static_Handler.h
 */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include "scope.h"
#include "utilities.hpp"

/*!
 *  \class OBJ_Static_Handler
 *  \brief Handles the loading of a .obj scene with its associated .mtl
 */
class OBJ_Static_Handler
{
    public:
        OBJ_Static_Handler(std::string OBJ_path, std::string MTL_path);
        virtual ~OBJ_Static_Handler();

        void load(bool loadTextures = true, bool loadMeshes = true, bool computeVertexNormals = true);

        /* Getters */
        StaticMesh *getMesh(std::string meshname);
        vector<StaticMesh *> getAllMeshes();

    protected:
        void loadOBJ(bool loadMeshes, bool computeVertexNormals);
        void loadMTL(bool loadTextures);

    private:
        std::string m_OBJ_path, m_MTL_path;

        map<std::string, AbstractMaterial *>  m_materials;
        map<std::string, StaticMesh *>        m_meshes;

        bool m_MTL_loaded = false;
};

#endif // OBJ_STATIC_HANDLER_H
