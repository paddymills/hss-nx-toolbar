## NX DXF export using python API

## build

### python (src)
python files require no additional dependencies

```
python -m build.py
```

### rust (filedialog)

filedialog requires rust to build
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

#### Python version
make sure python version matches what NX needs, otherwise NX will complain about loading Python{version}.dll

NX Python versions:
- NX 12: Python 3.6
- NX 1953: Python 3.8

if multiple versions are installed, you can set an env variable to coerce pyo3 into using the right interpreter
```
PYO3_PYTHON={PATH_TO_PYTHON_INTERPRETER}
```
