#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

struct Entry {
  uint64_t key;
  uint64_t offset;
  uint32_t size;
};

int main(int argc, char *argv[]) {
  const uint64_t TABLE_SIZE = 65536;

  std::vector<Entry> table(TABLE_SIZE);

  std::ofstream out("upgrades.bones", std::ios::binary);

  // reserve space for table
  out.seekp(sizeof(uint64_t) * 2 + sizeof(Entry) * TABLE_SIZE);

  for (auto &file : fs::directory_iterator(
           "/home/boneless/Dev/Boneless/upgrader/assets/upgrades")) {
    auto path = file.path();
    std::string name = path.stem().string();

    uint64_t key = std::stoull(name);

    std::cout << "Packing " << key << std::endl;

    std::ifstream in(path, std::ios::binary);

    std::vector<char> data((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());

    uint64_t offset = out.tellp();
    out.write(data.data(), data.size());

    Entry e{key, offset, (uint32_t)data.size()};

    uint64_t index = key % TABLE_SIZE;

    while (table[index].size != 0)
      index = (index + 1) % TABLE_SIZE;

    table[index] = e;
  }

  // write header + table
  out.seekp(0);

  uint64_t magic = 0x5041434B;
  out.write((char *)&magic, sizeof(magic));
  out.write((char *)&TABLE_SIZE, sizeof(TABLE_SIZE));
  out.write((char *)table.data(), sizeof(Entry) * TABLE_SIZE);
}
