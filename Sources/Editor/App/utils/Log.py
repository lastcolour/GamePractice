def _createLogger():
    import logging
    logger = logging.Logger("main")
    logger.addHandler(logging.StreamHandler())
    logger.setLevel(logging.DEBUG)
    return logger

Log = _createLogger()
del _createLogger