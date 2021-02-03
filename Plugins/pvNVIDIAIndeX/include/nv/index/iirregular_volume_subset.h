/******************************************************************************
 * Copyright 2020 NVIDIA Corporation. All rights reserved.
 *****************************************************************************/
/// \file
/// \brief Distributed subsets of irregular volume datasets.

#ifndef NVIDIA_INDEX_IIRREGULAR_VOLUME_SUBSET_H
#define NVIDIA_INDEX_IIRREGULAR_VOLUME_SUBSET_H

#include <mi/dice.h>
#include <mi/base/interface_declare.h>
#include <mi/math/vector.h>

#include <nv/index/idistributed_data_subset.h>

namespace nv {
namespace index {

/// Distributed data storage class for irregular volume subsets.
///
/// An irregular volume dataset is defined as a set of cells (see the \c IIrregular_volume_subset::Cell structure). The  
/// cells are defined by a set of faces (see the \c IIrregular_volume_subset::Face structure). The faces, finally, are 
/// defined by a set of vertex positions in three dimensional space. The cell, face and vertex information is handled in  
/// three separate arrays (see \c IIrregular_volume_subset::Mesh_storage). In order to support irregular volumes with 
/// heterogeneous cell and face definitions, which allows for cells with varying numbers of faces and faces with varying 
/// numbers of vertices, the association of faces to cells and vertices to faces is handled by shared cell-face and 
/// face-vertex arrays. This further allows for sharing faces of connected cells in the irregular volume mesh.
/// 
/// Attribute values can either be defined per vertex or per cell (see IIrregular_volume_subset::Attribute_affiliation).
///
/// \note The current implementation of NVIDIA IndeX supports rendering of irregular volumes that
///       use the following cell shapes: tetrahedron, pyramid, wedge/prism, hexahedron.
///
/// \ingroup nv_index_data_subsets
///
class IIrregular_volume_subset :
    public mi::base::Interface_declare<0xac50a241,0x2b4a,0x4e07,0x98,0x9e,0x04,0xa3,0x21,0x95,0x31,0x2f,
                                       IDistributed_data_subset>
{
public:
    /// Irregular volume cell definition.
    ///
    /// This structure holds the number of faces of the irregular volume cell and the starting
    /// index of the face index in the shared cell-face array (see \c Mesh_storage). The cell-face indices are
    /// stored consecutively in the shared cell-face array.
    ///
    struct Cell
    {
        mi::Uint32  nb_faces;             ///< Number of faces compounding this cell.
        mi::Uint32  start_face_index;     ///< Starting index into the cell-face index array.
    };

    /// Irregular volume cell-face definition.
    ///
    /// This structure holds the number of vertices of the irregular volume cell face and the starting
    /// index of the face vertices in the shared face-vertex array (see \c Mesh_storage). The face-vertex indices are
    /// stored consecutively in the shared face-vertex array.
    ///
    struct Face
    {
        mi::Uint32  nb_vertices;          ///< Number of vertices in the face.
        mi::Uint32  start_vertex_index;   ///< Starting index into the face-vertex index array.
    };

    /// Irregular volume mesh storage parameters.
    ///
    /// This structure defines the basic irregular volume dataset parameters such as the number of vertices, faces and
    /// cells in the underlying mesh structure as well as the size of the shared cell-face and face-vertex arrays.
    ///
    /// An additional global mesh maximum edge length is a required mesh parameter the rendering system strongly
    /// requires. This parameter cannot be determined in a straightforward way inside the rendering system as
    /// it only is presented with data sub-sets.
    ///
    struct Mesh_parameters
    {
        // mesh geometry and topology info
        mi::Uint32  nb_vertices;            ///< Size of the vertex array in number of vertices.
        mi::Uint32  nb_face_vtx_indices;    ///< Size of the face vertex index array in number of elements.
        mi::Uint32  nb_faces;               ///< Size of the face array in number of faces.
        mi::Uint32  nb_cell_face_indices;   ///< Size of the cell face index array in number of elements.
        mi::Uint32  nb_cells;               ///< Size of the cell array in number of faces.

        // mesh geometry traits
        mi::Float32 global_max_edge_length; ///< The length of the longest edge in the irregular volume mesh 
    };
    
    /// Irregular volume mesh storage.
    ///
    /// The storage is generated by a call to \c generate_mesh_storage() passing an initialized instance of mesh parameters
    /// \c Mesh_parameters(). A valid mesh storage then contains initialized arrays for storing the irregular volume
    /// subset.
    ///
    struct Mesh_storage
    {
        mi::math::Vector_struct<mi::Float32, 3>*    vertices;           ///< The vertex array.
        mi::Uint32*                                 face_vtx_indices;   ///< The face-vertex index array.
        Face*                                       faces;              ///< The face array.
        mi::Uint32*                                 cell_face_indices;  ///< The cell-face index array.
        Cell*                                       cells;              ///< The cell array.
    };
    
    /// Types of attribute set affiliations.
    ///
    enum Attribute_affiliation
    {
        ATTRIB_AFFIL_PER_VERTEX = 0x00,     ///< Per irregular volume mesh-vertex attribute.
        ATTRIB_AFFIL_PER_CELL   = 0x01      ///< Per irregular volume mesh-cell attribute.
    };
    
    /// Types of attribute set values.
    ///
    enum Attribute_type
    {
        ATTRIB_TYPE_UINT8       = 0x00,     ///<  8bit unsigned integer scalar attribute type.
        ATTRIB_TYPE_UINT8_2     = 0x01,     ///<  8bit unsigned integer vector attribute type with two components.
        ATTRIB_TYPE_UINT8_3     = 0x02,     ///<  8bit unsigned integer vector attribute type with three components.
        ATTRIB_TYPE_UINT8_4     = 0x03,     ///<  8bit unsigned integer vector attribute type with four components.
        ATTRIB_TYPE_UINT16      = 0x04,     ///< 16bit unsigned integer scalar attribute type.
        ATTRIB_TYPE_UINT16_2    = 0x05,     ///< 16bit unsigned integer vector attribute type with two components.
        ATTRIB_TYPE_UINT16_3    = 0x06,     ///< 16bit unsigned integer vector attribute type with three components.
        ATTRIB_TYPE_UINT16_4    = 0x07,     ///< 16bit unsigned integer vector attribute type with four components.
        ATTRIB_TYPE_FLOAT32     = 0x08,     ///< 32bit floating point scalar attribute type.
        ATTRIB_TYPE_FLOAT32_2   = 0x09,     ///< 32bit floating point vector attribute type with two components.
        ATTRIB_TYPE_FLOAT32_3   = 0x10,     ///< 32bit floating point vector attribute type with three components.
        ATTRIB_TYPE_FLOAT32_4   = 0x11      ///< 32bit floating point vector attribute type with four components.
    };

    /// Irregular volume attribute parameters.
    ///
    /// This structure defines the basic parameters of a single attribute set associated with the irregular volume dataset.
    ///
    struct Attribute_parameters
    {
        Attribute_affiliation   affiliation;        ///< Attribute affiliation. See \c Attribute_affiliation.
        Attribute_type          type;               ///< Attribute type. See \c Attribute_type.
        mi::Uint32              nb_attrib_values;   ///< Number of attribute values in number of elements.
    };

    /// Attribute storage.
    ///
    /// The attribute storage is generated by a call to \c generate_mesh_storage() passing an initialized instance of
    /// attribute parameters \c Attribute_parameters. A valid attribute storage contains a raw pointer to an array of
    /// attribute values.
    ///
    struct Attribute_storage
    {
        void*  attrib_values;      ///< The attribute array.
    };

    /// Generate and initialize an instance of irregular volume mesh storage.
    ///
    /// This function initializes an instance of \c Mesh_storage according to the passed instance of \c Mesh_parameters.
    /// At any point during the lifetime of an \c IIrregular_volume_subset instance there can only be a single valid
    /// mesh storage. Multiple calls to this function will invalidate previous instances of the mesh storage and return
    /// a new valid instance according the newly passed mesh parameters.
    ///
    /// \param[in]  mesh_params     The irregular volume mesh parameters.
    /// \param[out] mesh_storage    Mesh_storage instance that contains valid references to the vertex, 
    ///                             face, and cell arrays of the irregular volume mesh.
    ///
    /// \return                     True if a valid storage was generated, false otherwise.
    ///
    virtual bool generate_mesh_storage(
        const Mesh_parameters& mesh_params,
        Mesh_storage&          mesh_storage) = 0;

    /// Generate and initialize an instance of irregular volume attribute set storage.
    ///
    /// This function initializes an instance of \c Attribute_storage according to the passed instance of \c Attribute_parameters.
    /// An irregular volume dataset can be associated with multiple attribute sets which are identified by an attribute
    /// index. At any point during the lifetime of an \c IIrregular_volume_subset instance there can only be a single valid
    /// attribute storage per attribute index. Multiple calls to this function will invalidate previous instances of the
    /// attribute storage associated with the passed index value and return a new valid instance according the newly
    /// passed attribute parameters.
    ///
    /// \param[in]  attrib_index    The storage index for this attribute set.
    /// \param[in]  attrib_params   The attribute parameters.
    /// \param[out] attrib_storage  Attribute storage instance that contains valid references to the attribute value array.
    ///
    /// \return                     True if a valid storage was generated, false otherwise.
    ///
    virtual bool generate_attribute_storage(
        mi::Uint32                  attrib_index,
        const Attribute_parameters& attrib_params,
        Attribute_storage&          attrib_storage) = 0;

    /// Get the irregular volume mesh parameters of the currently valid mesh storage.
    ///
    /// When this function is called before initializing a valid mesh storage (\c generate_mesh_storage()) the returned
    /// instance of \c Mesh_parameters will describe an empty irregular volume mesh (i.e. all values 0).
    ///
    /// \return     The irregular volume mesh parameters of the currently valid mesh storage.
    /// 
    virtual Mesh_parameters get_mesh_parameters() const = 0;

    /// Get the current valid irregular volume mesh storage.
    ///
    /// When this function is called before initializing a valid mesh storage (\c generate_mesh_storage()) the returned
    /// instance of \c Mesh_storage will hold invalid pointers to the particular arrays (i.e. value 0).
    ///
    /// \return     A copy of the currently valid irregular volume mesh storage.
    ///
    virtual Mesh_storage get_mesh() const = 0;

    /// Get the current number of valid attribute sets.
    ///
    /// \return     The number of attribute sets.
    ///
    virtual mi::Uint32 get_nb_attributes() const = 0;

    /// Get the attribute parameters of a currently valid attribute set for a given index.
    ///
    /// \param[in]  attrib_index    The storage index of the attribute set.
    /// \param[out] attrib_params   The attribute set parameters for the given index.
    ///
    /// \return                     True when the attribute set according to the passed index could be found, false otherwise.
    ///
    virtual bool get_attribute_parameters(
        mi::Uint32              attrib_index,
        Attribute_parameters&   attrib_params) const = 0;

    /// Get the currently valid attribute set storage for a given attribute index.
    ///
    /// \param[in]  attrib_index    The storage index of the attribute set.
    /// \param[out] attrib_storage  The attribute storage for the given index.
    ///
    /// \return                     True when the attribute storage according to the passed index could be found, false otherwise.
    //
    virtual bool get_attribute(
        mi::Uint32          attrib_index,
        Attribute_storage&  attrib_storage) const = 0;

    /// GPU device id if the buffer is located on a GPU device.
    ///
    /// \returns GPU device id, negative values indicate that the data is currently not stored on any device.
    ///
    /// \note Experimental
    ///
    virtual mi::Sint32 get_gpu_device_id() const = 0;

    /// Get the currently valid attribute set device-storage for a given attribute index.
    ///
    /// \param[in]  attrib_index    The storage index of the attribute set.
    /// \param[out] attrib_storage  The attribute storage for the given index.
    ///
    /// \return                     True when the attribute storage according to the passed index could be found, false otherwise.
    ///
    /// \note Experimental
    ///
    virtual bool get_active_attribute_device_storage(
        mi::Uint32          attrib_index,
        Attribute_storage&  attrib_storage) const = 0;

    /// \note Experimental
    virtual bool get_backup_attribute_device_storage(
        mi::Uint32          attrib_index,
        Attribute_storage&  attrib_storage) const = 0;

    /// \note Experimental
    virtual bool swap_active_attribute_storage() = 0;


    /// Caching interface (preliminary). Returns true if cache is loaded.
    ///
    /// \note Experimental
    ///
    virtual bool use_cache_file(
        const char*         filename,
        mi::Uint32          mode,
        mi::Uint32          flags) = 0;
};

} // namespace index
} // namespace nv

#endif // NVIDIA_INDEX_IIRREGULAR_VOLUME_SUBSET_H
