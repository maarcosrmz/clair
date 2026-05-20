#pragma once
<<<<<<<< HEAD:src/tools/clair/codegen/classes.hpp
#include "../module_info.hpp"
========
#include "../clair-c2py/wdata.hpp"
>>>>>>>> 6bfefe1 (Extract codegen out of clair-c2py into a shared directory):src/tools/codegen/classes.hpp

str_t codegen_cls(std::ostream &code, str_t const &cls_py_name, cls_info_t const &cls, str_t const &full_module_name);
