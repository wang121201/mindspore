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

#include "minddata/dataset/engine/ir/datasetops/source/mnist_node.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "minddata/dataset/engine/datasetops/source/mnist_op.h"

#include "minddata/dataset/util/status.h"
namespace mindspore {
namespace dataset {

MnistNode::MnistNode(std::string dataset_dir, std::string usage, std::shared_ptr<SamplerObj> sampler,
                     std::shared_ptr<DatasetCache> cache)
    : DatasetNode(std::move(cache)), dataset_dir_(dataset_dir), usage_(usage), sampler_(sampler) {}

Status MnistNode::ValidateParams() {
  RETURN_IF_NOT_OK(ValidateDatasetDirParam("MnistNode", dataset_dir_));

  RETURN_IF_NOT_OK(ValidateDatasetSampler("MnistNode", sampler_));

  RETURN_IF_NOT_OK(ValidateStringValue("MnistNode", usage_, {"train", "test", "all"}));

  return Status::OK();
}

std::vector<std::shared_ptr<DatasetOp>> MnistNode::Build() {
  // A vector containing shared pointer to the Dataset Ops that this object will create
  std::vector<std::shared_ptr<DatasetOp>> node_ops;

  // Do internal Schema generation.
  auto schema = std::make_unique<DataSchema>();
  RETURN_EMPTY_IF_ERROR(schema->AddColumn(ColDescriptor("image", DataType(DataType::DE_UINT8), TensorImpl::kCv, 1)));
  TensorShape scalar = TensorShape::CreateScalar();
  RETURN_EMPTY_IF_ERROR(
    schema->AddColumn(ColDescriptor("label", DataType(DataType::DE_UINT32), TensorImpl::kFlexible, 0, &scalar)));
  build_status = AddCacheOp(&node_ops);  // remove me after changing return val of Build()
  RETURN_EMPTY_IF_ERROR(build_status);

  node_ops.push_back(std::make_shared<MnistOp>(usage_, num_workers_, rows_per_buffer_, dataset_dir_,
                                               connector_que_size_, std::move(schema), std::move(sampler_->Build())));

  return node_ops;
}

// Get the shard id of node
Status MnistNode::GetShardId(int32_t *shard_id) {
  *shard_id = sampler_->ShardId();

  return Status::OK();
}

}  // namespace dataset
}  // namespace mindspore
