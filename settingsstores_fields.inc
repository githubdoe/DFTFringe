// Shared settings schema list.
// Entry format for callbacks:
// FIELD(type, memberName, defaultValue, key, converter)

#define SETTINGS_STORE_FOR_EACH_MIRROR_FIELD(FIELD) \
	FIELD(QString, mirrorName, QStringLiteral("default"), "config mirror name", toString) \
	FIELD(bool, doNull, true, "config doNull", toBool) \
	FIELD(double, diameter, 200.0, "config diameter", toDouble) \
	FIELD(double, roc, 2000.0, "config roc", toDouble) \
	FIELD(double, obstruction, 0.0, "config obstruction", toDouble) \
	FIELD(double, cc, -1.0, "config cc", toDouble) \
	FIELD(double, lambda, 640.0, "config lambda", toDouble) \
	FIELD(double, fringeSpacing, 1.0, "config fringe spacing", toDouble) \
	FIELD(bool, flipH, false, "flipH", toBool) \
	FIELD(bool, useAnnulus, false, "md use annulus", toBool) \
	FIELD(double, annulusPercent, 0.0, "md annulus percent", toDouble) \
	FIELD(bool, annulusToObstruction, false, "md Annulus to obs", toBool) \
	FIELD(int, outlineShape, 0, "outlineShape", toInt) \
	FIELD(double, ellipseMinorAxis, 50.0, "ellipseMinorAxis", toDouble) \
	FIELD(bool, apertureReductionEnabled, false, "configAperatureReductionChecked", toBool) \
	FIELD(double, apertureReduction, 0.0, "config aperatureReduction", toDouble)

#define SETTINGS_STORE_FOR_EACH_APPLICATION_FIELD(FIELD) \
	FIELD(QString, projectPath, QString(), "projectPath", toString) \
	FIELD(QString, mirrorConfigFile, QString(), "mirrorConfigFile", toString) \
	FIELD(QString, lastPath, QString(), "lastPath", toString)

// examples for futur PRs
/*#define SETTINGS_STORE_FOR_EACH_GENERAL_FIELD(FIELD) \
	FIELD(bool, useMakeStarTest, false, "useMakeStarTest", toBool) \
	FIELD(int, wavefrontDownSizeValue, 650, "wavefrontDownSizeValue", toInt) \
	FIELD(bool, wavefrontShouldDownsize, false, "wavefrontShouldDownsize", toBool) \
	FIELD(double, outputLambda, 550.0, "outputLambda", toDouble) \
	FIELD(bool, applyOffsets, false, "applyOffsets", toBool) \
	FIELD(int, astigDistGraphWidth, 0, "AstigDistGraphWidth", toInt) \
	FIELD(bool, gaussianBlurEnabled, true, "GBlur", toBool) \
	FIELD(int, gaussianBlurValue, 20, "GBValue", toInt) \
	FIELD(bool, gaussianRadiusConverted, false, "gaussianRadiusConverted", toBool) \
	FIELD(int, lowMemoryThreshold, 0, "lowMemoryThreshold", toInt) \
	FIELD(bool, deletePrevWave, false, "deletePrevWave", toBool)

#define SETTINGS_STORE_FOR_EACH_CONTOUR_FIELD(FIELD) \
	FIELD(double, contourRange, 0.100, "contourRange", toDouble) \
	FIELD(bool, contourShowFill, true, "contourShowFill", toBool) \
	FIELD(bool, contourShowLines, true, "contourShowLines", toBool) \
	FIELD(bool, contourShowRuler, false, "contourShowRuler", toBool) \
	FIELD(int, colorMapType, 0, "colorMapType", toInt) \
	FIELD(int, colorMapIndex, 1, "colorMap ndx", toInt) \
	FIELD(QString, contourLineColor, QStringLiteral("grey"), "ContourLineColor", toString) \
	FIELD(QString, contourRulerColor, QStringLiteral("grey"), "ContourRulerColor", toString) \
	FIELD(double, contourRulerRadialDeg, 0.0, "contourRulerRadialDeg", toDouble)*/
