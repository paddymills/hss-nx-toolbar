
import os
import tomllib

import dialog

def find_config_path():
    """
    Find the configuration file path.

    First will look for the config file specified by the environment variable HSS_DXF_CONFIG_PATH.

    If not set, it will look for 'dxf-export.toml' in several standard locations.
    1) UGII_CUSTOM_UFUNC_DIR/application
    2) UGII_CUSTOM_UFUNC_DIR
    3) UGII_CUSTOM_DIR
    4) DWGDXF_DIR
    """

    if os.environ.get('HSS_DXF_CONFIG_PATH'):
        return os.environ.get('HSS_DXF_CONFIG_PATH')

    def env_path(var, path=None):
        env_var = os.environ.get(var)
        return env_var and os.path.join(env_var, path, 'dxf-export.toml')

    paths = [
        env_path('UGII_CUSTOM_UFUNC_DIR', 'application'),
        env_path('UGII_CUSTOM_UFUNC_DIR'),
        env_path('UGII_CUSTOM_DIR'),
        env_path('DWGDXF_DIR'),
    ]
    for p in paths:
        if p and os.path.exists(p):
            return p
    
    dialog.error("Configuration file 'dxf-export.toml' not found in expected locations.")
    raise FileNotFoundError("Configuration file 'dxf-export.toml' not found.")

def load_config():
    """Load configuration from config.toml file."""
    with open(find_config_path()) as f:
        config = tomllib.load(f)
    return config
