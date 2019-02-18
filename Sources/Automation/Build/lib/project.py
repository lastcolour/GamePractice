from .logger import log
from .runtests import TestRunner
from .buildstep import BuildStep

class Project:
    def __init__(self, configs):
        self._buildSteps = []
        self._unitTests = []
        self._init(configs)

    def _extendBuildStep(self, buildConfig):
        tStepExtendNode = "step-extension"
        if tStepExtendNode in buildConfig:
            buildConfig.extendFromFile(buildConfig.value(tStepExtendNode))

    def _generateSubBuilds(self, buildStep):
        if buildStep.isAndroid():
            for item in buildStep.get("android-abi"):
                tAndroidAbi = item.value()
                log.info("[Info] Generate sub-build step for: {0}".format(tAndroidAbi))
                tSubStep = buildStep.clone()
                tSubStep.set("android-abi", tAndroidAbi)
                tSubStep.set("platform-suffix", "/{0}".format(tAndroidAbi))
                yield tSubStep
        else:
            yield buildStep

    def _initTestSteps(self, config):
        if config.isAndroid():
            return
        tBuildConfig = config.get("build")
        if "test-steps" not in tBuildConfig:
            log.warning("[Warning] No test specified")
        testConf = tBuildConfig.get("test-steps")
        if "unittests" in testConf:
            self._unitTests.append(TestRunner(testConf.get("unittests")))

    def _initBuildSteps(self, configs):
        tBuildConfig = configs.get("build")
        tBuildConfig.createFmtVars("build-vars")
        for tBuildStep in tBuildConfig.get("build-steps"):
            if not self._isSupportPlatform(tBuildStep):
                log.info("[Info] Skipping due to not supported platform build: {0}"
                         .format(tBuildStep.value("step-name")))
                continue
            log.info("[Info] Process build step: {0}".format(tBuildStep.value("step-name")))
            self._extendBuildStep(tBuildStep)
            for tSubStep in self._generateSubBuilds(tBuildStep):
                self._buildSteps.append(BuildStep(tSubStep))

    def _isSupportPlatform(self, config):
        return config.platform() in config.get("supported-platforms")

    def _init(self, configs):
        self._initBuildSteps(configs)
        self._initTestSteps(configs)

    def filter(self, filterTag):
        if filterTag is "":
            log.info("[Info] Skip filter spet while filter tag is empty")
            return
        log.debug("[Debug] Appling filter tag: '{0}'".format(filterTag))
        filteredBuildSteps = []
        for buildStep in self._buildSteps:
            if buildStep.hasTag(filterTag):
                log.info("[Ifno] Skip build target while it has filter tag: {0}".format(buildStep.getName()))
            else:
                filteredBuildSteps.append(buildStep)
        self._buildSteps = filteredBuildSteps

    def build(self):
        for step in self._buildSteps:
            step.run()

    def clean(self):
        for step in self._buildSteps:
            tOutDirs = step.getOutDirs()

    def tests(self):
        for item in self._unitTests:
            item.run()
