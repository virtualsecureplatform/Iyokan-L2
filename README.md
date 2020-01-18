# Iyokan-L2
![Iyokanロゴ-no-ring](https://user-images.githubusercontent.com/33079554/72664800-924e9780-3a45-11ea-9877-859a794c91c8.jpg)

# Build
## Dependency
please install below software before build
```
tfhe
libtbb2
libtbb-dev
```

Iyokan-L2 needs VSP Core Cirucit. We need to create the circuit file by Iyokan-L1.
## Procedure
```
cmake .
make
```

# Run
```
PlainMode
    ./iyokanl2 -l <logic_file_name> -p <plain_file_name> -o <result_file_name>
TestMode
    ./iyokanl2 -l <logic_file_name> -c <cycle_num> -t <thread_num> -i <cipher_file_name> -o <result_file_name> -s <secret_key_file_name>
CipherMode
    ./iyokanl2 -l <logic_file_name> -c <cycle_num> -t <thread_num> -i <cipher_file_name> -o <result_file_name>
```

`vsp-core.json` is official VSP Core circuit file. Please use this file as logicFile.

Run on PlainMode
```
src/iyokanl2 -l vsp-core.json -p hoge.plain -o hoge.result
```

Run on TestMode
```
src/iyokanl2 -c 10 -t 10 -l vsp-core.json -i hoge.cipher -o hoge.result -s secret.key
```

Run on CipherMode
```
src/iyokanl2 -c 10 -t 10 -l vsp-core.json -i hoge.cipher -o hoge.result
```

# Changelog

## v0.2.0 (2019/12/21)
Add Feature
- cuFHE Backend(Run on GPU)

## v0.1.1 (2019/12/16)
Add Feature
- PlainMode(Calculate exec cycle)

## v0.1.0 (2019/12/12)
First Alpha Release
- Implemented CipherMode, TestMode

## v0.0.1 (2019/12/04)
PoC Implementation
