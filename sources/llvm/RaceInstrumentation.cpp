#include <map>

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


namespace {
using namespace llvm;

struct RaceInstrumentation : public FunctionPass {
  static char ID;
  StructType *thread_data_ty = nullptr;

  RaceInstrumentation() : FunctionPass(ID) {}

  StructType *getThreadDataTy(LLVMContext& ctx) {
    if (!thread_data_ty) {
      thread_data_ty = StructType::create("struct.__vrd_thread_data",
                                          Type::getInt8PtrTy(ctx),
                                          Type::getInt64Ty(ctx));
    }
    return thread_data_ty;
  }

  void getAnalysisUsage(AnalysisUsage &Info) const override {
      Info.setPreservesAll();
  }

  bool runOnFunction(Function& F) override {
      if (F.isDeclaration())
          return false;
     errs() << "Instrumenting: ";
     errs().write_escaped(F.getName()) << '\n';

     bool changed = false;
     for (auto& BB: F) {
        changed |= runOnBasicBlock(BB);
     }

     return changed;
  }

  bool runOnBasicBlock(BasicBlock& block);

  void instrumentThreadCreate(CallInst *call, Value *data);

  void instrumentTSanFuncEntry(CallInst *call);

  void instrumentThreadFunExit(Function *fun);
};

static inline Value *getThreadCreateData(Function *fun, CallInst *call) {
    if (fun->getName().equals("thrd_create")) {
        return call->getOperand(2);
    } else  if (fun->getName().equals("pthread_create")) {
        return call->getOperand(3);
    }

    return nullptr;
}


static inline Value *getMutexLock(Function *fun, CallInst *call) {
    if (fun->getName().equals("mtx_lock") || fun->getName().equals("pthread_mutex_lock")) {
        return call->getOperand(0)->stripPointerCasts();
    }

    return nullptr;
}

static inline Value *getMutexUnlock(Function *fun, CallInst *call) {
    if (fun->getName().equals("mtx_unlock") || fun->getName().equals("pthread_mutex_unlock")) {
        return call->getOperand(0)->stripPointerCasts();
    }

    return nullptr;
}

static inline bool isTSanFuncEntry(Function *fun) {
    return fun->getName().equals("__tsan_func_entry");
}

void RaceInstrumentation::instrumentThreadCreate(CallInst *call, Value *data) {
    Module *module = call->getModule();
    LLVMContext &ctx = module->getContext();
    auto *dataTy = getThreadDataTy(ctx);
    auto *alloc = new AllocaInst(dataTy, 0, "__vrd_thr_data", call);

    std::vector<Value *> indices_fst = {
        Constant::getIntegerValue(Type::getInt32Ty(ctx), APInt(32, 0)),
        Constant::getIntegerValue(Type::getInt32Ty(ctx), APInt(32, 0))
    };
    std::vector<Value *> indices_snd = {
        Constant::getIntegerValue(Type::getInt32Ty(ctx), APInt(32, 0)),
        Constant::getIntegerValue(Type::getInt32Ty(ctx), APInt(32, 1))
    };
    auto *alloc_fst = GetElementPtrInst::Create(dataTy, alloc, indices_fst, "", call);
    auto *alloc_snd = GetElementPtrInst::Create(dataTy, alloc, indices_snd, "", call);

    new StoreInst(data, alloc_fst, call);
    const FunctionCallee &tid_fun = module->getOrInsertFunction("__vrd_new_thrd_id",
                                                                Type::getInt64Ty(ctx));
    auto *tid_call = CallInst::Create(tid_fun, {}, "", call);
    tid_call->setDebugLoc(call->getDebugLoc());
    new StoreInst(tid_call, alloc_snd, call);

    CastInst *cast = CastInst::CreatePointerCast(alloc, Type::getInt8PtrTy(ctx), "", call);
    call->replaceUsesOfWith(data, cast);

    const FunctionCallee &instr_fun = module->getOrInsertFunction("__vrd_thrd_create",
                                                                  Type::getVoidTy(ctx),
                                                                  Type::getInt64Ty(ctx));
    std::vector<Value *> args = {tid_call};
    auto *new_call = CallInst::Create(instr_fun, args, "");
    new_call->setDebugLoc(call->getDebugLoc());
    new_call->insertAfter(call);
}

static void insertMutexLockOrUnlock(CallInst *call, Value *mtx, const std::string& fun) {
    Module *module = call->getModule();
    LLVMContext &ctx = module->getContext();
    const FunctionCallee &instr_fun = module->getOrInsertFunction(fun,
                                                                  Type::getVoidTy(ctx),
                                                                  Type::getInt8PtrTy(ctx));
    CastInst *cast = CastInst::CreatePointerCast(mtx, Type::getInt8PtrTy(ctx));
    std::vector<Value *> args = {cast};
    auto *new_call = CallInst::Create(instr_fun, args, "", call);
    new_call->setDebugLoc(call->getDebugLoc());
    cast->insertBefore(new_call);
}

DebugLoc findFirstDbgLoc(Instruction *I) {
    for (const auto& inst : *I->getParent()) {
        auto &DL = inst.getDebugLoc();
        if (DL)
            return DL;
    }
    assert(false && "Found no debugging loc");
    return DebugLoc();
}

void RaceInstrumentation::instrumentTSanFuncEntry(CallInst *call) {
    Module *module = call->getModule();
    Function *fun = call->getFunction();
    LLVMContext &ctx = module->getContext();
    auto *dataTy = getThreadDataTy(ctx);

    /* __tsan_fun_entry may not have dbgloc, workaround that */
    if (!call->getDebugLoc())
        call->setDebugLoc(findFirstDbgLoc(call));

    const FunctionCallee &instr_fun = module->getOrInsertFunction("__vrd_thrd_entry",
                                                                  Type::getVoidTy(ctx),
                                                                  Type::getInt64Ty(ctx));

    if (fun->getName().equals("main")) {
        std::vector<Value *> args = {Constant::getIntegerValue(Type::getInt64Ty(ctx), APInt(64, 0))};
        auto *new_call = CallInst::Create(instr_fun, args, "", call);
        new_call->setDebugLoc(call->getDebugLoc());
        instrumentThreadFunExit(fun);
        return;
    }
    if (fun->arg_size() != 1) {
        errs() << "Ignoring func_entry in function " << fun->getName() << " due to wrong number of operands\n" ;
        return;
    }

    std::vector<Value *> indices_snd = {
        Constant::getIntegerValue(Type::getInt32Ty(ctx), APInt(32, 0)),
        Constant::getIntegerValue(Type::getInt32Ty(ctx), APInt(32, 1))
    };
    Value *arg = fun->getArg(0);
    auto *cast = CastInst::CreateBitOrPointerCast(arg, dataTy->getPointerTo(), "", call);
    auto *data_snd = GetElementPtrInst::Create(dataTy, cast, indices_snd, "", call);
    auto *load = new LoadInst(Type::getInt64Ty(ctx), data_snd, "", call);

    std::vector<Value *> args = {load};
    auto *new_call = CallInst::Create(instr_fun, args, "", call);
    new_call->setDebugLoc(call->getDebugLoc());

    instrumentThreadFunExit(fun);
}

static inline bool blockHasNoSuccessors(BasicBlock &block) {
    return succ_begin(&block) == succ_end(&block);
}

static bool instrumentNoreturn(BasicBlock &block, const FunctionCallee &instr_fun) {
    for (auto& I : block) {
        if (auto *call = dyn_cast<CallInst>(&I)) {
            if (auto *fun = call->getCalledFunction()) {
                if (fun->hasFnAttribute(Attribute::NoReturn)) {
                    auto *new_call = CallInst::Create(instr_fun, {}, "", &I);
                    new_call->setDebugLoc(call->getDebugLoc());
                    return true;
                }
            }
        }
    }
    return false;
}

void RaceInstrumentation::instrumentThreadFunExit(Function *fun) {
    Module *module = fun->getParent();
    LLVMContext &ctx = module->getContext();
    const FunctionCallee &instr_fun = module->getOrInsertFunction("__vrd_thrd_exit",
                                                                  Type::getVoidTy(ctx));

    for (auto &block : *fun) {
        if (blockHasNoSuccessors(block)) {
            if (instrumentNoreturn(block, instr_fun)) {
                continue;
            }

            /* Failed finding noreturn call, so insert the call before the terminator */
            auto *new_call = CallInst::Create(instr_fun, {}, "", block.getTerminator());
            new_call->setDebugLoc(block.getTerminator()->getDebugLoc());
        }
    }
}


bool RaceInstrumentation::runOnBasicBlock(BasicBlock& block) {
    for (auto& I : block) {
        if (CallInst *call = dyn_cast<CallInst>(&I)) {
            auto *calledop = call->getCalledOperand()->stripPointerCastsAndAliases();
            auto *calledfun = dyn_cast<Function>(calledop);
            if (calledfun == nullptr) {
                errs() << "A call via function pointer ignored: " << *call << "\n";
                continue;
            }

            if (auto *mtx = getMutexLock(calledfun, call)) {
                insertMutexLockOrUnlock(call, mtx, "__vrd_mutex_lock");
            } else if (auto *mtx = getMutexUnlock(calledfun, call)) {
                insertMutexLockOrUnlock(call, mtx, "__vrd_mutex_unlock");
            } else if (auto *data = getThreadCreateData(calledfun, call)) {
                instrumentThreadCreate(call, data);
            } else if (isTSanFuncEntry(calledfun)) {
                instrumentTSanFuncEntry(call);
            }
        }
    }

    return false;
}

}  // namespace

char RaceInstrumentation::ID = 0;
static RegisterPass<RaceInstrumentation> VRD("vamos-race-instrumentation",
                                   "VAMOS race instrumentation pass",
                                   true /* Only looks at CFG */,
                                   true /* Analysis Pass */);

static RegisterStandardPasses VRDP(
    PassManagerBuilder::EP_FullLinkTimeOptimizationLast,
    [](const PassManagerBuilder &,
       legacy::PassManagerBase &PM) { PM.add(new RaceInstrumentation()); });
