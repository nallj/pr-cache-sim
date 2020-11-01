#include "userError.hpp"

userError::userError(const std::string& message) : message_(message) {}

userError::userError(const std::string& message, helpFn_t help_fn) :
  message_(message), show_user_help_(true), help_fn_(help_fn) {}

const char* userError::what() const noexcept(true) {
  return message_.c_str();
}

void userError::showHelpIfProvided() const noexcept(true) {
  if (show_user_help_) {
    help_fn_();
  }
}
