// Part of the Carbon Language project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef CARBON_TOOLCHAIN_BASE_VALUE_IDS_H_
#define CARBON_TOOLCHAIN_BASE_VALUE_IDS_H_

#include "common/ostream.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/YAMLParser.h"
#include "toolchain/base/index_base.h"

namespace Carbon {

// The value of a real literal token.
//
// This is either a dyadic fraction (mantissa * 2^exponent) or a decadic
// fraction (mantissa * 10^exponent).
//
// These values are not canonicalized, because we don't expect them to repeat
// and don't use them in SemIR values.
struct Real : public Printable<Real> {
  auto Print(llvm::raw_ostream& output_stream) const -> void {
    mantissa.print(output_stream, /*isSigned=*/false);
    output_stream << "*" << (is_decimal ? "10" : "2") << "^" << exponent;
  }

  // The mantissa, represented as an unsigned integer.
  llvm::APInt mantissa;

  // The exponent, represented as a signed integer.
  llvm::APInt exponent;

  // If false, the value is mantissa * 2^exponent.
  // If true, the value is mantissa * 10^exponent.
  // TODO: This field increases Real from 32 bytes to 40 bytes. Consider
  // changing how it's tracked for space savings.
  bool is_decimal;
};

// Corresponds to a float value represented by an APFloat. This is used for
// floating-point values in SemIR.
struct FloatId : public IdBase<FloatId> {
  static constexpr llvm::StringLiteral Label = "float";
  using ValueType = llvm::APFloat;
  static const FloatId Invalid;
  using IdBase::IdBase;
};
constexpr FloatId FloatId::Invalid(FloatId::InvalidIndex);

// Corresponds to a Real value.
struct RealId : public IdBase<RealId> {
  static constexpr llvm::StringLiteral Label = "real";
  using ValueType = Real;
  static const RealId Invalid;
  using IdBase::IdBase;
};
constexpr RealId RealId::Invalid(RealId::InvalidIndex);

// Corresponds to StringRefs for identifiers.
//
// `NameId` relies on the values of this type other than `Invalid` all being
// non-negative.
struct IdentifierId : public IdBase<IdentifierId> {
  static constexpr llvm::StringLiteral Label = "identifier";
  using ValueType = llvm::StringRef;
  static const IdentifierId Invalid;
  using IdBase::IdBase;
};
constexpr IdentifierId IdentifierId::Invalid(IdentifierId::InvalidIndex);

// Corresponds to StringRefs for string literals.
struct StringLiteralValueId : public IdBase<StringLiteralValueId> {
  static constexpr llvm::StringLiteral Label = "string";
  using ValueType = llvm::StringRef;
  static const StringLiteralValueId Invalid;
  using IdBase::IdBase;
};
constexpr StringLiteralValueId StringLiteralValueId::Invalid(
    StringLiteralValueId::InvalidIndex);

}  // namespace Carbon

#endif  // CARBON_TOOLCHAIN_BASE_VALUE_IDS_H_
