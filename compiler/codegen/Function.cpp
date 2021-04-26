/*
 * Copyright (c) 2021, WSO2 Inc. (http://www.wso2.org) All Rights Reserved.
 *
 * WSO2 Inc. licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "Function.h"
#include "BasicBlock.h"
#include "CodeGenUtils.h"
#include "ConditionBrInsn.h"
#include "FunctionParam.h"
#include "Operand.h"
#include "Package.h"
#include "TerminatorInsn.h"
#include "Types.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

namespace nballerina {

Function::Function(Package &parentPackage, std::string name, std::string workerName, unsigned int flags)
    : parentPackage(parentPackage), name(std::move(name)), workerName(std::move(workerName)), flags(flags) {}

const std::string &Function::getName() const { return name; }
const std::vector<FunctionParam> &Function::getParams() const { return requiredParams; }
const std::optional<RestParam> &Function::getRestParam() const { return restParam; }
const std::optional<Variable> &Function::getReturnVar() const { return returnVar; }

void Function::insertParam(const FunctionParam &param) { requiredParams.push_back(param); }
void Function::insertLocalVar(const Variable &var) {
    localVars.insert(std::pair<std::string, Variable>(var.getName(), var));
}
void Function::setReturnVar(const Variable &var) { returnVar = var; }
void Function::insertBasicBlock(const std::shared_ptr<BasicBlock> &bb) { basicBlocks.push_back(bb); }

const Variable &Function::getLocalOrGlobalVariable(const Operand &op) const {
    if (op.getKind() == GLOBAL_VAR_KIND) {
        return parentPackage.getGlobalVariable(op.getName());
    }
    return getLocalVariable(op.getName());
}

const Variable &Function::getLocalVariable(const std::string &opName) const {
    const auto &varIt = localVars.find(opName);
    assert(varIt != localVars.end());
    return varIt->second;
}

const Package &Function::getPackageRef() const { return parentPackage; }

// TODO mutable reference shouldn't be required
Package &Function::getPackageMutableRef() const { return parentPackage; }

size_t Function::getNumParams() const { return requiredParams.size(); }

bool Function::isMainFunction() const { return (name == MAIN_FUNCTION_NAME); }

bool Function::isExternalFunction() const { return ((flags & NATIVE) == NATIVE); }

} // namespace nballerina
