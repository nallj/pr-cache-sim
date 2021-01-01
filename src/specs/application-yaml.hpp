#ifndef NALLJ_DRACHMA_APPLICATION_YAML
#define NALLJ_DRACHMA_APPLICATION_YAML

#include <yaml-cpp/yaml.h> // convert, Node

#include "application.hpp"
#include "../components/prc.hpp" // rrSelectionType, taskSchedulingType

// yaml-cpp extension for deserializing the application class.
namespace YAML {
  template<>
  struct convert<application> {
    static Node encode(const application& rhs);
    static bool decode(const Node& node, application& rhs);
  };
}

#endif
