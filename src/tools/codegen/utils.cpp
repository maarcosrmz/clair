#include "./utils.hpp"
<<<<<<<< HEAD:src/tools/clair/codegen/utils.cpp
#include "utility/string_tools.hpp"
========
#include "../../utility/string_tools.hpp"
>>>>>>>> 6bfefe1 (Extract codegen out of clair-c2py into a shared directory):src/tools/codegen/utils.cpp

#include <fmt/format.h>

namespace codegen {

  std::string cpp_to_py_types(std::vector<std::string> const &cpp_types) {
    if (cpp_types.empty()) return std::string{};
    return fmt::format(R"RAW(c2py::python_typename<{}>())RAW", util::join(cpp_types, ">(), c2py::python_typename<"));
  };

} // namespace codegen