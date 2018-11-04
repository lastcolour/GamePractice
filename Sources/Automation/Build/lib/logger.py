__all__ = ["log"]

def _createLogger():
    import logging
    import sys
    logger = logging.getLogger("__main__")
    logger.setLevel(logging.DEBUG)
    logger.addHandler(logging.StreamHandler(sys.stderr))
    return logger

log = _createLogger()
del _createLogger