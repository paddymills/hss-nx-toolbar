# NX DXF export using python API

## build

Python files require no additional dependencies
Rust modules require rust (cargo binary) to compile.

```
python -m build.py
```

Rust module compilation can be skipped with the `--skip-rs-build` flag. This should only be done if the rust compilation is taking long. If the rust code has not been compiled, the build script will crash when it tries to copy the binary.
```
python -m build.py --skip-rs-build
```

### Python version
Make sure python version matches what NX needs, otherwise NX will complain about loading Python{version}.dll

NX Python versions:
- NX 12: Python 3.6
- NX 1953: Python 3.8

if multiple versions are installed, you can set an environment variable to coerce pyo3 into using the right interpreter
```
PYO3_PYTHON={PATH_TO_PYTHON_INTERPRETER}
```
## deploy

To deploy to local, use the `--local` flag
```
python -m build.py --local
```

To deploy to an engineering server installation directory use the `--dest` flag with the base installation folder

```
python -m build.py --dest {base install directory}
```
