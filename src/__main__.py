
import sys
import entrypoints

entrypoint_map = {
    "select":       entrypoints.select,
    "active":       entrypoints.active,
    "all_open":     entrypoints.all_open,
    "mfg":          entrypoints.mfg,
    "test":         entrypoints.test,
}


# parse caller option
try:
    opt = sys.argv[1]

# default -> select
except IndexError:
    opt = "select"

entrypoint_map[opt]()