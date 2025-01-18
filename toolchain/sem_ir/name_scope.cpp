// Part of the Carbon Language project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "toolchain/sem_ir/name_scope.h"

#include "toolchain/sem_ir/file.h"

namespace Carbon::SemIR {

auto NameScope::Print(llvm::raw_ostream& out) const -> void {
  out << "{inst: " << inst_id_ << ", parent_scope: " << parent_scope_id_
      << ", has_error: " << (has_error_ ? "true" : "false");

  out << ", extended_scopes: [";
  llvm::ListSeparator scope_sep;
  for (auto id : extended_scopes_) {
    out << scope_sep << id;
  }
  out << "]";

  out << ", names: {";
  llvm::ListSeparator sep;
  for (auto entry : names_) {
    if (entry.is_poisoned) {
      continue;
    }
    out << sep << entry.name_id << ": " << entry.inst_id;
  }
  out << "}";

  out << "}";
}

auto NameScope::AddRequired(Entry name_entry) -> void {
  CARBON_CHECK(!name_entry.is_poisoned, "Cannot add a poisoned name: {0}.",
               name_entry.name_id);
  auto add_name = [&] {
    EntryId index(names_.size());
    names_.push_back(name_entry);
    return index;
  };
  auto result = name_map_.Insert(name_entry.name_id, add_name);
  if (!result.is_inserted()) {
    // A required name can overwrite poison.
    auto& name = names_[result.value().index];
    CARBON_CHECK(name.is_poisoned, "Failed to add required name: {0}",
                 name_entry.name_id);
    name = name_entry;
  }
}

auto NameScope::LookupOrAdd(SemIR::NameId name_id, InstId inst_id,
                            AccessKind access_kind)
    -> std::pair<bool, EntryId> {
  auto insert_result = name_map_.Insert(name_id, EntryId(names_.size()));
  if (!insert_result.is_inserted()) {
    return {false, EntryId(insert_result.value())};
  }

  names_.push_back(
      {.name_id = name_id, .inst_id = inst_id, .access_kind = access_kind});
  return {true, EntryId(names_.size() - 1)};
}

auto NameScope::LookupOrPoison(NameId name_id) -> std::optional<EntryId> {
  auto insert_result = name_map_.Insert(name_id, EntryId(names_.size()));
  if (insert_result.is_inserted()) {
    names_.push_back({.name_id = name_id,
                      .inst_id = InstId::Invalid,
                      .access_kind = AccessKind::Public,
                      .is_poisoned = true});
    return std::nullopt;
  }
  return insert_result.value();
}

auto NameScopeStore::GetInstIfValid(NameScopeId scope_id) const
    -> std::pair<InstId, std::optional<Inst>> {
  if (!scope_id.is_valid()) {
    return {InstId::Invalid, std::nullopt};
  }
  auto inst_id = Get(scope_id).inst_id();
  if (!inst_id.is_valid()) {
    return {InstId::Invalid, std::nullopt};
  }
  return {inst_id, file_->insts().Get(inst_id)};
}

auto NameScopeStore::IsCorePackage(NameScopeId scope_id) const -> bool {
  if (!IsPackage(scope_id)) {
    return false;
  }
  auto scope_name =
      file_->names().GetAsStringIfIdentifier(Get(scope_id).name_id());
  return scope_name == "Core";
}

}  // namespace Carbon::SemIR
