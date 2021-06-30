#!/bin/bash
# Copyright 2021 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================
# an simple tutorial as follows, more parameters can be setting
if [ $# != 4 ]
then
    echo "Usage: sh run_standalone_eval_ascend.sh [DATA_PATH] [CKPT_PATH] [DEVICE_ID] [EVAL_CLASS]"
exit 1
fi

export DATA_PATH=$1
export CKPT_PATH=$2
export DEVICE_ID=$3
export EVAL_CLASS=$4

python eval.py --data_path=$DATA_PATH --ckpt_path=$CKPT_PATH \
               --device_id=$DEVICE_ID --device_target="Ascend"  \
               --classes_per_it_val=EVAL_CLASS > eval_log 2>&1 &
