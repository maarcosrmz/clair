#pragma once
<<<<<<<< HEAD:src/tools/clair/codegen/doc.hpp
#include "../module_info.hpp"
#include "utility/string_tools.hpp"
========
#include "../clair-c2py/wdata.hpp"
#include "../../utility/string_tools.hpp"
>>>>>>>> 6bfefe1 (Extract codegen out of clair-c2py into a shared directory):src/tools/codegen/doc.hpp

#include <tuple>
#include <vector>

[[nodiscard]] std::tuple<str_t, std::vector<std::vector<str_t>>, std::vector<str_t>> pydoc(std::vector<fnt_info_t> const &f_list);
[[nodiscard]] str_t pydoc(cls_info_t const &cls);
[[nodiscard]] std::tuple<str_t, std::vector<str_t>> pydoc_of_synthetized_constructor(cls_info_t const &cls_info);
