/* date = February 14th 2021 8:11 am */

#ifndef _3_D_MODELS_H
#define _3_D_MODELS_H

// **************************************
// OBJ Loader ***************************
// **************************************

enum face_type
{
    NO_VALUE,
    VERTEX,
    VERTEX_TEXCOORD,
    VERTEX_NORMAL,
    VERTEX_TEXCOORD_NORMAL,
};

struct mtl_data
{
    hash_table NameIndex; // Get index for mtl data via the name string
    i32 Count;
    
    v3 *Ka; // Ambient
    v3 *Kd; // Diffuse
    v3 *Ks; // Specular
    v3 *Tf; // Transmission filter
    r32 *Ns; // Spec exponent
    r32 *Ni; // Refraction index
    r32 *Tr; // Transparency
    u32 *Illum; // Illumination model: lookup in list
    u8 *Map_Ka; // Ambient texture map
    u8 *Map_Kd; // Diffuse texture map
    u8 *Map_bump; // Bumpmap
};

struct obj_material_group
{
    //char Name[MAX_PATH];
    u8 MatName[50];
    
    u32 *Indices;
    u32  IndiceCount;
};

struct model_information
{
    v3 MinBound;
    v3 MaxBound;
};

struct obj_data
{
    char Name[MAX_PATH];
    
    v3 *Vertice;
    v2 *TexCoords;
    v3 *Normals;
    u8 *SmoothingValue;
    u32 Count;
    
    // Gives information if faces have normals, texcoords, both or nothing
    face_type FaceType;
    
    obj_material_group *SurfaceGroups;
    u32 GroupCount;
    
    mtl_data Materials;
    model_information Infos;
};

internal b32 LoadOBJFile(obj_data *Object, u8 *Filename, arena_allocator *FixArena, arena_allocator *ScratchArena);
internal b32 LoadMtlFile(mtl_data *Materials, arena_allocator *FixArena, arena_allocator *ScratchArena, u8 *Path, u8 *CutPath);

inline   u32 ProcessVectorLine(r32 *Results, u32 VectorDim, u8 *LineChar);
inline   face_type IdentifyFaceType(u8 *Character);
internal u8 *EvaluateNextFaceIndexGroup(hash_table *FaceTable, obj_data *Object, obj_material_group *CurrentGroup, u8 *Character, v3 *Vertice, v2 *TexCoords, v3 *Normals);
internal obj_material_group CollectAllGroupsInOne(obj_data *Object, arena_allocator *FixArena);

#endif //3_D_MODELS_H
