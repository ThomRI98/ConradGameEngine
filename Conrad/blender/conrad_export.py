# This file is a custom exporter for blender compatible with the ConradEngine.
# Command line executable script.
# Automatically exports the current .blend file into a ConradEngine readable format WHEN EXECUTED.

import bpy
import bmesh
import os
import struct
import sys
from mathutils import Vector

OBJTYPE_STATIC = 0
OBJTYPE_DYNAMIC = 1
OBJTYPE_STREAM = 2

MESH_OBJECT_CODE = 0
MATERIAL_OBJECT_CODE = 1

LIGHT_OBJECT_CODE = 2
LIGHT_POINT_CODE = 0
LIGHT_SUN_CODE = 1
LIGHT_SPOT_CODE = 2

CAMERA_OBJECT_CODE = 3

VEC_ARRAY_OBJECT_CODE = 255
VEC_OBJECT_CODE = 254
STR_OBJECT_CODE = 253
IMG_OBJECT_CODE = 252

def triangulate_mesh(mesh):
    bm = bmesh.new()
    bm.from_mesh(mesh)
    
    bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method='BEAUTY', ngon_method='BEAUTY') # 'FIXED' for faster mode
    
    bm.to_mesh(mesh)
    bm.free()

class ConradExporter:
    def __init__(self, filetarget):
        self.filetarget = filetarget
        self.file = open(filetarget, 'wb') # write in bytes
        
    def writeBool(self, value):
        self.file.write(bytes([value]))
        
        return 1

    def writeInt(self, value): # 4 bytes written left to right
        l = list(struct.unpack('4B', struct.pack('I', value)))
        #l.reverse()
        self.file.write(bytes(l))
        
        return 4
        
    def writeFloat(self, value):
        l = list(struct.unpack('4B', struct.pack('f', value)))
        #l.reverse()
        self.file.write(bytes(l))
        
        return 4
        
    def writeChar(self, c):
        try:
            self.file.write(bytes(chr(c), 'utf-8'))
        except TypeError:
            self.file.write(bytes(c, 'utf-8'))        
            
        return 1

        
    def writeString(self, string):
        self.file.write(bytes(string, 'utf-8'))
        self.writeChar('\0')
        
        return len(string) + 1
                    
        
    def writeVec(self, vec): # 4 bytes of meta
        if len(vec) == 0:
            return
        
        #self.writeChar(VEC_OBJECT_CODE)
        self.writeInt(len(vec)) # Dimension
        
        for c in vec:
            self.writeFloat(c)
        
        return 4 + len(vec) * 4

    
    def writeVecArray(self, vec_array): # 8 bytes of meta
        if len(vec_array) == 0:
            return
        
        #self.writeChar(VEC_ARRAY_OBJECT_CODE)
        
        self.writeInt(len(vec_array)) # Count (number of sub vectors)
        self.writeInt(len(vec_array[0])) # Dimension
        
        for vec in vec_array:
            for c in vec:
                self.writeFloat(c)
                
        return 8 + 4*len(vec_array)*len(vec_array[0])
                
    def writeImage(self, image): # 9 bytes of meta
        self.writeChar(IMG_OBJECT_CODE)
        
        ''' WRITING THE ACTUAL IMAGE PIXELS : NOT SUPPORTED YET
        sizeX = image.size[0]
        sizeY = image.size[1]
        self.writeInt(sizeX) # x size
        self.writeInt(sizeY) # y size
        
        self.file.write(bytes([int(p*255) for p in image.pixels]))
        '''
        
        return self.writeString(image.filepath_from_user()) # Writing the path instead for now
    
    def writeLight(self, light):
        print("Writing light", light.name_full)
        totalSize = 0
        
        node = light.data.node_tree.nodes['Emission']    
        
        self.writeChar(LIGHT_OBJECT_CODE)
        self.writeInt(0) # Total size
        
        if("Sun" in light.name_full):
            totalSize += self.writeChar(LIGHT_SUN_CODE)        
        elif("Point" in light.name_full):
            totalSize += self.writeChar(LIGHT_POINT_CODE)
        elif("Spot" in light.name_full):
            totalSize += self.writeChar(LIGHT_SPOT_CODE)
        
        totalSize += self.writeVec(light.location)
        totalSize += self.writeVec([node.inputs['Color'].default_value[i] for i in range(3)])

        up = Vector((0.0, 0.0, -1.0, 0.0))
        direction = (light.matrix_local @ up)
        direction.normalize()
        
        totalSize += self.writeVec([direction[i] for i in range(3)])
        
        intensity = node.inputs['Strength'].default_value
        totalSize += self.writeFloat(intensity)
        
        castShadow = 0 #For now
        totalSize += self.writeBool(castShadow)
        
        # Writing data size
        self.file.seek(-(totalSize+4), 1)
        self.writeInt(totalSize)
        self.file.seek(0, 2) # File end
        return totalSize
    
    def writeMaterial(self, material): # 1 byte of meta
        print("Writing material:", material.name_full)
        
        totalSize = 0
        
        self.writeChar(MATERIAL_OBJECT_CODE)
        self.writeInt(0) # Reserving 4 bytes for the total size
        
        totalSize += self.writeString(material.name_full)
        
        node = material.node_tree.nodes.get("Principled BSDF")
        
        ambient_color = [1.0, 1.0, 1.0] # Not supported for now
        diffuse_color = [material.diffuse_color[i] for i in range(3)]
        specular_intensity = node.inputs[5].default_value # Cycle node value instead of material value (actually changes with the menu)
        specular_color = [material.specular_color[i] for i in range(3)]
        emit_color = [0.0, 0.0, 0.0] # Not supported for now
        roughness = node.inputs[7].default_value
        
        totalSize += self.writeVec(ambient_color)
        totalSize += self.writeVec(diffuse_color)
        totalSize += self.writeFloat(specular_intensity)
        totalSize += self.writeVec(specular_color)
        totalSize += self.writeVec(emit_color)
        totalSize += self.writeInt(int(pow(2, 8*roughness)))
        
        # Texture
        try:
            img = material.node_tree.nodes['Image Texture'].image
        except KeyError:
            print("ERROR : Please add a texture to '", material.name_full, "'")
            return -1
            
        #self.writeImage(img)
        filepath = img.filepath_from_user()
        totalSize += self.writeString(filepath) # For now
        
        #print("\t Material data size :", totalSize / 1000, "kB")
        print("Material data size :", totalSize)
        
        # Writing data size
        self.file.seek(-(totalSize+4), 1)
        self.writeInt(totalSize)
        self.file.seek(0, 2) # File end
        return totalSize
        
    def writeMesh(self, object, type = OBJTYPE_STATIC):
        totalSize = 0
        
        self.writeChar(MESH_OBJECT_CODE)        
        self.writeInt(0) # Reserving 4 bytes for the total size
        
        # Extracting datas
        name = object.name
        print("Writing mesh:", name)
        
        # Geometry
        points_coord = []
        normals_coord = []
        triangulate_mesh(object.data) # Triangulating the mesh

        # Writing geometry
        vert_indices = []
        for face in object.data.polygons:
            vert_indices.append(face.vertices[0])
            vert_indices.append(face.vertices[1])
            vert_indices.append(face.vertices[2])
            
        for index in vert_indices:
            v = object.data.vertices[index]
            transformed_v = object.matrix_local @ v.co # Vertices are normalized between [0;1]. We must thus multiply each by the transformation matrix
            normals_coord.append([c for c in v.normal])
            points_coord.append([component for component in transformed_v])
    
        # Materialp
        material_name = object.data.materials[0].name_full

        # Texture
        tex_coord = [[vertex_uv.uv[i] for i in range(2)] for vertex_uv in object.data.uv_layers['UVMap'].data] # UV Map
        
        # Writing into file
        
        totalSize += self.writeChar(type)
        totalSize += self.writeString(name)
        
        totalSize += self.writeVecArray(points_coord)
        
        totalSize += self.writeString(material_name)
        totalSize += self.writeVecArray(tex_coord)
        totalSize += self.writeVecArray(normals_coord)
        
        # Writing data size
        self.file.seek(-(totalSize+4), 1)
        self.writeInt(totalSize)
        self.file.seek(0, 2) # File end
        
        print("Total size for mesh :", totalSize)
        return totalSize
            
    def close(self):
        self.file.close()
    

def export(filepath):
    print("Exporting...")
    ex = ConradExporter(filepath)
    totalSize = 0
    
    '''totalSize += ex.writeInt(0)
    totalSize += ex.writeChar(MATERIAL_OBJECT_CODE)
    ex.file.seek(-totalSize, 1)
    ex.writeInt(totalSize)
    return (True, 0)'''

    # Writing materials (must be done before meshes)
    for material in bpy.data.materials:
        if material.users == 0:
            continue
        
        size = ex.writeMaterial(material)
        if size < 0:
            return (False, 0)
        totalSize += size
    
    # Writing meshes
    for object in bpy.data.objects:
        if object.users == 0: # Deleted object
            continue
        
        if object.type == 'MESH':
            size = ex.writeMesh(object)
        
        elif object.type == 'LIGHT':
            size = ex.writeLight(object)
        
        if size < 0:
            return (False, 0)
                
        totalSize += size
    
    ex.close()
        
    return (True, totalSize) # Success


success, totalSize = export("D:/GitHub/ConradGameEngine/Conrad/blender/testfile.scene")
if success:
    print("Done exporting", totalSize/1000, "kB.")
else:
    print("An error occured. Exported file will be corrupted.")
