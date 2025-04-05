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
#ifndef NODE_POOL_BUILDER_HPP
#define NODE_POOL_BUILDER_HPP

// ASSIMP REQUIRED FOR BUILDER
#ifdef ASSIMP_SCENE_ENABLED

#include <oasis/node_pool.hpp>
#include <glm/glm.hpp>
#include <unordered_map>
#include <functional>


namespace oasis {

// Forward Declaration
class scene;

/**
 * @class node_pool_builder
 * @brief Constructs a node pool from a scene.
 * 
 * This class provides functionality to build a node pool representation 
 * of a scene into a Sparse Voxel DAG (SVDAG).
 */
class node_pool_builder : public virtual node_pool {
public:
  /// Default constructor.
  explicit node_pool_builder() = default;
  
  /**
   * @brief Builds a node pool from a scene.
   * 
   * Recursively processes the given scene to generate a voxel-based 
   * representation. The process involves subdividing space up to the 
   * specified depth and storing voxelized geometry in the node pool.
   * 
   * @param p_scene Pointer to the scene containing geometry data.
   * @param depth Maximum depth of the octree (higher depth increases detail).
   * @param corner The minimum corner of the bounding region.
   * @param size The length of the bounding region's edge.
   *
   * @note this is unoptimized and slow
   */
  void build(scene* p_scene, int depth, glm::vec3 corner, float size);

private:
  int recursive_build(
    scene* p_scene,
    int d, int md, glm::vec3 min, float size, 
    std::vector<size_t>& indexes, size_t start, 
    std::unordered_map<node_t<int>, int> dedup,
    std::function<void(uint64_t)> progress_callback);
};

} // namespace oasis 

#endif // ASSIMP_SCENE_ENABLED 

#endif // NODE_POOL_BUILDER_HPP
