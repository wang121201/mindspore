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

#ifndef MINDSPORE_CCSRC_MINDDATA_DATASET_ENGINE_IR_DATASETOPS_CONCAT_NODE_H_
#define MINDSPORE_CCSRC_MINDDATA_DATASET_ENGINE_IR_DATASETOPS_CONCAT_NODE_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "minddata/dataset/engine/ir/datasetops/dataset_node.h"

namespace mindspore {
namespace dataset {

class ConcatNode : public DatasetNode {
 public:
  /// \brief Constructor
  explicit ConcatNode(const std::vector<std::shared_ptr<DatasetNode>> &datasets,
                      const std::shared_ptr<SamplerObj> &sampler = nullptr,
                      const std::vector<std::pair<int, int>> &children_flag_and_nums = {},
                      const std::vector<std::pair<int, int>> &children_start_end_index = {});

  /// \brief Destructor
  ~ConcatNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::shared_ptr<SamplerObj> sampler_;
  std::vector<std::pair<int, int>> children_flag_and_nums_;
  std::vector<std::pair<int, int>> children_start_end_index_;
};

}  // namespace dataset
}  // namespace mindspore
#endif  // MINDSPORE_CCSRC_MINDDATA_DATASET_ENGINE_IR_DATASETOPS_CONCAT_NODE_H_
