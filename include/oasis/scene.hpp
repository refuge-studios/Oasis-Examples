/*
 * Copyright (c) 2025 REFUGE STUDIOS PTY LTD. All rights reserved.
 *
 * This software is licensed for use as an API in projects developed by 
 * third-party clients, subject to the following conditions:
 *
 * - Clients may use this API for integration into their own applications.
 * - Redistribution, sublicensing, or modification of the API itself is prohibited.
 * - Reverse engineering, decompilation, or disassembly of this software is not allowed.
 * - This software must not be used to develop competing services or products.
 * - Any project using this API must include the attribution: 
 *   "Powered by Refuge Studios."
 *
 * Refuge Studios retains full ownership and control over this software.
 * Access to updates and support is provided at the discretion of Refuge Studios.
 *
 * THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * REFUGE STUDIOS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY ARISING 
 * FROM THE USE OF THIS SOFTWARE.
 */

#pragma once
#ifndef ASSIMP_SCENE_HPP
#define ASSIMP_SCENE_HPP

#include <array>
#include <string>
#include <optional>
#include <vector>
#include <span>
#include <map>
#include <glm/glm.hpp>

#include "CImg.h"
using namespace cimg_library;

namespace oasis {

/**
 * @struct material_t
 * @brief Represents a material with associated properties such as color and texture.
 */
struct material_t {
  std::string name;              ///< Name of the material.
  std::string texture;           ///< Path to the texture file (if applicable).
  glm::vec3   diffuse_color;     ///< Diffuse color of the material.
  glm::vec3   specular_color;    ///< Specular color of the material (for highlights).
  glm::vec3   ambient_color;     ///< Ambient color (for global illumination effects).
  float       specular_exponent; ///< Controls the shininess of the material in Phong shading.
  float       transparancy;      ///< Transparency level (1.0 = fully opaque, 0.0 = fully transparent).
};

/**
 * @struct triangle_t
 * @brief Represents a triangle with vertex positions and normals.
 */
struct triangle_t {
  glm::vec3 v0, v1, v2; ///< Vertex positions of the triangle.
  glm::vec3 n0, n1, n2; ///< Normal vectors at each vertex.
};

/**
 * @struct indexed_tri_t
 * @brief Represents an indexed triangle using indices into shared vertex buffers.
 */
struct indexed_tri_t {
  std::array<size_t, 3> vertices_idx;   ///< Indices of the triangle's vertices.
  std::array<size_t, 3> tex_coord_idx;  ///< Indices of texture coordinates (if applicable).
  std::array<size_t, 3> normals_idx;    ///< Indices of normal vectors.
  size_t material_idx;                  ///< Index of the material used for this triangle.
};

/**
 * @struct aabb_t
 * @brief Represents an axis-aligned bounding box (AABB) used for spatial partitioning.
 */
struct aabb_t {
  glm::vec3 min; ///< Minimum corner of the bounding box.
  glm::vec3 max; ///< Maximum corner of the bounding box.

  /**
   * @brief Default constructor initializes an empty bounding box.
   */
  aabb_t() : min(glm::vec3(std::numeric_limits<float>::max())), 
             max(glm::vec3(std::numeric_limits<float>::lowest())) {}

  /**
   * @brief Constructs an AABB with specified minimum and maximum points.
   * @param min Minimum corner of the bounding box.
   * @param max Maximum corner of the bounding box.
   */
  aabb_t(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
  
  /**
   * @brief Computes the center point of the bounding box.
   * @return Center of the AABB.
   */
  constexpr glm::vec3 get_center() const { return 0.5f * (min + max); }

  /**
   * @brief Computes the half-size of the bounding box.
   * @return Half-size of the AABB.
   */
  constexpr glm::vec3 get_half() const { return 0.5f * (max - min); }
  
  /**
   * @brief Checks if a point is inside the bounding box.
   * @param p The point to test.
   * @return True if the point is inside, false otherwise.
   */
  constexpr bool contains(const glm::vec3& p) const {
    return  p.x >= min.x && p.x <= max.x &&
            p.y >= min.y && p.y <= max.y &&
            p.z >= min.z && p.z <= max.z;
  }

  /**
   * @brief Checks if this bounding box intersects another bounding box.
   * @param other The other AABB to check against.
   * @return True if the boxes intersect, false otherwise.
   */
  constexpr bool intersects(const aabb_t& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
  }

  /**
   * @brief Resets the bounding box to an empty state.
   */
  constexpr void to_empty() {
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest());
  }

  /**
   * @brief Expands the bounding box to include a given point.
   * @param p The point to merge into the bounding box.
   */
  constexpr void merge(const glm::vec3& p) {
    min = glm::min(min, p); 
    max = glm::max(max, p);
  }
};


class scene {
public:
  using texture_map = std::map<std::string, CImg<uint32_t>>;
  // Scene data
  std::vector<glm::vec3>     m_vertices;      ///< List of vertex positions.
  std::vector<glm::vec3>     m_normals;       ///< List of normal vectors.
  std::vector<material_t>    m_materials;     ///< List of materials used in the scene.
  std::vector<glm::vec2>     m_tex_coords;    ///< List of texture coordinates.
  std::vector<indexed_tri_t> m_indexed_tris;  ///< Indexed triangle geometry.
  std::vector<glm::vec3>     m_triangles;     ///< Raw triangle list (non-indexed).
  aabb_t                     m_bbox;          ///< Axis-aligned bounding box of the scene.
  texture_map                m_textures;      ///< Texture storage.

public:
  // Load scene data from a file.
  bool load(const std::string& filepath);

public:
  // Read-write access (mutable)
  inline std::span<glm::vec3> get_vertices() { return m_vertices; }
  
  inline std::span<glm::vec3> get_normals() { return m_normals; }

  inline std::span<material_t> get_materials() { return m_materials; }

  inline std::span<glm::vec2> get_tex_coords() { return m_tex_coords; }

  inline std::span<indexed_tri_t> get_indexed_geom() { return m_indexed_tris; }
  
  // Read-only access (const span)
  inline std::span<const glm::vec3> get_vertices() const { return m_vertices; }
  
  inline std::span<const glm::vec3> get_normals() const { return m_normals; }

  inline std::span<const material_t> get_materials() const { return m_materials; }
  
  inline std::span<const glm::vec2> get_tex_coords() const { return m_tex_coords; }
  
  inline std::span<const indexed_tri_t> get_indexed_geom() const { return m_indexed_tris; }

  inline std::size_t get_triangles_count() const {
    return !m_indexed_tris.empty() ? m_indexed_tris.size() 
                                   : m_triangles.size(); 
  }
  
  inline std::size_t get_vertices_count() const { return m_vertices.size(); }

  inline std::optional<std::span<const glm::vec3, 3>> get_triangle(const std::size_t id) const { 
    if (id * 3 >= m_triangles.size()) return std::nullopt;
    return std::span<const glm::vec3, 3>(&m_triangles[id * 3], 3);
  }  

  inline std::size_t get_raw_triangles_count() const { return m_triangles.size() / 3; }

  inline void get_bounds(glm::vec3& min, glm::vec3& max) const { 
    min = m_bbox.min;
    max = m_bbox.max;
  }

  inline const glm::vec3* get_triangle_ptr(const std::size_t id) const { 
    return ( id*3 < m_triangles.size()) ? &m_triangles[id*3] : NULL; }

  inline aabb_t get_aabb() const { return m_bbox; }

  inline void get_triangle_vertices(std::size_t id_tri, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) {
    v0 = m_vertices[m_indexed_tris[id_tri].vertices_idx[0]];
    v1 = m_vertices[m_indexed_tris[id_tri].vertices_idx[1]];
    v2 = m_vertices[m_indexed_tris[id_tri].vertices_idx[2]];
  }

  inline void get_triangle_normals(std::size_t id_tri, glm::vec3& n0, glm::vec3& n1, glm::vec3& n2) {
    n0 = m_normals[m_indexed_tris[id_tri].normals_idx[0]];
    n1 = m_normals[m_indexed_tris[id_tri].normals_idx[1]];
    n2 = m_normals[m_indexed_tris[id_tri].normals_idx[2]];
  }

  inline void get_triangle_color(std::size_t id_tri, glm::vec3& c) {
    c = m_materials[get_triangle_material_id(id_tri)].diffuse_color;
  }

  inline size_t get_triangle_material_id(std::size_t id_tri) {
    return (id_tri < m_indexed_tris.size()) 
      ? m_indexed_tris[id_tri].material_idx : 0;
  }

  inline void get_triangle_tex_coords(std::size_t id_tri, glm::vec2& t0, glm::vec2& t1, glm::vec2& t2) {
		t0 = m_tex_coords[m_indexed_tris[id_tri].tex_coord_idx[0]];
		t1 = m_tex_coords[m_indexed_tris[id_tri].tex_coord_idx[1]];
		t2 = m_tex_coords[m_indexed_tris[id_tri].tex_coord_idx[2]];
	}

	inline bool is_triangle_textured(std::size_t id_tri) {
		return m_indexed_tris[id_tri].tex_coord_idx[0] != 0
				&& m_indexed_tris[id_tri].tex_coord_idx[1] != 0
				&& m_indexed_tris[id_tri].tex_coord_idx[2] != 0;
	}

  inline bool load_textures(std::string path) {
	  int matIndex = -1;
	  for (const auto & material : m_materials) {
      ++matIndex;
	    if (matIndex == 0) 
        continue;
	    
      std::string texName(material.texture);
	    if (texName.size() > 3 && m_textures.count(texName) == 0) {
	      // TODO: Check if absolute or relative path - now assumes relative
	      printf("Loading texture '%s' of material '%s'...\n", texName.c_str(), material.name.c_str());
        std::string texPath = path + "/" + texName;
        std::replace(texPath.begin(), texPath.end(), '\\', '/');
        CImg<unsigned char> texture(texPath.c_str());
        m_textures[texName] = texture;
	    }
    }
    if (matIndex == -1) {
      return false;
    }
    return true;
	}

  inline void get_tex_color(const std::string tex_name, 
                            const glm::vec2& uv, 
                            glm::vec3& c) {
    if (m_textures.count(tex_name) == 0) {
			printf("Texture not found: '%s'\n", tex_name.c_str());
			return;
		}
		
    const auto& texture = m_textures[tex_name];
		int w = texture.width();
		int h = texture.height();
		 int x = std::clamp(static_cast<int>(uv[0] * w), 0, w - 1);
    int y = std::clamp(static_cast<int>(uv[1] * h), 0, h - 1);

		// Todo: this assumes the texture is RGB
		unsigned char r = texture(x, y, 0);
		unsigned char g = texture(x, y, 1);
		unsigned char b = texture(x, y, 2);

		c[0] = ((float) r) / 255.0;
		c[1] = ((float) g) / 255.0;
		c[2] = ((float) b) / 255.0;
	}
};

} // namespace oasis 

#endif // ASSIMP_SCENE_HPP
