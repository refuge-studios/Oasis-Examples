// Simple Example for using (Assimp) Node Pool Builder Module
// Note: Building is slow.

#define ASSIMP_SCENE_ENABLED
#include <oasis/node_pool.hpp>
#include <oasis/node_pool_builder.hpp>
#include <oasis/scene.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>

class dag_node_pool final : public virtual oasis::node_pool, public oasis::node_pool_builder {
private:
  friend class oasis::node_pool;

public:
  inline dag_node_pool() : oasis::node_pool(), oasis::node_pool_builder() {}

  inline ~dag_node_pool() final = default;

  bool create(const std::string filename, const std::string out_filename, uint8_t depth) {
    oasis::scene scene;
    if (!scene.load(filename)) {
      std::cerr << "Failed to create scene from: " << filename << std::endl;
      return false;
    }

    const std::string input_dir = std::filesystem::path(filename).parent_path().string();
    if (!scene.load_textures(input_dir)) {
      std::cerr << "failed to create texures" << input_dir << std::endl;
    }

    glm::vec3 min, max;
    scene.get_bounds(min, max);

    glm::vec3 size = max - min;
    float max_size = glm::max(glm::max(size.x, size.y), size.z);
    
    auto start = std::chrono::high_resolution_clock::now();
    build(&scene, depth, min, max_size);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time to voxelize: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << " ms" << std::endl;
    std::cout << "DAG nodes: " << get_nodes().size() << std::endl;

    std::ofstream out_file(out_filename, std::ios::binary);
    if (out_file) {
      size_t voxel_count = get_nodes().size();
      out_file.write(reinterpret_cast<const char*>(&voxel_count), sizeof(voxel_count));
      out_file.write(reinterpret_cast<const char*>(get_nodes().data()), voxel_count * sizeof(oasis::node_t<int>));
      out_file.close();
    } else {
      std::cerr << "Failed to write SVDAG file." << std::endl;
    }
    return true;
  }
};

int main(int argc, char* argv[]) {
  std::string filename, out_filename;
  int depth;

  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <input_filename> <output_filename> <depth>" << std::endl;
    return 1;
  }

  filename = argv[1];
  out_filename = argv[2];
  depth = std::atoi(argv[3]);

  std::cout << "Input file: " << filename << std::endl;
  std::cout << "Output file: " << out_filename << std::endl;
  std::cout << "Depth: " << depth << std::endl;

  dag_node_pool d_pool;
  if (!d_pool.create(filename, out_filename, depth)) {
    std::cout << "Failed to create: " << filename << std::endl;
    return 1;
  }

  std::cout << "Successfully created: " << filename << std::endl;
  return 0;
}
