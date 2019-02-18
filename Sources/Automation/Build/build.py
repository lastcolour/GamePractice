import argparse
import time
import sys
import os

from lib.config import ConfigNode
from lib.project import Project

_BUILD_CONFIG_FILE = os.path.realpath(__file__ + "../../configs/build.json")
_LOCAL_CONFIG_FILE = os.path.realpath(__file__ + "../../../../../config.local")

def _getPlatformName():
    if sys.platform.startswith("linux"):
        return "Linux"
    elif sys.platform.startswith("win"):
        return "Windows"
    else:
        raise RuntimeError("Can't get platform type")

def parseArgs():
    parser = argparse.ArgumentParser(description='Build project')
    parser.add_argument('-build', choices=['Debug', "Release"], help='build type',
        dest='buildType', metavar='BuildType', required=True)
    parser.add_argument('-clean', help='Clean artifacts',
        dest='cleanBuild', required=False, action="store_const", const=False)
    parser.add_argument('-tests', help='Run test',
        dest='runTests', required=False, action="store_const", const=True)
    parser.add_argument('-platform', choices=['Linux', 'Windows', 'Android', 'All'], help='target platform',
        dest='platform', metavar='TargetPlatform', required=False, default=_getPlatformName())
    parser.add_argument('-filter', metavar="FilterTag", help="Skip targets by tags", required=False,
        dest="filterTag", type=str)
    return parser.parse_args()

def main():
    buildArgs = parseArgs()
    ConfigNode.setArgVars(buildArgs.buildType, buildArgs.platform)
    configs = ConfigNode()
    configs.loadLocalConfigs(_LOCAL_CONFIG_FILE)
    configs.load(_BUILD_CONFIG_FILE)

    proj = Project(configs)
    proj.filter(buildArgs.filterTag)
    if buildArgs.cleanBuild:
        proj.clean()
    proj.build()
    if buildArgs.runTests:
        proj.tests()

if __name__ == "__main__":
    main()