/*
 * Copyright 2016 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// Removes duplicate functions. That can happen due to C++ templates,
// and also due to types being different at the source level, but
// identical when finally lowered into concrete wasm code.
//

#include <wasm.h>
#include <pass.h>
#include <ast_utils.h>

namespace wasm {

struct FunctionHasher : public PostWalker<FunctionHasher, Visitor<FunctionHasher>> {
  bool isFunctionParallel() { return true; }

  FunctionHasher* create() override {
    auto* ret = new FunctionHasher;
    ret->setOutput(output);
    return ret;
  }

  void setOutput(std::map<Function*, uint32_t>* output_) {
    output = output_;
  }

  void walk(Expression*& root) {
    assert(digest == 0);
    auto* func = getFunction();
    hash(func->getNumParams());
    for (auto type : func->params) hash(type);
    hash(func->getNumVars());
    for (auto type : func->vars) hash(type);
    hash(func->result);
    hash64(func->type.is() ? uint64_t(func->type.str) : uint64_t(0));
    hash(ExpressionAnalyzer::hash(root));
    output->at(func) = digest;
  }

private:
  std::map<Function*, uint32_t>* output;
  uint32_t digest = 0;

  void hash(uint32_t hash) {
    digest = rehash(digest, hash);
  }
  void hash64(uint64_t hash) {
    digest = rehash(rehash(digest, hash >> 32), uint32_t(hash));
  };
};

struct FunctionReplacer : public PostWalker<FunctionReplacer, Visitor<FunctionReplacer>> {
  bool isFunctionParallel() { return true; }

  FunctionReplacer* create() override {
    auto* ret = new FunctionReplacer;
    ret->setReplacements(replacements);
    return ret;
  }

  void setReplacements(std::map<Name, Name>* replacements_) {
    replacements = replacements_;
  }

  void visitCall(Call* curr) {
    auto iter = replacements->find(curr->target);
    if (iter != replacements->end()) {
      curr->target = iter->second;
    }
  }

private:
  std::map<Name, Name>* replacements;
};

struct DuplicateFunctionElimination : public Pass {
  void run(PassRunner* runner, Module* module) override {
    while (1) {
      // Hash all the functions
      hashes.clear();
      for (auto& func : module->functions) {
        hashes[func.get()] = 0; // ensure an entry for each function - we must not modify the map shape in parallel, just the values
      }
      FunctionHasher hasher;
      hasher.setOutput(&hashes);
      hasher.startWalk(module);
      // Find hash-equal groups
      std::map<uint32_t, std::vector<Function*>> hashGroups;
      for (auto& func : module->functions) {
        hashGroups[hashes[func.get()]].push_back(func.get());
      }
      // Find actually equal functions and prepare to replace them
      std::map<Name, Name> replacements;
      std::set<Name> duplicates;
      for (auto& pair : hashGroups) {
        auto& group = pair.second;
        if (group.size() == 1) continue;
        // pick a base for each group, and try to replace everyone else to it. TODO: multiple bases per hash group, for collisions
        Function* base = group[0];
        for (auto* func : group) {
          if (func != base && equal(func, base)) {
            replacements[func->name] = base->name;
            duplicates.insert(func->name);
          }
        }
      }
      // perform replacements
      if (replacements.size() > 0) {
        // remove the duplicates
        auto& v = module->functions;
        v.erase(std::remove_if(v.begin(), v.end(), [&](const std::unique_ptr<Function>& curr) {
          return duplicates.count(curr->name) > 0;
        }), v.end());
        module->updateFunctionsMap();
        // replace direct calls
        FunctionReplacer replacer;
        replacer.setReplacements(&replacements);
        replacer.startWalk(module);
        // replace in table
        for (auto& name : module->table.names) {
          auto iter = replacements.find(name);
          if (iter != replacements.end()) {
            name = iter->second;
          }
        }
        // replace in start
        if (module->start.is()) {
          auto iter = replacements.find(module->start);
          if (iter != replacements.end()) {
            module->start = iter->second;
          }
        }
        // replace in exports
        for (auto& exp : module->exports) {
          auto iter = replacements.find(exp->value);
          if (iter != replacements.end()) {
            exp->value = iter->second;
          }
        }
      } else {
        break;
      }
    }
  }

private:
  std::map<Function*, uint32_t> hashes;

  bool equal(Function* left, Function* right) {
    if (left->getNumParams() != right->getNumParams()) return false;
    if (left->getNumVars() != right->getNumVars()) return false;
    for (Index i = 0; i < left->getNumLocals(); i++) {
      if (left->getLocalType(i) != right->getLocalType(i)) return false;
    }
    if (left->result != right->result) return false;
    if (left->type != right->type) return false;
    return ExpressionAnalyzer::equal(left->body, right->body);
  }
};

static RegisterPass<DuplicateFunctionElimination> registerPass("duplicate-function-elimination", "removes duplicate functions");

} // namespace wasm

