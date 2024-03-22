#include "llvm/Transforms/Utils/TestPass.h"
using namespace llvm;
PreservedAnalyses TestPass::run(Module &M, ModuleAnalysisManager &AM) {
        for (auto &F : M) {
            errs() << "Nome funzione: " << F.getName() << "\n";
            if (F.isVarArg()){
                errs() << "Numero di argomenti " << F.arg_size() << "+*\n";
            } else {
                errs() << "Numero di argomenti" << F.arg_size() << "\n";
            }
            int callCounter = 0;
            for (auto &BB : F) {
                for (auto &I : BB) {
                    if (I.getOpcode() == Instruction::Call) {
                        callCounter++;
                    }
                }
            }
            errs() << "Chiamate:" << callCounter << "\n";
            errs() << "BasicBlocks: " << F.size() << "\n";
            errs() << "Numero di istruzioni: " << F.getInstructionCount() << "\n";
        }
        return PreservedAnalyses::all();
}
// PreservedAnalyses TestPass::run(Function &F, FunctionAnalysisManager &AM) {
//         errs() << "Questa funzione si chiama " << F.getName() << "\n";
//         if (F.isVarArg()){
//             errs() << "La fun "<< F.getName() << " ha" << F.arg_size()<< " argomenti"<< "+*\n";
//         } else {
//             errs() << "Numero di argomenti: " << F.arg_size() << "\n";
//         }
//         int callCounter = 0;
//         for (auto &BB : F) {
//             for (auto &I : BB) {
//                 if (I.getOpcode() == Instruction::Call) {
//                     callCounter++;
//                 }
//             }
//         }
//         errs() << "Numero chiamate" << callCounter << "\n";
//         errs() << "Numero di Basic Blocks " << F.size() << "\n";
//         errs() << "Istruzioni: " << F.getInstructionCount() << "\n";
//         return PreservedAnalyses::all();
// }

