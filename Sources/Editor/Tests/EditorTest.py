import sys
import pathlib

PATH = pathlib.Path("{0}/../../{1}".format(__file__, "App")).resolve()
sys.path.append(PATH.__str__())