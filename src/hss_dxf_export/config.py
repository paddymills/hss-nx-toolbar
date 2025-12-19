
import os
import tomllib
from types import SimpleNamespace

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


class ConfigNamespace(SimpleNamespace):
    def __init__(self, config: dict):
        for key, value in config.items():
            if isinstance(value, dict):
                value = ConfigNamespace(value)
            setattr(self, key, value)

    @staticmethod
    def load_from_file():
        """Load configuration from config.toml file."""
        with open(find_config_path(), 'rb') as f:
            config_dict = tomllib.load(f)
        return ConfigNamespace(config_dict)

class DxfConfig(object):
    """Singleton class to hold DXF export configuration."""
    # see https://www.geeksforgeeks.org/python/singleton-pattern-in-python-a-complete-guide/

    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(DxfConfig, cls).__new__(cls)
            cls._instance.config = None
        return cls._instance
    
    @property
    def config(self):
        if self._instance.config is None:
            self._instance.config = ConfigNamespace.load_from_file()
        return self._instance.config

    def __getattr__(self, name):
        return getattr(self.config, name)

# create a single global config instance
config = DxfConfig()
