/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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
#include "backend/optimizer/pass/convert_tuple_output_to_maketuple.h"

#include <algorithm>
#include <memory>

#include "backend/session/anf_runtime_algorithm.h"
#include "backend/optimizer/common/helper.h"
#include "backend/session/kernel_graph.h"

namespace mindspore {
namespace opt {
namespace {
CNodePtr ConvertTupleOuputToPlantInputs(const FuncGraphPtr &graph, const AnfNodePtr &input_node) {
  MS_EXCEPTION_IF_NULL(graph);
  if (!AnfAlgo::IsTupleOutput(input_node)) {
    MS_LOG(EXCEPTION) << "Cannot using the function to convert a not tuple output node to maketuple!";
  }
  if (input_node->isa<CNode>()) {
    MS_LOG(EXCEPTION) << "The function can only split a parameter or valuenode bug got " << input_node->DebugString();
  }
  std::vector<AnfNodePtr> convert_inputs = {NewValueNode(prim::kPrimMakeTuple)};
  auto kernel_graph = graph->cast<KernelGraphPtr>();
  MS_EXCEPTION_IF_NULL(kernel_graph);
  auto splited_node_list = kernel_graph->SplitTupleOutputNodeToNodeList(input_node);
  for (const auto &node : splited_node_list) {
    if (AnfAlgo::IsTupleOutput(node)) {
      convert_inputs.emplace_back(ConvertTupleOuputToPlantInputs(graph, node));
      continue;
    }
    convert_inputs.emplace_back(node);
  }

  auto make_tuple = graph->NewCNode(convert_inputs);
  std::vector<abstract::AbstractBasePtr> abstract_list;
  auto make_tuple_input_size = AnfAlgo::GetInputTensorNum(make_tuple);
  for (size_t index = 0; index < make_tuple_input_size; ++index) {
    auto make_tuple_input = AnfAlgo::GetInputNode(make_tuple, index);
    MS_EXCEPTION_IF_NULL(make_tuple_input);
    abstract_list.emplace_back(make_tuple_input->abstract());
  }
  make_tuple->set_abstract(std::make_shared<abstract::AbstractTuple>(abstract_list));
  return make_tuple;
}

CNodePtr ConvertTupleInputToMakeTuple(const FuncGraphPtr &graph, const CNodePtr &cnode_ptr) {
  MS_EXCEPTION_IF_NULL(cnode_ptr);
  MS_EXCEPTION_IF_NULL(graph);
  std::vector<AnfNodePtr> convert_inputs = {cnode_ptr->input(0)};
  for (size_t index = 0; index < AnfAlgo::GetInputTensorNum(cnode_ptr); ++index) {
    auto input_node = AnfAlgo::GetInputNode(cnode_ptr, index);
    if (AnfAlgo::IsTupleOutput(input_node)) {
      std::vector<TypeId> types;
      std::vector<std::vector<size_t>> shapes;
      std::vector<AnfNodePtr> make_tuple_inputs_list = {NewValueNode(prim::kPrimMakeTuple)};
      if (input_node->isa<CNode>()) {
        for (size_t tuple_out_index = 0; tuple_out_index < AnfAlgo::GetOutputTensorNum(input_node); ++tuple_out_index) {
          make_tuple_inputs_list.emplace_back(CreatTupleGetItemNode(graph, input_node, tuple_out_index));
          types.push_back(AnfAlgo::GetOutputInferDataType(input_node, tuple_out_index));
          shapes.emplace_back(AnfAlgo::GetOutputInferShape(input_node, tuple_out_index));
        }
        auto make_tuple = graph->NewCNode(make_tuple_inputs_list);
        AnfAlgo::SetOutputInferTypeAndShape(types, shapes, make_tuple.get());
        convert_inputs.emplace_back(make_tuple);
        continue;
      }
      convert_inputs.emplace_back(ConvertTupleOuputToPlantInputs(graph, input_node));
    } else {
      convert_inputs.push_back(input_node);
    }
  }
  auto new_node = graph->NewCNode(convert_inputs);
  new_node->set_abstract(cnode_ptr->abstract());
  return new_node;
}
}  // namespace

const BaseRef ConvertTupleOutputToMaketuple::DefinePattern() const {
  VarPtr V = std::make_shared<Var>();
  VarPtr Xs = std::make_shared<SeqVar>();
  return VectorRef({V, Xs});
}

const AnfNodePtr ConvertTupleOutputToMaketuple::Process(const FuncGraphPtr &func_graph, const AnfNodePtr &node,
                                                        const EquivPtr &) const {
  if (node == nullptr || !node->isa<CNode>()) {
    return nullptr;
  }
  auto cnode = node->cast<CNodePtr>();
  MS_EXCEPTION_IF_NULL(cnode);
  if (IsPrimitiveCNode(cnode, prim::kPrimTupleGetItem) || IsPrimitiveCNode(cnode, prim::kPrimControlDepend)) {
    return nullptr;
  }
  if (std::any_of(cnode->inputs().begin() + 1, cnode->inputs().end(), [](const AnfNodePtr &node) {
        return node->Type() != nullptr && AnfAlgo::IsRealKernel(node) && AnfAlgo::IsTupleOutput(node);
      })) {
    return ConvertTupleInputToMakeTuple(func_graph, cnode);
  }
  return nullptr;
}
}  // namespace opt
}  // namespace mindspore
