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

#ifndef __TRANSLATABLE__H__
#define __TRANSLATABLE__H__

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include <iostream>

namespace nballerina {

class Translatable {
  public:
    virtual void translate(llvm::Module &module, llvm::IRBuilder<> &builder) = 0;
};

/*
 * Visitor (Translator) generic class definitions
 */
class TranslatorBase {
  public:
    virtual ~TranslatorBase() = default;
};

template <typename TranslatableType>
class Translator {
  public:
    virtual void visit(TranslatableType &p, llvm::Module &module, llvm::IRBuilder<> &builder) = 0;
};

template <typename... V>
class Translators;

template <typename V1>
class Translators<V1> : public Translator<V1>, virtual public TranslatorBase {};

template <typename V1, typename... V>
class Translators<V1, V...> : public Translator<V1>, public Translators<V...> {};

/*
 * Visitee (Translatable) generic class definitions
 */

class TranslatableInterface {
  public:
    virtual void accept(TranslatorBase &v, llvm::Module &module, llvm::IRBuilder<> &builder) = 0;
};

template <typename TranslatableInterface, typename TranslatableType>
class TranslatableBase : virtual public TranslatableInterface {
  public:
    void accept(TranslatorBase &vb, llvm::Module &module, llvm::IRBuilder<> &builder) override {
        if (Translator<TranslatableType> *v = dynamic_cast<Translator<TranslatableType> *>(&vb)) {
            v->visit(*static_cast<TranslatableType *>(this), module, builder);
        } else { // Handle error
            llvm_unreachable("Invalid cast to Translator<TranslatableType>");
        }
    }
};

template <typename TranslatableType>
using TranslatableNew = TranslatableBase<TranslatableInterface, TranslatableType>;

} // namespace nballerina

#endif //!__TRANSLATABLE__H__
