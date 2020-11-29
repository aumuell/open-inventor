add_library(InventorXt
    src/devices/SoXtFocus.c++
    src/devices/SoXtKeybd.c++
    src/devices/SoXtMouse.c++
    src/devices/SoXtSpball.c++
    src/lightEditor/SoXtDirLitEd.c++
    src/mixedMode/SoFloatCBList.c++
    src/mixedMode/SoXtColEd.c++
    src/mixedMode/SoXtColPatch.c++
    src/mixedMode/SoXtColSlider.c++
    src/mixedMode/SoXtColWheel.c++
    src/mixedMode/SoXtSlider.c++
    src/mixedMode/SoXtUIRegion.c++
    src/motif/SoXtCntlPnl.c++
    src/motif/SoXtFltTxt.c++
    src/motif/SoXtLtSldrSet.c++
    src/motif/SoXtMMSldrMax.c++
    src/motif/SoXtMMSldrMin.c++
    src/motif/SoXtMinMax.c++
    src/motif/SoXtMltSldr.c++
    src/motif/SoXtMltSldrs.c++
    src/motif/SoXtMtlSSet.c++
    src/motif/SoXtSSetBase.c++
    src/motif/SoXtSldrMdl.c++
    src/motif/SoXtSldrMdls.c++
    src/motif/SoXtSldrSet.c++
    src/motif/SoXtSldrTl.c++
    src/motif/SoXtTfSldrSet.c++
    src/mtlEdit/SoXtMtlEdit.c++
    src/mtlEdit/SoXtMtlList.c++
    src/viewers/SoXtBitmapButton.c++
    src/viewers/SoXtConstVwr.c++
    src/viewers/SoXtCursors.c++
    src/viewers/SoXtExamVwr.c++
    src/viewers/SoXtFlyVwr.c++
    src/viewers/SoXtFullVwr.c++
    src/viewers/SoXtIcons.c++
    src/viewers/SoXtPlaneVwr.c++
    src/viewers/SoXtViewer.c++
    src/viewers/SoXtWalkVwr.c++
    src/viewers/Sgm/ThumbWheel.c
    src/SoGLwDAUtil.c
    src/SoGLwDrawA.c
    src/SoGLwMDrawA.c
    src/SoXt.c++
    src/SoXtClipboard.c++
    src/SoXtComponent.c++
    src/SoXtGLWidget.c++
    src/SoXtImportInterest.c++
    src/SoXtPrintDlog.c++
    src/SoXtRendrArea.c++
    src/SoXtRsrc.c++
    src/sgi_visual.c
)

set_target_properties(InventorXt PROPERTIES
    #VERSION ${CMAKE_PROJECT_VERSION}
    VERSION 0.0.0
    SOVERSION 0
)

target_include_directories(InventorXt PRIVATE
    src/viewers
    ../lib/database/include
    ../lib/interaction/include
    ../lib/nodekits/include
)

target_include_directories(InventorXt PUBLIC
    include
    ${MOTIF_INCLUDE_DIR}
)

target_link_libraries(InventorXt PUBLIC
    ${MOTIF_LIBRARIES}
    X11::Xt
    X11::X11
    X11::Xi
    Inventor
)

target_link_libraries(InventorXt PUBLIC OpenGL::GL OpenGL::GLU)

install(TARGETS InventorXt)

install(DIRECTORY include/Inventor/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${IVHDRTOP}
    FILES_MATCHING PATTERN "*.h"
)
