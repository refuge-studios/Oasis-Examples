/*
 * Copyright (c) 2025 REFUGE STUDIOS PTY LTD. All rights reserved.
 * Created by Adam Yuan on 1/12/24.
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
#ifndef NODE_POOL_BASE_HPP
#define NODE_POOL_BASE_HPP

#include <array>
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept> // std::out_of_range
#include <span>

namespace oasis {

template <typename T>
struct node_t {
  std::array<T, 8> children = {0};  // Initialize with 0

  // Default constructor
  constexpr node_t() = default;

  // Constructor that takes an array of nodes
  constexpr node_t(const std::array<T, 8>& nodes) : children(nodes) {}

  // Check if the node has a non-zero value (meaning it's not null or empty)
  constexpr bool has_value() const {
    for (const auto& child : children) {
      if (child != T()) {
        return true;
      }
    }
    return false;
  }

  // Implicit conversion to bool (whether node has value)
  constexpr operator bool() const {
    return has_value();
  }

  // Equality operator (compares children)
  constexpr bool operator==(const node_t<T>& r) const {
    return children == r.children;
  }

  // Inequality operator (compares children)
  constexpr bool operator!=(const node_t<T>& r) const {
    return !(*this == r);
  }

  // Accessor to the entire children array (const version)
  constexpr const std::array<T, 8>& value() const {
    return children;
  }

  // Accessor to the entire children array (non-const version)
  constexpr std::array<T, 8>& value() {
    return children;
  }

  // Access a specific child by index (const version)
  constexpr const T& value(size_t index) const {
    if (index < children.size()) {
      return children[index];
    }
    throw std::out_of_range("Index is out of range");
  }

  // Access a specific child by index (non-const version)
  constexpr T& value(size_t index) {
    if (index < children.size()) {
      return children[index];
    }
    throw std::out_of_range("Index is out of range");
  }

  // Static function that returns a null node (all zeros)
  constexpr static node_t<T> null() {
    return node_t<T>();
  }
};


struct murmur_hasher32_t {
	inline uint32_t operator()(std::span<const uint32_t> word_span) const {
		uint32_t h = 0; // seed
		for (uint32_t k : word_span) {
			k *= 0xcc9e2d51;
			k = (k << 15) | (k >> 17);
			k *= 0x1b873593;
			h ^= k;
			h = (h << 13) | (h >> 19);
			h = h * 5 + 0xe6546b64;
		}
		h ^= uint32_t(word_span.size());
		h ^= h >> 16;
		h *= 0x85ebca6b;
		h ^= h >> 13;
		h *= 0xc2b2ae35;
		h ^= h >> 16;
		return h;
	}
	inline uint32_t operator()(std::span<const uint32_t, 2> word_span) const {
		uint64_t h = word_span[0] | ((uint64_t)word_span[1] << 32ULL);
		h ^= h >> 33;
		h *= 0xff51afd7ed558ccd;
		h ^= h >> 33;
		h *= 0xc4ceb9fe1a85ec53;
		h ^= h >> 33;
		return h;
	}
};

}

namespace std {
  template<>
  struct hash<oasis::node_t<int>> {
    size_t operator()(const oasis::node_t<int>& v) const {
      oasis::murmur_hasher32_t hasher;
      return hasher(std::span<const uint32_t>(reinterpret_cast<const uint32_t*>(v.value().data()), 8));
    }
  };
}


namespace oasis {

/**
 * @class node_pool
 * @brief A pool-based data structure for managing Sparse Voxel Directed 
 * Acyclic Graph (SVDAG) nodes.
 * 
 * The `node_pool` class serves as a storage container for voxel nodes in 
 * a hierarchical spatial structure. It provides functionality for managing, 
 * accessing, and optimizing nodes through deduplication and compression.
 */
class node_pool {
private:
  /// Storage for all nodes in the pool.
  std::vector<node_t<int>> m_nodes;
  
  // Allow access to internal structures for specialized operations.
  friend class node_pool_editor;
  friend class node_pool_builder;
  friend class node_pool_traversal;

public:
  /// Default destructor.
  inline virtual ~node_pool() = default;
  
  /// Default constructor.
  inline explicit node_pool() {}

public:
  /**
   * @brief Adjusts all node indices by a given offset.
   * 
   * This function shifts all stored node indices by `size`, which 
   * is useful when merging or resizing node pools.
   * 
   * @param size The amount by which to shift indices.
   */
  void shift_indexes(int size);

  /**
   * @brief Compress the node pool by deduplicating nodes.
   * 
   * This function optimizes storage by identifying and merging 
   * duplicate voxel nodes, reducing memory usage.
   */
  void compress();
  
  /**
   * @brief Loads a node pool from a binary file.
   * 
   * Reads a serialized node pool from a file, reconstructing the 
   * internal structure for use in rendering or processing.
   * 
   * @param filename The path to the binary file.
   */
  void deserialize(const std::string filename);
  
  /**
   * @brief Returns a reference to the list of nodes.
   * 
   * Allows access to the internal vector storing all nodes 
   * within the pool.
   * 
   * @return Reference to the node vector.
   */
  std::vector<node_t<int>>& get_nodes();
  
  /**
   * @brief Retrieves a specific node by index.
   * 
   * @param index The index of the node to retrieve.
   * @return Reference to the requested node.
   */
  node_t<int>& get_node(int index);

  /**
   * @brief Returns the number of nodes in the pool.
   * 
   * @return Total count of nodes stored in the pool.
   */
  size_t size() const;
};


} // namespace oasis 

#endif 
