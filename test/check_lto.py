import os
import time
import sys

files = ["native", "c", "rust"]

# Change to build directory
os.chdir("../build")

# Check whether the required files exist
if not os.path.isfile('./nballerinacc'):
    print('Missing nballerinacc!')
    sys.exit(-1)

os.chdir("test/lto")
for file in files:
    if not os.path.isfile(f"{file}-bir-dump"):
        print(f'Missing {file}-bir-dump file!')
        sys.exit(-1)
os.chdir("../../")

if (not os.path.isfile('runtime/rust_rt/release/libballerina_rt.a')) or \
        (not os.path.isfile('runtime/c_rt/libballerina_crt.a')):
    print('Missing Runtime libs!')
    sys.exit(-1)

# Run nballerinacc on BIR dump
for file in files:
    os.system(f'./nballerinacc test/lto/{file}-bir-dump')

# Compile LLVM IR with clang
os.chdir("test/lto")
for file in files:
    os.system(f'clang-11 --target=x86_64-unknown-linux-gnu -c -O3 -flto=thin \
        -Wno-override-module -o {file}.o {file}-bir-dump.ll')

for file in files:
    os.system(f'clang-11 -flto=thin -fuse-ld=lld-11 -L../../runtime/rust_rt/release/ \
        -L../../runtime/c_rt/ -lballerina_rt -lballerina_crt -lpthread -ldl -o {file}.out -O3 {file}.o')

# Run tests and capture elapsed time
elapsed_t = {}
for file in files:
    start = time.perf_counter()
    os.system(f'./{file}.out')
    end = time.perf_counter()
    elapsed_t[file] = end - start

# Check whether execution times are within an order of magnitude
if elapsed_t['rust']/elapsed_t['native'] > 10:
    print('LTO is not inlining Rust runtime functions!')
    sys.exit(-1)

if elapsed_t['c']/elapsed_t['native'] > 10:
    print('LTO is not inlining C runtime functions!')
    sys.exit(-1)
