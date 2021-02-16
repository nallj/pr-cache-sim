#ifndef NALLJ_DRACHMA_APPLICATION_YAML
#define NALLJ_DRACHMA_APPLICATION_YAML

#include <yaml-cpp/yaml.h> // convert, Node

#include "../components/prc.hpp" // rrSelectionType, taskSchedulingType
#include "../specs/application.hpp"
#include "../specs/taskSpec.hpp"

// yaml-cpp extension for deserializing the application class.
namespace YAML {
  template<>
  struct convert<application> {
    static Node encode(const application& rhs);
    static bool decode(const Node& node, application& rhs);
  };
}

#endif
