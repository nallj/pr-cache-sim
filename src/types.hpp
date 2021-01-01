#ifndef NALLJ_DRACHMA_TYPES
#define NALLJ_DRACHMA_TYPES

#include <string>
#include <unordered_map>
#include <vector>

using idList_t = std::vector<unsigned>;
template<class T>
using idLookupMap_t = std::unordered_map<unsigned, T>;
template<class T>
using taskLookupMap_t = std::unordered_map<std::string, T>;

using bitstreamSizeMap_t = idLookupMap_t<unsigned>;
using taskRrLookupMap_t = taskLookupMap_t<idLookupMap_t<idList_t>>;

#endif
