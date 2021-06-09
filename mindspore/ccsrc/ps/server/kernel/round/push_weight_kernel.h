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

#ifndef MINDSPORE_CCSRC_PS_SERVER_KERNEL_PUSH_WEIGHT_KERNEL_H_
#define MINDSPORE_CCSRC_PS_SERVER_KERNEL_PUSH_WEIGHT_KERNEL_H_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ps/server/common.h"
#include "ps/server/kernel/round/round_kernel.h"
#include "ps/server/kernel/round/round_kernel_factory.h"
#include "ps/server/executor.h"

namespace mindspore {
namespace ps {
namespace server {
namespace kernel {
class PushWeightKernel : public RoundKernel {
 public:
  PushWeightKernel() = default;
  ~PushWeightKernel() override = default;

  void InitKernel(size_t threshold_count) override;
  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs);
  bool Reset() override;
  void OnLastCountEvent(const std::shared_ptr<core::MessageHandler> &message) override;

 private:
  void PushWeight(std::shared_ptr<FBBuilder> fbb, const schema::RequestPushWeight *push_weight_req);
  std::map<std::string, Address> ParseFeatureMap(const schema::RequestPushWeight *push_weight_req);
  void BuildPushWeightRsp(std::shared_ptr<FBBuilder> fbb, const schema::ResponseCode retcode,
                          const std::string &reason);

  Executor *executor_;
  uint32_t local_rank_;
};
}  // namespace kernel
}  // namespace server
}  // namespace ps
}  // namespace mindspore
#endif  // MINDSPORE_CCSRC_PS_SERVER_KERNEL_PUSH_WEIGHT_KERNEL_H_