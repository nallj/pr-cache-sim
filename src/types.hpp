#ifndef NALLJ_DRACHMA_TYPES
#define NALLJ_DRACHMA_TYPES

#include <string>
#include <unordered_map>
#include <vector>

// #include "specs/bitstream.hpp" // TODO: Is this still needed anywhere?
#include "specs/moduleSpec.hpp"

using idList_t = std::vector<unsigned>;
template<class T>
using idLookupMap_t = std::unordered_map<unsigned, T>;
template<class T>
using taskLookupMap_t = std::unordered_map<std::string, T>;

using bitstreamSizeMap_t = idLookupMap_t<unsigned>;
template<class T>
using rrModuleLookupMap_t = idLookupMap_t<idLookupMap_t<T>>;
using targetBs_t = std::pair<bool, moduleSpec>; // { isFound, bs }
using taskRrLookupMap_t = taskLookupMap_t<idLookupMap_t<idList_t>>;

using rrSpecMap_t = rrModuleLookupMap_t<moduleSpec>;

#endif
