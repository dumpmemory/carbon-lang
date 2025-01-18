// Part of the Carbon Language project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "toolchain/sem_ir/type.h"

#include "toolchain/sem_ir/file.h"

namespace Carbon::SemIR {

auto TypeStore::GetInstId(TypeId type_id) const -> InstId {
  return file_->constant_values().GetInstId(GetConstantId(type_id));
}

auto TypeStore::GetAsInst(TypeId type_id) const -> Inst {
  return file_->insts().Get(GetInstId(type_id));
}

auto TypeStore::GetObjectRepr(TypeId type_id) const -> TypeId {
  type_id = GetUnqualifiedType(type_id);
  auto class_type = TryGetAs<ClassType>(type_id);
  if (!class_type) {
    return type_id;
  }
  const auto& class_info = file_->classes().Get(class_type->class_id);
  if (!class_info.is_defined()) {
    return TypeId::Invalid;
  }
  return class_info.GetObjectRepr(*file_, class_type->specific_id);
}

auto TypeStore::GetUnqualifiedType(TypeId type_id) const -> TypeId {
  if (auto const_type = TryGetAs<ConstType>(type_id)) {
    return const_type->inner_id;
  }
  return type_id;
}

static auto TryGetIntTypeInfo(const File& file, TypeId type_id)
    -> std::optional<TypeStore::IntTypeInfo> {
  auto object_repr_id = file.types().GetObjectRepr(type_id);
  if (!object_repr_id.is_valid()) {
    return std::nullopt;
  }
  auto inst_id = file.types().GetInstId(object_repr_id);
  if (inst_id == IntLiteralType::SingletonInstId) {
    // `Core.IntLiteral` has an unknown bit-width.
    return TypeStore::IntTypeInfo{.is_signed = true,
                                  .bit_width = IntId::Invalid};
  }
  auto int_type = file.insts().TryGetAs<IntType>(inst_id);
  if (!int_type) {
    return std::nullopt;
  }
  auto bit_width_inst = file.insts().TryGetAs<IntValue>(int_type->bit_width_id);
  return TypeStore::IntTypeInfo{
      .is_signed = int_type->int_kind.is_signed(),
      .bit_width = bit_width_inst ? bit_width_inst->int_id : IntId::Invalid};
}

auto TypeStore::IsSignedInt(TypeId int_type_id) const -> bool {
  auto int_info = TryGetIntTypeInfo(*file_, int_type_id);
  return int_info && int_info->is_signed;
}

auto TypeStore::GetIntTypeInfo(TypeId int_type_id) const -> IntTypeInfo {
  auto int_info = TryGetIntTypeInfo(*file_, int_type_id);
  CARBON_CHECK(int_info, "Type {0} is not an integer type", int_type_id);
  return *int_info;
}

}  // namespace Carbon::SemIR
