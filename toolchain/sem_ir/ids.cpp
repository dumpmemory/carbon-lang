// Part of the Carbon Language project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "toolchain/sem_ir/ids.h"

#include "toolchain/sem_ir/singleton_insts.h"
#include "toolchain/sem_ir/typed_insts.h"

namespace Carbon::SemIR {

auto InstId::Print(llvm::raw_ostream& out) const -> void {
  if (IsSingletonInstId(*this)) {
    out << Label << "(" << SingletonInstKinds[index] << ")";
  } else {
    IdBase::Print(out);
  }
}

auto ConstantId::Print(llvm::raw_ostream& out, bool disambiguate) const
    -> void {
  if (!is_valid()) {
    IdBase::Print(out);
    return;
  }
  if (is_template()) {
    if (disambiguate) {
      out << "template_constant(";
    }
    out << template_inst_id();
    if (disambiguate) {
      out << ")";
    }
  } else if (is_symbolic()) {
    out << "symbolic_constant" << symbolic_index();
  } else {
    CARBON_CHECK(!is_constant());
    out << "runtime";
  }
}

auto RuntimeParamIndex::Print(llvm::raw_ostream& out) const -> void {
  if (*this == Unknown) {
    out << Label << "<unknown>";
  } else {
    IndexBase::Print(out);
  }
}

auto GenericInstIndex::Print(llvm::raw_ostream& out) const -> void {
  out << "generic_inst";
  if (is_valid()) {
    out << (region() == Declaration ? "_in_decl" : "_in_def") << index();
  } else {
    out << "<invalid>";
  }
}

auto BoolValue::Print(llvm::raw_ostream& out) const -> void {
  if (*this == False) {
    out << "false";
  } else if (*this == True) {
    out << "true";
  } else {
    CARBON_FATAL("Invalid bool value {0}", index);
  }
}

auto IntKind::Print(llvm::raw_ostream& out) const -> void {
  if (*this == Unsigned) {
    out << "unsigned";
  } else if (*this == Signed) {
    out << "signed";
  } else {
    CARBON_FATAL("Invalid int kind value {0}", index);
  }
}

auto NameId::ForIdentifier(IdentifierId id) -> NameId {
  if (id.index >= 0) {
    return NameId(id.index);
  } else if (!id.is_valid()) {
    return NameId::Invalid;
  } else {
    CARBON_FATAL("Unexpected identifier ID {0}", id);
  }
}

auto NameId::Print(llvm::raw_ostream& out) const -> void {
  if (!is_valid() || index >= 0) {
    IdBase::Print(out);
    return;
  }
  out << Label << "(";
  if (*this == SelfValue) {
    out << "SelfValue";
  } else if (*this == SelfType) {
    out << "SelfType";
  } else if (*this == PeriodSelf) {
    out << "PeriodSelf";
  } else if (*this == ReturnSlot) {
    out << "ReturnSlot";
  } else if (*this == PackageNamespace) {
    out << "PackageNamespace";
  } else if (*this == Base) {
    out << "Base";
  } else {
    CARBON_FATAL("Unknown index {0}", index);
    IdBase::Print(out);
  }
  out << ")";
}

auto InstBlockId::Print(llvm::raw_ostream& out) const -> void {
  if (*this == Unreachable) {
    out << "unreachable";
  } else if (*this == Empty) {
    out << Label << "_empty";
  } else if (*this == Exports) {
    out << "exports";
  } else if (*this == ImportRefs) {
    out << "import_refs";
  } else if (*this == GlobalInit) {
    out << "global_init";
  } else {
    IdBase::Print(out);
  }
}

auto TypeId::Print(llvm::raw_ostream& out) const -> void {
  out << Label << "(";
  if (*this == TypeType::SingletonTypeId) {
    out << "TypeType";
  } else if (*this == AutoType::SingletonTypeId) {
    out << "AutoType";
  } else if (*this == ErrorInst::SingletonTypeId) {
    out << "Error";
  } else {
    AsConstantId().Print(out, /*disambiguate=*/false);
  }
  out << ")";
}

auto LibraryNameId::ForStringLiteralValueId(StringLiteralValueId id)
    -> LibraryNameId {
  CARBON_CHECK(id.index >= InvalidIndex, "Unexpected library name ID {0}", id);
  if (id == StringLiteralValueId::Invalid) {
    // Prior to SemIR, we use invalid to indicate `default`.
    return LibraryNameId::Default;
  } else {
    return LibraryNameId(id.index);
  }
}

auto LibraryNameId::Print(llvm::raw_ostream& out) const -> void {
  if (*this == Default) {
    out << Label << "Default";
  } else if (*this == Error) {
    out << Label << "<error>";
  } else {
    IdBase::Print(out);
  }
}

auto LocId::Print(llvm::raw_ostream& out) const -> void {
  out << Label << "_";
  if (is_node_id() || !is_valid()) {
    out << node_id();
  } else {
    out << import_ir_inst_id();
  }
}

}  // namespace Carbon::SemIR
