#include "./forward_decls.hpp"

#include <fmt/format.h>
#include <map>
#include <sstream>
#include <string>
#include "utility/string_tools.hpp"

using util::join;

namespace codegen {

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

// Split "ns1::ns2::Name" → {"ns1::ns2", "Name"}.  No namespace → first element is empty.
static std::pair<str_t, str_t> split_ns(str_t const &fqn) {
  auto pos = fqn.rfind("::");
  if (pos == str_t::npos) return {"", fqn};
  return {fqn.substr(0, pos), fqn.substr(pos + 2)};
}

// Build the extern "C" parameter-type list (no names; pass-by-ref params become T*).
static str_t extern_c_params(std::vector<ir::ParamVarDecl> const &params) {
  return join(params, [](auto const &p) -> str_t {
    return (!p.is_fortran_value && p.type.name.find('*') == str_t::npos)
        ? "void *" : p.type.name;
  }, ", ");
}

// Emit a map of namespace → declarations, wrapping each non-empty namespace.
static void emit_ns_map(std::stringstream &out, std::map<str_t, std::vector<str_t>> const &nsmap) {
  for (auto const &[ns, decls] : nsmap) {
    if (ns.empty()) {
      for (auto const &d : decls) out << d << "\n";
    } else {
      out << "namespace " << ns << " {\n";
      for (auto const &d : decls) out << "  " << d << "\n";
      out << "}\n";
    }
  }
}

// ---------------------------------------------------------------------------
// gen_forward_decls  (Fortran / f2py only — assumes linkage_name is set)
// ---------------------------------------------------------------------------

std::string gen_forward_decls(module_info_t const &m) {
  std::vector<str_t>                  extern_c_lines;
  std::map<str_t, std::vector<str_t>> ns_full_decls;

  // ── Derived-type structs ──────────────────────────────────────────────────
  for (auto const &[pyname, cls_info] : m.classes) {
    auto [ns, name] = split_ns(cls_info.ptr->fully_qualified_name);

    // extern "C" declarations for non-static type-bound procedures.
    // Self is always opaque (void *); CLASS(T)/CLASS(*) uses void** (descriptor box).
    for (auto const &[mname, overloads] : cls_info.methods) {
      for (auto const &fi : overloads) {
        if (!fi.ptr || fi.ptr->linkage_name.empty() || fi.ptr->is_static) continue;
        auto const &fd = *fi.ptr;
        str_t self = fd.self_is_polymorphic ? "void **" : "void *";
        auto  ext  = extern_c_params(fd.params);
        auto  sig  = fmt::format("{}({}{}{});", fd.linkage_name, self, ext.empty() ? "" : ", ", ext);
        extern_c_lines.push_back(fd.return_type.name + " " + sig);
      }
    }

    // full struct definition with fields only
    auto fields = join(cls_info.fields, [](auto const *f) {
      return fmt::format("  {} {};", f->type.name, f->name);
    }, "\n");

    ns_full_decls[ns].push_back(fmt::format("struct {} {{\n{}}};",
        name, fields.empty() ? "" : fields + "\n"));
  }

  // ── Free function declarations ────────────────────────────────────────────
  for (auto const &[pyname, overloads] : m.functions) {
    for (auto const &fi : overloads) {
      if (!fi.ptr) continue;
      auto const &fd = *fi.ptr;
      auto sig = fd.qualified_name + "(" + fd.param_types_str() + ")";

      extern_c_lines.push_back(fmt::format("{} {}({});",
          fd.return_type.name, fd.linkage_name, extern_c_params(fd.params)));
    }
  }

  // ── Emit ─────────────────────────────────────────────────────────────────
  // Ordering:
  //   1. Full struct definitions — complete types available before extern "C",
  //      which is necessary for VALUE-attribute derived-type parameters.
  //   2. extern "C" block       — Flang symbol prototypes; self is always void *
  //      so no struct declaration is needed before this block.
  std::stringstream out;
  emit_ns_map(out, ns_full_decls);
  if (!extern_c_lines.empty()) {
    out << "extern \"C\" {\n";
    for (auto const &l : extern_c_lines) out << "  " << l << "\n";
    out << "}\n";
  }
  return out.str();
}

} // namespace codegen
