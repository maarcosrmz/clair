#pragma once
<<<<<<<< HEAD:src/tools/clair/codegen/module.hpp
#include "../c2py/wdata.hpp"
========
#include "../clair-c2py/wdata.hpp"
>>>>>>>> 6bfefe1 (Extract codegen out of clair-c2py into a shared directory):src/tools/codegen/module.hpp

str_t codegen_module(module_info_t const &m);

str_t codegen_wrap_info(module_info_t const &m);

str_t codegen_hxx(module_info_t const &m);
