IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

INST_HEADER_ROOT   = $(IVROOT)/usr/include/Inventor/sensors
HEADER_PATH        = $(INST_HEADER_ROOT)
NOSHIP_HEADER_PATH = $(HEADER_PATH)

INST_HEADERS = \
	SoAlarmSensor.h \
	SoDataSensor.h \
	SoDelayQueueSensor.h \
	SoFieldSensor.h \
	SoIdleSensor.h \
	SoNodeSensor.h \
	SoOneShotSensor.h \
	SoPathSensor.h \
	SoSensor.h \
	SoSensorManager.h \
	SoSensors.h \
	SoTimerQueueSensor.h \
	SoTimerSensor.h

LDIRT = $(INST_HEADER_ROOT)

install: headers

include $(IVCOMMONRULES)

