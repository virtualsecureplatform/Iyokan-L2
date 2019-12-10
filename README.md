# Iyokan-L2

# Build
## Dependency
please install below package
```
yosys
dotnet
```

## Build
First, Synthesize verilog format file with yosys
```
cp /path/to/VSPCore.v .
yosys build.ys
```

Next, Build Iyokan-L1 and convert yosys result json to original format.
```
dotnet run vsp-core.json vsp-core-converted.json
```
```
dotnet run <intputFileName> <outputFileName>
```
# Changelog
- v0.0.1 2019/12/04 - PoC Implementation
