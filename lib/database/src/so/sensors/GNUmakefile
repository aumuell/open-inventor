IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS += -I$(DEPTH)/include -DLIBRARYBUILD -DDEBUG

CXXFILES =\
	SoAlarmSensor.c++ \
	SoDataSensor.c++ \
	SoDelayQueueSensor.c++ \
	SoFieldSensor.c++ \
	SoIdleSensor.c++ \
	SoNodeSensor.c++ \
	SoOneShotSensor.c++ \
	SoPathSensor.c++ \
	SoSensor.c++ \
	SoSensorMgr.c++ \
	SoTimerQueueSensor.c++ \
	SoTimerSensor.c++

all:: $(OBJECTS)

include $(COMMONRULES)
