#ifndef LLVM_RUNNER_HPP
#define LLVM_RUNNER_HPP

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace cherry::ir {

    class LLVMRunner {
    public:
        static void run(const ::llvm::Module& module) {
            namespace fs = std::filesystem;

            fs::create_directories("out");

            std::error_code ec;
            ::llvm::raw_fd_ostream out_ll("out/cherry.ll", ec, ::llvm::sys::fs::OF_Text);
            if (ec) {
                std::cerr << "[LLVMRunner] Error writing to out/cherry.ll: " << ec.message() << "\n";
                return;
            }

            module.print(out_ll, nullptr);
            out_ll.close();

            // Compile .ll to .o with llc
            int llc_status = std::system(
                "/opt/homebrew/opt/llvm/bin/llc -march=arm64 -filetype=obj out/cherry.ll -o out/cherry.o"
            );
            if (llc_status != 0) {
                std::cerr << "[LLVMRunner] llc failed to compile .ll to .o\n";
                return;
            }

            // Link .o to executable with clang
            int clang_status = std::system(
                "clang -isysroot $(xcrun --sdk macosx --show-sdk-path) out/cherry.o -o out/cherry_exec"
            );
            if (clang_status != 0) {
                std::cerr << "[LLVMRunner] clang failed to link the object file.\n";
                return;
            }

            // Clear console (compatible with macOS and Linux)
            std::system("clear || cls");

            std::cout << "=== Program Output ===" << std::endl;

            // Run the compiled binary
            int run_status = std::system("./out/cherry_exec");
            int exit_code = WEXITSTATUS(run_status);

            std::cout << "\n=== Exit Code: " << exit_code << " ===" << std::endl;
        }
    };

}

#endif // LLVM_RUNNER_HPP
