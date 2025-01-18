// Part of the Carbon Language project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "toolchain/check/context.h"
#include "toolchain/check/handle.h"

namespace Carbon::Check {

auto HandleParseNode(Context& context, Parse::ImplicitParamListStartId node_id)
    -> bool {
  context.node_stack().Push(node_id);
  context.param_and_arg_refs_stack().Push();
  context.BeginSubpattern();
  return true;
}

auto HandleParseNode(Context& context, Parse::ImplicitParamListId node_id)
    -> bool {
  if (context.node_stack().PeekIs(Parse::NodeKind::ImplicitParamListStart)) {
    // End the subpattern started by a trailing comma, or the opening delimiter
    // of an empty list.
    context.EndSubpatternAsEmpty();
  }
  // Note the Start node remains on the stack, where the param list handler can
  // make use of it.
  auto refs_id = context.param_and_arg_refs_stack().EndAndPop(
      Parse::NodeKind::ImplicitParamListStart);
  context.node_stack().Push(node_id, refs_id);
  // The implicit parameter list's scope extends to the end of the following
  // parameter list.
  return true;
}

auto HandleParseNode(Context& context, Parse::TuplePatternStartId node_id)
    -> bool {
  context.node_stack().Push(node_id);
  context.param_and_arg_refs_stack().Push();
  context.BeginSubpattern();
  return true;
}

auto HandleParseNode(Context& context, Parse::PatternListCommaId /*node_id*/)
    -> bool {
  context.param_and_arg_refs_stack().ApplyComma();
  context.BeginSubpattern();
  return true;
}

auto HandleParseNode(Context& context, Parse::TuplePatternId node_id) -> bool {
  if (context.node_stack().PeekIs(Parse::NodeKind::TuplePatternStart)) {
    // End the subpattern started by a trailing comma, or the opening delimiter
    // of an empty list.
    context.EndSubpatternAsEmpty();
  }
  // Note the Start node remains on the stack, where the param list handler can
  // make use of it.
  auto refs_id = context.param_and_arg_refs_stack().EndAndPop(
      Parse::NodeKind::TuplePatternStart);
  context.node_stack().Push(node_id, refs_id);
  return true;
}

}  // namespace Carbon::Check
