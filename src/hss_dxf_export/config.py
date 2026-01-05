import os
import re
from types import SimpleNamespace

import dialog
import NXOpen
import tomllib


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

    session = NXOpen.Session.GetSession()

    if session.GetEnvironmentVariableValue("HSS_DXF_EXPORT_CONFIG"):
        return session.GetEnvironmentVariableValue("HSS_DXF_EXPORT_CONFIG")

    def env_path(var, *path):
        env_var = session.GetEnvironmentVariableValue(var)
        return env_var and os.path.join(env_var, *path, "dxf-export.toml")

    paths = [
        env_path("UGII_CUSTOM_UFUNC_DIR", "application"),
        env_path("UGII_CUSTOM_UFUNC_DIR"),
        env_path("UGII_CUSTOM_DIR"),
        env_path("DWGDXF_DIR"),
    ]
    for p in paths:
        if p and os.path.exists(p):
            return p

    dialog.error(
        "Configuration file 'dxf-export.toml' not found in expected locations."
    )
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
        with open(find_config_path(), "rb") as f:
            config_dict = tomllib.load(f)
        return ConfigNamespace(config_dict)


class DxfConfig(object):
    """Singleton class to hold DXF export configuration."""

    # see https://www.geeksforgeeks.org/python/singleton-pattern-in-python-a-complete-guide/

    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(DxfConfig, cls).__new__(cls)
            cls._instance._config = None
        return cls._instance

    def __init__(self):
        self._config = None

    @property
    def config(self):
        if self._config is None:
            self._config = ConfigNamespace.load_from_file()

            self.sketch_mapping_regex = []
            cfgs = [
                config.layers.marking,
                config.layers.no_cut,
                config.layers.detail,
            ]
            for layer in cfgs:
                for pattern in layer.sketches:
                    self.sketch_mapping_regex.append((re.compile(pattern), layer.layer))

        return self._config

    def map_sketch_to_layer(self, sketch_name: str) -> int | None:
        """Return the layer name for the given sketch name, or None if no mapping found."""
        for pattern, layer in self.sketch_mapping_regex:
            if pattern.fullmatch(sketch_name):
                return layer
        return None

    def is_heatnum_sketch(self, sketch_name: str) -> bool:
        for sk in self.config.layers.heatnum.sketches:
            if re.match(sk, sketch_name):
                return True
        return False

    def __getattr__(self, name):
        return getattr(self.config, name)


# create a single global config instance
config = DxfConfig()
