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

#ifndef MINDSPORE_CCSRC_FRONTEND_PARALLEL_OPS_INFO_CONV2D_INFO_H_
#define MINDSPORE_CCSRC_FRONTEND_PARALLEL_OPS_INFO_CONV2D_INFO_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "ir/value.h"
#include "frontend/parallel/auto_parallel/operator_costmodel.h"
#include "frontend/parallel/ops_info/operator_info.h"
#include "frontend/parallel/strategy.h"

namespace mindspore {
namespace parallel {
class Conv2DInfo : public OperatorInfo {
 public:
  Conv2DInfo(const std::string &operator_name, const Shapes &inputs_shape, const Shapes &outputs_shape,
             const PrimitiveAttrs &attrs)
      : OperatorInfo(operator_name, inputs_shape, outputs_shape, attrs, std::make_shared<BatchParallelCost>()) {}
  ~Conv2DInfo() override = default;

  Status Init(const StrategyPtr &strategy) override;
  Status InitForCostModel(const StrategyPtr &strategy) override;
  std::vector<StrategyPtr> GenerateOpStrategies(int64_t) override;
  Status SetCostUnderStrategy(const StrategyPtr &) override;
  void ReComputeBatchSplitFlagList() override;

 protected:
  Status GetAttrs() override;
  Status CheckStrategy(const StrategyPtr &strategy) override;
  Status CheckHWStrategy(int64_t h_strategy, int64_t w_strategy);
  Status InferForwardCommunication() override;
  Status InferDevMatrixShape() override;
  Status InferTensorMap() override;
  ReplaceGraphPtr replace_graph(const CNodePtr &cnode) override;

 private:
  int64_t out_channel_ = 1;
  std::vector<int64_t> kernel_size_;  // two integers
  int64_t mode_ = 1;
  int64_t pad_mode_ = 0;           // "pad": 0; "same": 1; "valid": 2;
  std::vector<int64_t> pad_list_;  // four integers
  std::vector<int64_t> stride_;    // four integers
  std::vector<int64_t> dilation_;  // four integers
  int64_t group_ = 1;
  std::string format_;
  bool out_channel_shard_ = false;
  int64_t new_out_channel_ = 1;
};

constexpr size_t IN_CHANNEL_INDEX = 1;
}  // namespace parallel
}  // namespace mindspore

#endif  // MINDSPORE_CCSRC_FRONTEND_PARALLEL_OPS_INFO_CONV2D_INFO_H_
