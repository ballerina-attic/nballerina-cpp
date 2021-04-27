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

#include <llvm/IR/IRBuilder.h>

namespace nballerina {
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
    virtual void visit(TranslatableType &visiteeObj, llvm::IRBuilder<> &builder) = 0;
};

template <typename... TranslatableTypes>
class Translators;

template <typename TranslatableType>
class Translators<TranslatableType> : public Translator<TranslatableType>, virtual public TranslatorBase {};

template <typename TranslatableType, typename... TranslatableTypes>
class Translators<TranslatableType, TranslatableTypes...> : public Translator<TranslatableType>,
                                                            public Translators<TranslatableTypes...> {};

/*
 * Visitee (Translatable) generic class definitions
 */

class TranslatableInterface {
  public:
    virtual void accept(TranslatorBase &visitorObj, llvm::IRBuilder<> &builder) = 0;
};

template <typename TranslatableInterface, typename TranslatableType>
class TranslatableBase : virtual public TranslatableInterface {
  public:
    void accept(TranslatorBase &visitorObj, llvm::IRBuilder<> &builder) override {
        if (Translator<TranslatableType> *visitor = dynamic_cast<Translator<TranslatableType> *>(&visitorObj)) {
            visitor->visit(*static_cast<TranslatableType *>(this), builder);
        } else { // Handle error
            llvm_unreachable("Invalid cast to Translator<TranslatableType>");
        }
    }
};

template <typename TranslatableType>
using Translatable = TranslatableBase<TranslatableInterface, TranslatableType>;

} // namespace nballerina

#endif //!__TRANSLATABLE__H__
