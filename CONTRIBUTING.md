# Contributing to nBallerina

Ballerina is an open source programming language and platform for cloud-era application programmers to easily write software. It is licensed under the [Apache License](https://www.apache.org/licenses/LICENSE-2.0) and is nurtured by all the contributors of the Ballerina community.

nBallerina is a sub project of Ballerina, which facilitates compiling Ballerina code into platform specific native binary. 

We appreciate your help!

- [Get started](#get-started)
- [Build the source code](#build-the-source-code)
- [Submit your contribution](#submit-your-contribution)
- [Coding Conventions](#coding-conventions)

## Get started

Please read the [Ballerina contribution guide](https://github.com/ballerina-platform/ballerina-lang/blob/master/CONTRIBUTING.md) to understand what you need to know to contribute to overall Ballerina project. This specific guide lists any additional points to note when contributing to nBallerina. 

## Build the source code 

For instructions, see [Building from source](https://github.com/ballerina-platform/nballerina/blob/master/README.md).

## Submit your contribution

1. Do your changes in the source code. Please follow [Coding Conventions](#coding-conventions) section when you write code. 
2. Add unit tests accordingly. (The build process will automatically execute all the tests.)
3. Commit the changes to your fork and push them to the corresponding original repo by sending a Pull Request (PR). 

    >**Tip:** Follow these commit message requirements:

    - Separate subject from the body with a blank line
    - Limit the subject line to 50 characters
    - Capitalize the subject line
    - Do not end the subject line with a period
    - Use the imperative mood in the subject line
    - Wrap the body at 72 characters
    - Use the body to explain what and why vs. how

    In short, a properly formed Git commit subject line should always be able to complete the following sentence:
    <pre>
    If applied, this commit will [your subject line here]
    </pre>
    For more information, go to [How to Write a Git Commit Message](https://chris.beams.io/posts/git-commit/).
4. Accept the Contributor License Agreement (CLA)
    
    You need to Accept the Ballerina Contributor License Agreement (CLA) when prompted by a GitHub email notification after sending your first Pull Request (PR). Subsequent PRs will not require CLA acceptance.

    If the CLA gets changed for some (unlikely) reason, you will be presented with the new CLA text after sending your first PR after the change.

## Coding Conventions
For nBallerina code, we are following [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html) with following changes. 
1. Four space indent. Shouldn't use tab character. Most of the IDEs can be configured to replace tab character with 4 spaces. 
2. Write your code to fit within 120 columns.
3. CamelCase naming conventions. Elaborate function names, class member names and global variable names.  Local variables can be short. Local variables and member variables should start with lowercase letter. 
4. Opening brace of classes, functions, loops, and conditional statements should be on the same line. 
5. Include following license header in all .h and .cpp files. 

    ```cpp
    /*
    * Copyright (c) 2020, WSO2 Inc. (http://www.wso2.org) All Rights Reserved.
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
    ```
