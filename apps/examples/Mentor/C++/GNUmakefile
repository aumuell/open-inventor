IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS += -I../../../../lib/database/include/ \
	    -I../../../../lib/interaction/include \
	    -I../../../../lib/nodekits/include \
	    -O3 \
	    -I../../../../libSoXt/include 

TARGETS = 02.1.HelloCone 02.2.EngineSpin \
	02.3.Trackball 02.4.Examiner \
	03.1.Molecule 03.2.Robot 03.3.Naming \
	04.1.Cameras 04.2.Lights \
	05.1.FaceSet 05.2.IndexedFaceSet \
	05.3.TriangleStripSet 05.4.QuadMesh \
	05.5.Binding 05.6.TransformOrdering \
	06.1.Text 06.2.Simple3DText 06.3.Complex3DText \
	07.1.BasicTexture 07.2.TextureCoordinates \
	07.3.TextureFunction \
	08.1.BSCurve 08.2.UniCurve 08.3.BezSurf \
	08.4.TrimSurf \
	09.1.Print  09.2.Texture 09.3.Search \
	09.4.PickAction 09.5.GenSph \
	10.1.addEventCB 10.2.setEventCB \
	10.3and4.MotifList 10.5.SelectionCB \
        10.6.PickFilterTopLevel 10.7.PickFilterManip \
	10.8.PickFilterNodeKit \
	11.1.ReadFile 11.2.ReadString \
	12.1.FieldSensor 12.2.NodeSensor \
	12.3.AlarmSensor 12.4.TimerSensor \
	13.1.GlobalFlds \
	13.2.ElapsedTime 13.3.TimeCounter \
	13.4.Gate 13.5.Boolean 13.6.Calculator \
	13.7.Rotor 13.8.Blinker \
	14.1.FrolickingWords 14.2.Editors 14.3.Balance \
	15.1.ConeRadius 15.2.SliderBox 15.3.AttachManip \
	15.4.Customize \
	16.1.Overlay 16.2.Callback 16.3.AttachEditor \
	16.4.OneWindow 16.5.Examiner \
	17.1.ColorIndex 17.2.GLCallback 17.3.GLFloor

all:: $(TARGETS)

LLDOPTS = \
	-L../../../../libimage -L../../../../lib/ \
	-L/usr/X11R6/lib -L../../../../libSoXt \
	-L../../../../libFL/src \
	-L/usr/X11R6/lib \

LLDLIBS = \
	-lInventorXt -lInventor \
	-limage \
	-lXm -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm

02.1.HelloCone: 02.1.HelloCone.o 
	$(CXX) $(LDOPTS) 02.1.HelloCone.o $(LDLIBS) -o $@

02.2.EngineSpin: 02.2.EngineSpin.o 
	$(CXX) $(LDOPTS) 02.2.EngineSpin.o $(LDLIBS) -o $@

02.3.Trackball: 02.3.Trackball.o 
	$(CXX) $(LDOPTS) 02.3.Trackball.o $(LDLIBS) -o $@

02.4.Examiner: 02.4.Examiner.o 
	$(CXX) $(LDOPTS) 02.4.Examiner.o $(LDLIBS) -o $@

03.1.Molecule: 03.1.Molecule.o 
	$(CXX) $(LDOPTS) 03.1.Molecule.o $(LDLIBS) -o $@ 

03.2.Robot: 03.2.Robot.o 
	$(CXX) $(LDOPTS) 03.2.Robot.o $(LDLIBS) -o $@ 

03.3.Naming: 03.3.Naming.o 
	$(CXX) $(LDOPTS) 03.3.Naming.o $(LDLIBS) -o $@
		
04.1.Cameras: 04.1.Cameras.o 
	$(CXX) $(LDOPTS) 04.1.Cameras.o $(LDLIBS) -o $@ 

04.2.Lights: 04.2.Lights.o 
	$(CXX) $(LDOPTS) 04.2.Lights.o $(LDLIBS) -o $@
	
05.1.FaceSet: 05.1.FaceSet.o 
	$(CXX) $(LDOPTS) 05.1.FaceSet.o $(LDLIBS) -o $@ 

05.2.IndexedFaceSet: 05.2.IndexedFaceSet.o 
	$(CXX) $(LDOPTS) 05.2.IndexedFaceSet.o $(LDLIBS) -o $@
	
05.3.TriangleStripSet: 05.3.TriangleStripSet.o 
	$(CXX) $(LDOPTS) 05.3.TriangleStripSet.o $(LDLIBS) -o $@ 

05.4.QuadMesh: 05.4.QuadMesh.o 
	$(CXX) $(LDOPTS) 05.4.QuadMesh.o $(LDLIBS) -o $@
	
05.5.Binding: 05.5.Binding.o 
	$(CXX) $(LDOPTS) 05.5.Binding.o $(LDLIBS) -o $@ 

05.6.TransformOrdering: 05.6.TransformOrdering.o 
	$(CXX) $(LDOPTS) 05.6.TransformOrdering.o $(LDLIBS) -o $@
	
06.1.Text: 06.1.Text.o 
	$(CXX) $(LDOPTS) 06.1.Text.o $(LDLIBS) -o $@ 

06.2.Simple3DText: 06.2.Simple3DText.o 
	$(CXX) $(LDOPTS) 06.2.Simple3DText.o $(LDLIBS) -o $@ 

06.3.Complex3DText: 06.3.Complex3DText.o 
	$(CXX) $(LDOPTS) 06.3.Complex3DText.o $(LDLIBS) -o $@ 
	
07.1.BasicTexture: 07.1.BasicTexture.o 
	$(CXX) $(LDOPTS) 07.1.BasicTexture.o $(LDLIBS) -o $@ 

07.2.TextureCoordinates: 07.2.TextureCoordinates.o 
	$(CXX) $(LDOPTS) 07.2.TextureCoordinates.o $(LDLIBS) -o $@
	
07.3.TextureFunction: 07.3.TextureFunction.o 
	$(CXX) $(LDOPTS) 07.3.TextureFunction.o $(LDLIBS) -o $@
	
08.1.BSCurve: 08.1.BSCurve.o 
	$(CXX) $(LDOPTS) 08.1.BSCurve.o $(LDLIBS) -o $@ 

08.2.UniCurve: 08.2.UniCurve.o 
	$(CXX) $(LDOPTS) 08.2.UniCurve.o $(LDLIBS) -o $@ 

08.3.BezSurf: 08.3.BezSurf.o 
	$(CXX) $(LDOPTS) 08.3.BezSurf.o $(LDLIBS) -o $@ 
	
08.4.TrimSurf: 08.4.TrimSurf.o 
	$(CXX) $(LDOPTS) 08.4.TrimSurf.o $(LDLIBS) -o $@ 
	
09.1.Print: 09.1.Print.o 
	$(CXX) $(LDOPTS) 09.1.Print.o $(LDLIBS) -o $@  

09.2.Texture: 09.2.Texture.o 
	$(CXX) $(LDOPTS) 09.2.Texture.o $(LDLIBS) -o $@ 

09.3.Search: 09.3.Search.o 
	$(CXX) $(LDOPTS) 09.3.Search.o $(LDLIBS) -o $@
	
09.4.PickAction: 09.4.PickAction.o 
	$(CXX) $(LDOPTS) 09.4.PickAction.o $(LDLIBS) -o $@ 

09.5.GenSph: 09.5.GenSph.o 
	$(CXX) $(LDOPTS) 09.5.GenSph.o $(LDLIBS) -o $@
	
10.1.addEventCB: 10.1.addEventCB.o 
	$(CXX) $(LDOPTS) 10.1.addEventCB.o $(LDLIBS) -o $@ 

10.2.setEventCB: 10.2.setEventCB.o 
	$(CXX) $(LDOPTS) 10.2.setEventCB.o $(LDLIBS) -o $@
	
10.3and4.MotifList: 10.3and4.MotifList.o 
	$(CXX) $(LDOPTS) 10.3and4.MotifList.o $(LDLIBS) -o $@ 

10.5.SelectionCB: 10.5.SelectionCB.o 
	$(CXX) $(LDOPTS) 10.5.SelectionCB.o $(LDLIBS) -o $@
        
10.6.PickFilterTopLevel: 10.6.PickFilterTopLevel.o 
	$(CXX) $(LDOPTS) 10.6.PickFilterTopLevel.o $(LDLIBS) -o $@ 

10.7.PickFilterManip: 10.7.PickFilterManip.o 
	$(CXX) $(LDOPTS) 10.7.PickFilterManip.o $(LDLIBS) -o $@
	
10.8.PickFilterNodeKit: 10.8.PickFilterNodeKit.o 
	$(CXX) $(LDOPTS) 10.8.PickFilterNodeKit.o $(LDLIBS) -o $@
	
11.1.ReadFile: 11.1.ReadFile.o 
	$(CXX) $(LDOPTS) 11.1.ReadFile.o $(LDLIBS) -o $@ 

11.2.ReadString: 11.2.ReadString.o 
	$(CXX) $(LDOPTS) 11.2.ReadString.o $(LDLIBS) -o $@
	
12.1.FieldSensor: 12.1.FieldSensor.o 
	$(CXX) $(LDOPTS) 12.1.FieldSensor.o $(LDLIBS) -o $@ 

12.2.NodeSensor: 12.2.NodeSensor.o 
	$(CXX) $(LDOPTS) 12.2.NodeSensor.o $(LDLIBS) -o $@
	
12.3.AlarmSensor: 12.3.AlarmSensor.o 
	$(CXX) $(LDOPTS) 12.3.AlarmSensor.o $(LDLIBS) -o $@ 

12.4.TimerSensor: 12.4.TimerSensor.o 
	$(CXX) $(LDOPTS) 12.4.TimerSensor.o $(LDLIBS) -o $@
	
13.1.GlobalFlds: 13.1.GlobalFlds.o 
	$(CXX) $(LDOPTS) 13.1.GlobalFlds.o $(LDLIBS) -o $@
	
13.2.ElapsedTime: 13.2.ElapsedTime.o 
	$(CXX) $(LDOPTS) 13.2.ElapsedTime.o $(LDLIBS) -o $@ 

13.3.TimeCounter: 13.3.TimeCounter.o 
	$(CXX) $(LDOPTS) 13.3.TimeCounter.o $(LDLIBS) -o $@
	
13.4.Gate: 13.4.Gate.o 
	$(CXX) $(LDOPTS) 13.4.Gate.o $(LDLIBS) -o $@ 

13.5.Boolean: 13.5.Boolean.o 
	$(CXX) $(LDOPTS) 13.5.Boolean.o $(LDLIBS) -o $@ 

13.6.Calculator: 13.6.Calculator.o 
	$(CXX) $(LDOPTS) 13.6.Calculator.o $(LDLIBS) -o $@
	
13.7.Rotor: 13.7.Rotor.o 
	$(CXX) $(LDOPTS) 13.7.Rotor.o $(LDLIBS) -o $@ 

13.8.Blinker: 13.8.Blinker.o 
	$(CXX) $(LDOPTS) 13.8.Blinker.o $(LDLIBS) -o $@
	
14.1.FrolickingWords: 14.1.FrolickingWords.o 
	$(CXX) $(LDOPTS) 14.1.FrolickingWords.o $(LDLIBS) -o $@ 

14.2.Editors: 14.2.Editors.o 
	$(CXX) $(LDOPTS) 14.2.Editors.o $(LDLIBS) -o $@ 

14.3.Balance: 14.3.Balance.o 
	$(CXX) $(LDOPTS) 14.3.Balance.o $(LDLIBS) -o $@
	
15.1.ConeRadius: 15.1.ConeRadius.o 
	$(CXX) $(LDOPTS) 15.1.ConeRadius.o $(LDLIBS) -o $@ 

15.2.SliderBox: 15.2.SliderBox.o 
	$(CXX) $(LDOPTS) 15.2.SliderBox.o $(LDLIBS) -o $@ 

15.3.AttachManip: 15.3.AttachManip.o 
	$(CXX) $(LDOPTS) 15.3.AttachManip.o $(LDLIBS) -o $@
	
15.4.Customize: 15.4.Customize.o 
	$(CXX) $(LDOPTS) 15.4.Customize.o $(LDLIBS) -o $@
	
16.1.Overlay: 16.1.Overlay.o 
	$(CXX) $(LDOPTS) 16.1.Overlay.o $(LDLIBS) -o $@ 

16.2.Callback: 16.2.Callback.o 
	$(CXX) $(LDOPTS) 16.2.Callback.o $(LDLIBS) -o $@ 

16.3.AttachEditor: 16.3.AttachEditor.o 
	$(CXX) $(LDOPTS) 16.3.AttachEditor.o $(LDLIBS) -o $@ 
	
16.4.OneWindow: 16.4.OneWindow.o 
	$(CXX) $(LDOPTS) 16.4.OneWindow.o $(LDLIBS) -o $@ 

16.5.Examiner: 16.5.Examiner.o 
	$(CXX) $(LDOPTS) 16.5.Examiner.o $(LDLIBS) -o $@
	
17.1.ColorIndex: 17.1.ColorIndex.o 
	$(CXX) $(LDOPTS) 17.1.ColorIndex.o $(LDLIBS) -o $@ 

17.2.GLCallback: 17.2.GLCallback.o 
	$(CXX) $(LDOPTS) 17.2.GLCallback.o $(LDLIBS) -o $@ 

17.3.GLFloor: 17.3.GLFloor.o 
	$(CXX) $(LDOPTS) 17.3.GLFloor.o $(LDLIBS) -o $@

include $(COMMONRULES)
