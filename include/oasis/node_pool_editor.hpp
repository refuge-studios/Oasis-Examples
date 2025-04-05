/*
 * Copyright (c) 2025 REFUGE STUDIOS PTY LTD. All rights reserved.
 * Created by Adam Yuan on 1/17/24.
 * Modified by Aidan Sanders
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
#ifndef NODE_POOL_EDITOR_HPP
#define NODE_POOL_EDITOR_HPP

#include <oasis/node_pool.hpp>
#include <optional>
#include <functional>
#include <unordered_map>
#include <glm/glm.hpp>

namespace oasis {

/**
 * @class node_pool_editor
 * @brief Provides editing functionality for a node pool.
 * 
 * This class extends `node_pool` with tools to modify the structure, 
 * including duplication, subdivision, and merging of nodes.
 */
class node_pool_editor : public virtual node_pool {
public:
  /// Default constructor.
  explicit node_pool_editor() = default;
    /**
   * @brief Merges another node pool into this pool.
   * 
   * The `other` pool is combined into the current node pool. Existing nodes 
   * may be overwritten, and compression can be applied to remove duplicates.
   * 
   * @param other The node pool to merge.
   * @param overwrite If true, replaces existing nodes when conflicts arise.
   * @param recompress If true, applies compression after merging.
   */
  void combine(node_pool other, bool overwrite, bool recompress);

  void subtract(node_pool other, bool recompress);
  
  /**
   * @brief Constructs a node pool using an intersection test function.
   * 
   * Processes a signed distance function (SDF) or other intersection 
   * test to create a DAG representation of the geometry.
   * 
   * @param depth Maximum depth of the octree structure.
   * @param intersect_test A function that evaluates whether a point 
   *        (position and size) is inside a shape.
   * @return A new node pool representing the geometry.
   */
  node_pool from_sdf(size_t depth, std::function<bool(glm::vec3, float)> intersect_test);

  /**
   * @brief Duplicates a specific child node.
   * 
   * Creates a duplicate of the child node at `child_index` within the 
   * parent node at `parent_index`. The new node is appended to the 
   * node pool, and the parent is updated to reference it.
   * 
   * @param parent_index Index of the parent node.
   * @param child_index Index of the child node to duplicate.
   * @return std::optional<size_t> 
   *   - The index of the new duplicated node if successful.
   *   - `std::nullopt` if the child does not exist.
   */
  std::optional<size_t> duplicate_child(size_t parent_index, size_t child_index);
  
  /**
   * @brief Subdivides a specific child node.
   * 
   * Replaces a child node at `child_index` with a new internal node, 
   * whose children all reference the original child node.
   * 
   * @param parent_index Index of the parent node.
   * @param child_index Index of the child node to subdivide.
   * @return std::optional<size_t> 
   *   - The index of the new subdivided node if successful.
   *   - `std::nullopt` if the child does not exist.
   */
  std::optional<size_t> subdivide_child(size_t parent_index, size_t child_index);
 
private:
  /**
   * @brief Recursively merges nodes during a pool combination.
   * 
   * Used internally by `combine` to integrate nodes from another pool.
   * 
   * @param overwrite If true, replaces existing nodes when conflicts arise.
   * @param parent_index Index of the parent node.
   * @param child_index Index of the child node.
   */
  void recursive_combine(bool overwrite, size_t parent_index, size_t child_index);

  void recursive_subtract(size_t parent_index, size_t child_index);
  
  /**
   * @brief Recursively builds a DAG from an intersection test.
   * 
   * Creates nodes for the octree based on whether the intersection test 
   * function returns true or false for each voxel.
   * 
   * @param pool Reference to the node pool being constructed.
   * @param min The minimum corner of the current voxel.
   * @param size The size of the voxel.
   * @param intersect_test The intersection function.
   * @param rscale The inverse scale of the voxel.
   * @param dedup A map to deduplicate identical nodes.
   * @return The index of the created node.
   */
  int recursive_sdf(node_pool& pool, 
                    glm::uvec3 min, 
                    size_t size, 
                    std::function<bool(glm::vec3, float)>& intersect_test, 
                    float rscale, 
                    std::unordered_map<node_t<int>, int>& dedup);
};

} // namespace oasis 

#endif // NODE_POOL_EDITOR_HPP
