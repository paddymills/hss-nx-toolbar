
import os
import tomllib

def load_config():
    """Load configuration from config.toml file."""
    config_path = os.path.join(os.environ['NX_SITE_DIR'], 'dxf-export.toml')
    with open(config_path) as f:
        config = tomllib.load(f)
    return config
