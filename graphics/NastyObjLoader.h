#pragma once

#include <whb/sdcard.h>
#include <whb/file.h>
#include <whb/log.h>

#include <string>
#include <istream>
#include <sstream>
#include <vector>

/*
Quick'n'nasty way to get some geometry in, before Crypt's object importer arrives.

ONLY USE TRIANGULATED MESHES! THIS EXPECTS VERTEX/TEXCOORD/NORMAL TRIPLES!
*/

void NastyImportObj(const std::string& path, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texcoords) {
  
  char *sdRootPath = WHBGetSdCardMountPath();
  char fullpath[256];
  sprintf(fullpath, "%s/%s", sdRootPath, path.c_str());
  WHBLogPrintf("Loading obj %s", fullpath);
  std::vector<float> rawvertices;
  std::vector<float> rawtexcoords;
  std::vector<float> rawnormals;
  char *text = WHBReadWholeFile(fullpath, NULL);

  std::istringstream objfile(text);
  std::string line;

  while (std::getline(objfile, line)) {
    std::string cmd;
    std::istringstream linestream(line);
    std::getline(linestream,cmd, ' ');
    if (cmd == "v") {
      std::string coord;
      while (std::getline(linestream,coord,' ')) {
         rawvertices.push_back(std::stof(coord));
      }
    } else if (cmd == "vt") {
      std::string coord;
      while (std::getline(linestream,coord,' ')) {
         rawtexcoords.push_back(std::stof(coord));
      }
    } else if (cmd == "vn") {
      std::string coord;
      while (std::getline(linestream,coord,' ')) {
         rawnormals.push_back(std::stof(coord));
      }
    } else if (cmd == "f") {
      std::string node;
      while (std::getline(linestream,node,' ')) {
        std::string index;
        WHBLogPrintf("Node %s", node.c_str());
        std::istringstream nodestream(node);
        std::getline(nodestream,index,'/');
        uint32_t index_i = std::stoi(index);
        for (uint32_t i = (index_i-1)*3; i < (index_i)*3; ++i) {
          WHBLogPrintf("Pushing vertex value %f", rawvertices[i]);
          vertices.push_back(rawvertices[i]);
        }
        std::getline(nodestream,index,'/');
        index_i = std::stoi(index);
        for (uint32_t i = (index_i-1)*2; i < (index_i)*2; ++i) {
          WHBLogPrintf("Pushing texcoord value %f", rawtexcoords[i]);
          texcoords.push_back(rawtexcoords[i]);
        }
        std::getline(nodestream,index,'/');
        index_i = std::stoi(index);
        for (uint32_t i = (index_i-1)*3; i < (index_i)*3; ++i) {
          WHBLogPrintf("Pushing normal value %f", rawnormals[i]);
          normals.push_back(rawnormals[i]);
        }
      }
    }
  }
  WHBLogPrintf("loaded %i vertex elements", vertices.size());
}
