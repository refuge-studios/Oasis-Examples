/*
 * Copyright (c) 2025 REFUGE STUDIOS PTY LTD. All rights reserved.
 * Created by Adam Yuan on 1/15/24.
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
#ifndef NODE_POOL_TRAVERSAL_GLM_HPP
#define NODE_POOL_TRAVERSAL_GLM_HPP

#include <oasis/node_pool.hpp>
#include <optional>
#include <glm/glm.hpp>

namespace oasis {

/**
 * @class node_pool_traversal
 * @brief Provides ray traversal functionality for a node pool.
 * 
 * This class extends `node_pool` with the ability to perform 
 * ray traversal through SVDAG. 
 */
class node_pool_traversal : public virtual node_pool {
public:
  /// Default constructor.
  explicit node_pool_traversal() = default;
  
  /**
   * @brief Performs ray traversal through the node pool.
   * 
   * Traverses the Node Pool using a ray defined by origin (`o`) and 
   * direction (`d`). If an intersection is found within `max_dist`, it 
   * returns the intersection point along with the hit distance.
   * 
   * @param o The ray origin in world space.
   * @param d The ray direction (assumed normalized).
   * @param max_depth Maximum traversal depth (octree level).
   * @param max_dist Maximum allowed distance for a hit.
   * @return std::optional<std::pair<glm::vec3, float>> 
   *   - If a hit occurs, returns hit distance.
   *   - If no hit occurs, returns `std::nullopt`.
   */ 
  std::optional<glm::vec3> traversal(glm::vec3 o, glm::vec3 d, 
                                     uint32_t max_depth, float max_dist);
};

} // namespace vox

#endif // NODE_POOL_TRAVERSAL_GLM_HPP 
