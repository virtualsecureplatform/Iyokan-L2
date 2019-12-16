# Iyokan-L2

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
src/iyokanl2 [-p <plain_file_name> ] [-v] -c <cycle> -t <thread_num> -l <logic_file_name> -i <cipher_file_name> -o <result_file_name> [-s <secretKeyFile>]
-p : plainMode(exec Plain and calculate exec cycle)
-v : verbose(output exec logs)
-c : cycleNum(specify exec cycle num)
-t : threadNum(specify exec threadNum)
-l : logicFileName(specify exec logicFileName(VSP Core circuit file))
-o : resultFileName(specify exec resultFileName)
-s : secretKeyFile(specify secretKeyFile. if use this option, Iyokan-L2 runs on testMode (testMode decrypts cipherFile, exec on plain and output encrypted result file))
```

`vsp-core.json` is official VSP Core circuit file. Please use this file as logicFile.

Run on PlainMode
```
src/iyokanl2 -p hoge.plain -o hoge.result
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

## v0.1.1 (2019/12/16)
Add Feature
- PlainMode(Calculate exec cycle)

## v0.1.0 (2019/12/12)
First Alpha Release
- Implemented CipherMode, TestMode

## v0.0.1 (2019/12/04)
PoC Implementation
