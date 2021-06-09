/**
 * Copyright 2021 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_LITE_TOOLS_OPTIMIZER_GRAPH_CONTROL_FLOW_PASS_H_
#define MINDSPORE_LITE_TOOLS_OPTIMIZER_GRAPH_CONTROL_FLOW_PASS_H_
#include <string>
#include <vector>
#include <unordered_map>
#include "schema/inner/model_generated.h"
#include "tools/converter/converter_flags.h"
#include "backend/optimizer/common/pass.h"

namespace mindspore::opt {
class ControlFlowPass : public Pass {
 public:
  ControlFlowPass() : Pass("control_flow_pass") {}
  ~ControlFlowPass() override = default;
  bool Run(const FuncGraphPtr &fg) override;

 private:
  // utility function
  static ValueNodePtr GetSwitchAnfPrim();
  static ValueNodePtr GetPartialAnfPrim();
  void ReplaceNode(const FuncGraphPtr &fg, const std::unordered_map<AnfNodePtr, AnfNodePtr> &replace_pairs);
  void FunGraphInputsOnlyUsedByAfterParts(const FuncGraphPtr &fg, const CNodePtr &aim_cnode,
                                          std::vector<AnfNodePtr> *fg_inputs_only_used_by_after_fg);
  int SplitGraph(const FuncGraphPtr &fg, const PrimitivePtr &aim_prim, AnfNodePtr *aim_prim_type_node,
                 std::vector<AnfNodePtr> *remain_nodes);
  int CreateAfterGraph(const FuncGraphPtr &main_fg, const std::vector<AnfNodePtr> &remain_nodes,
                       const CNodePtr &aim_cnode, FuncGraphPtr *after_fg);

  // process while
  int CreateWhileCondCallNode(
    const FuncGraphPtr &fg, const CNodePtr &while_cnode, std::vector<AnfNodePtr> *fg_inputs_only_used_by_after_fg,
    CNodePtr *cond_partial_cnode,
    std::unordered_map<AnfNodePtr, AnfNodePtr> *fg_inputs_and_after_partial_inputs_replace_pairs);
  int CreateWhileBodyPartialNode(const FuncGraphPtr &cond_fg, const CNodePtr &while_cnode, CNodePtr *body_partial_node);
  int CreateWhileAfterPartialNode(
    const FuncGraphPtr &main_fg, const FuncGraphPtr &cond_fg, const std::vector<AnfNodePtr> &remain_nodes,
    const std::vector<AnfNodePtr> &fg_inputs_only_used_by_after_fg,
    const std::unordered_map<AnfNodePtr, AnfNodePtr> &fg_inputs_and_after_partial_inputs_replace_pairs,
    CNodePtr *while_cnode, CNodePtr *after_partial_cnode);
  int ProcessWhileOp(const FuncGraphPtr &fg);

  // process if
  int CreateIfPartialNode(const FuncGraphPtr &fg, const std::vector<AnfNodePtr> &fg_inputs_only_used_by_after_partial,
                          const size_t &index, CNodePtr *if_cnode, FuncGraphPtr *after_fg,
                          CNodePtr *then_partial_cnode);
  int CreateIfThenPartialNode(const FuncGraphPtr &main_fg,
                              const std::vector<AnfNodePtr> &fg_inputs_only_used_by_after_partial, CNodePtr *if_cnode,
                              FuncGraphPtr *after_fg, CNodePtr *then_partial_cnode);
  int CreateIfElsePartialNode(const FuncGraphPtr &main_fg,
                              const std::vector<AnfNodePtr> &fg_inputs_only_used_by_after_partial, CNodePtr *if_cnode,
                              FuncGraphPtr *after_fg, CNodePtr *else_partial_cnode);
  int ProcessIfOp(const FuncGraphPtr &fg);

  const size_t kCNodePrimIndex = 0;
  const size_t kCNodeFirstInputIndex = 1;
  const size_t kCNodeSecondInputIndex = 2;

  const size_t kGetItemInputSize = 3;
  const size_t kPartialFirstInputSize = 2;

  const size_t kWhileMinInputSize = 3;
  const size_t kWhileCondIndex = 1;
  const size_t kWhileBodyIndex = 2;

  const size_t kIfMinInputSize = 4;
  const size_t kIfThenIndex = 1;
  const size_t kIfElseIndex = 2;
  const size_t kIfCondIndex = 3;
};
}  // namespace mindspore::opt
#endif