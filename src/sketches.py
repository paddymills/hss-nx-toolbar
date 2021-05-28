
import logging
import config
import NXOpen


logger = logging.getLogger(__name__)


def get_sketches_to_export(part):
        
    for sk in part.Sketches:
        
        for search_term, layer in config.WHITELISTED_SKETCHES.items():
            if search_term.search(sk.Name):

                # set layer
                if not part.IsReadOnly:
                    for geo in sk.GetAllGeometry():
                        geo.Layer = layer.value

                yield sk

                break

        else:
            logger.debug("Skipping sketch: {}".format( sk.Name ))