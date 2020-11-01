#ifndef NALLJ_DRACHMA_USER_ERROR
#define NALLJ_DRACHMA_USER_ERROR

#include <exception>
#include <string>

class userError : public std::exception {
  typedef void (*helpFn_t)();

  std::string message_;
  bool show_user_help_;
  helpFn_t help_fn_;

public:
  explicit userError(const std::string& message);
  explicit userError(const std::string& message, helpFn_t help_fn);
  
  const char* what() const noexcept(true);
  void showHelpIfProvided() const noexcept(true);
};

#endif
