## NX DXF export using python API

### build

python files require no additional dependencies

```
python -m build.py
```

# filedialog

filedialog requires rust to build. (NX 12.0 uses python3.6, NX1953 uses python3.8)
```
cd filedialog-rs
cargo build --release
```

copy into application directory
```
cp target/release/filedialog.dll ../menus/application/filedialog.pyd
   --- OR ---
python -m build.py rs
```

make sure python version matches what NX needs:
- NX 12: Python 3.6
- NX 1953: Python 3.8

if multiple versions are install, you can set an env variable to coerce pyo3 into using the right interpreter
```
PYO3_PYTHON={PATH_TO_PYTHON_INTERPRETER}
```
