Name: sgi-OpenInventor-devel
Version: 2.1.5
Release: 6
Distribution: Red Hat
Packager: Silicon Graphics

#
# Change the following as appropriate
#
Summary: Open Inventor Development - a toolkit for writing 3d programs
Copyright: Silicon Graphics, Inc.
Vendor: SGI
URL: http://www.sgi.com
#
# The setting of the Group tag should be picked from the list
# of values pre-defined by Red Hat in the file /usr/doc/rpm-<version>/groups.
# The value is similar to a directory path (e.g. "Networking/Daemons").
#
Group: Development/Libraries
%description
       The Open Inventor 3d Toolkit is an object-oriented toolkit
       that simplifies and abstracts the task of writing graphics
       programming  into  a  set  of  easy to use objects.  These
       objects range from low level data-centered objects such as
       Sphere,  Camera, Material, Light, and Group, to high level
       application-level objects such as Walkthrough  Viewer  and
       Material  Editor.   The  foundation concept in Inventor is
       the "scene database" which defines the objects to be  used
       in an application.  When using Inventor, a programmer cre­
       ates, edits, and composes these objects into  hierarchical
       3d  scene graphs (i.e., database).  A variety of fundamen­
       tal application tasks such as  rendering,  picking,  event
       handling, and file reading/writing are built-in operations
       of all objects in the database  and  thus  are  simple  to
       invoke.


%files
%attr(-, root, root) %dir /usr/include/Inventor
%attr(-, root, root) %dir /usr/lib/Inventor
%attr(-, root, root) %dir /usr/lib/Inventor/Debug
%attr(-, root, root) %dir /usr/man/man1
%attr(-, root, root) %dir /usr/man/man3

/usr/lib/Inventor/Debug/libInventor.so
/usr/lib/Inventor/Debug/libInventorXt.so
/usr/lib/Inventor/Debug/libFL.so
/usr/include/Inventor/*
/usr/man/man1/inventor.1
/usr/man/man3/SbBool.3iv
/usr/man/man3/SbBox2f.3iv
/usr/man/man3/SbBox2s.3iv
/usr/man/man3/SbBox3f.3iv
/usr/man/man3/SbColor.3iv
/usr/man/man3/SbCylinder.3iv
/usr/man/man3/SbCylinderPlaneProjector.3iv
/usr/man/man3/SbCylinderProjector.3iv
/usr/man/man3/SbCylinderSectionProjector.3iv
/usr/man/man3/SbCylinderSheetProjector.3iv
/usr/man/man3/SbLine.3iv
/usr/man/man3/SbLineProjector.3iv
/usr/man/man3/SbMatrix.3iv
/usr/man/man3/SbName.3iv
/usr/man/man3/SbPList.3iv
/usr/man/man3/SbPlane.3iv
/usr/man/man3/SbPlaneProjector.3iv
/usr/man/man3/SbProjector.3iv
/usr/man/man3/SbRotation.3iv
/usr/man/man3/SbSphere.3iv
/usr/man/man3/SbSpherePlaneProjector.3iv
/usr/man/man3/SbSphereProjector.3iv
/usr/man/man3/SbSphereSectionProjector.3iv
/usr/man/man3/SbSphereSheetProjector.3iv
/usr/man/man3/SbString.3iv
/usr/man/man3/SbTime.3iv
/usr/man/man3/SbVec2f.3iv
/usr/man/man3/SbVec2s.3iv
/usr/man/man3/SbVec3f.3iv
/usr/man/man3/SbVec4f.3iv
/usr/man/man3/SbViewVolume.3iv
/usr/man/man3/SbViewportRegion.3iv
/usr/man/man3/SbXfBox3f.3iv
/usr/man/man3/SoAction.3iv
/usr/man/man3/SoAlarmSensor.3iv
/usr/man/man3/SoAnnotation.3iv
/usr/man/man3/SoAntiSquish.3iv
/usr/man/man3/SoAppearanceKit.3iv
/usr/man/man3/SoArray.3iv
/usr/man/man3/SoAsciiText.3iv
/usr/man/man3/SoBase.3iv
/usr/man/man3/SoBaseColor.3iv
/usr/man/man3/SoBaseKit.3iv
/usr/man/man3/SoBaseList.3iv
/usr/man/man3/SoBlinker.3iv
/usr/man/man3/SoBoolOperation.3iv
/usr/man/man3/SoBoxHighlightRenderAction.3iv
/usr/man/man3/SoButtonEvent.3iv
/usr/man/man3/SoByteStream.3iv
/usr/man/man3/SoCalculator.3iv
/usr/man/man3/SoCallback.3iv
/usr/man/man3/SoCallbackAction.3iv
/usr/man/man3/SoCallbackList.3iv
/usr/man/man3/SoCamera.3iv
/usr/man/man3/SoCameraKit.3iv
/usr/man/man3/SoClipPlane.3iv
/usr/man/man3/SoColorIndex.3iv
/usr/man/man3/SoComplexity.3iv
/usr/man/man3/SoComposeMatrix.3iv
/usr/man/man3/SoComposeRotation.3iv
/usr/man/man3/SoComposeRotationFromTo.3iv
/usr/man/man3/SoComposeVec2f.3iv
/usr/man/man3/SoComposeVec3f.3iv
/usr/man/man3/SoComposeVec4f.3iv
/usr/man/man3/SoComputeBoundingBox.3iv
/usr/man/man3/SoConcatenate.3iv
/usr/man/man3/SoCone.3iv
/usr/man/man3/SoConeDetail.3iv
/usr/man/man3/SoCoordinate3.3iv
/usr/man/man3/SoCoordinate4.3iv
/usr/man/man3/SoCounter.3iv
/usr/man/man3/SoCube.3iv
/usr/man/man3/SoCubeDetail.3iv
/usr/man/man3/SoCylinder.3iv
/usr/man/man3/SoCylinderDetail.3iv
/usr/man/man3/SoDB.3iv
/usr/man/man3/SoDataSensor.3iv
/usr/man/man3/SoDebugError.3iv
/usr/man/man3/SoDecomposeMatrix.3iv
/usr/man/man3/SoDecomposeRotation.3iv
/usr/man/man3/SoDecomposeVec2f.3iv
/usr/man/man3/SoDecomposeVec3f.3iv
/usr/man/man3/SoDecomposeVec4f.3iv
/usr/man/man3/SoDelayQueueSensor.3iv
/usr/man/man3/SoDetail.3iv
/usr/man/man3/SoDetailList.3iv
/usr/man/man3/SoDirectionalLight.3iv
/usr/man/man3/SoDrawStyle.3iv
/usr/man/man3/SoElapsedTime.3iv
/usr/man/man3/SoEngine.3iv
/usr/man/man3/SoEngineList.3iv
/usr/man/man3/SoEngineOutput.3iv
/usr/man/man3/SoEngineOutputList.3iv
/usr/man/man3/SoEnvironment.3iv
/usr/man/man3/SoError.3iv
/usr/man/man3/SoEvent.3iv
/usr/man/man3/SoEventCallback.3iv
/usr/man/man3/SoFaceDetail.3iv
/usr/man/man3/SoFaceSet.3iv
/usr/man/man3/SoField.3iv
/usr/man/man3/SoFieldContainer.3iv
/usr/man/man3/SoFieldList.3iv
/usr/man/man3/SoFieldSensor.3iv
/usr/man/man3/SoFile.3iv
/usr/man/man3/SoFont.3iv
/usr/man/man3/SoFontStyle.3iv
/usr/man/man3/SoGLRenderAction.3iv
/usr/man/man3/SoGate.3iv
/usr/man/man3/SoGetBoundingBoxAction.3iv
/usr/man/man3/SoGetMatrixAction.3iv
/usr/man/man3/SoGroup.3iv
/usr/man/man3/SoHandleEventAction.3iv
/usr/man/man3/SoIdleSensor.3iv
/usr/man/man3/SoIndexedFaceSet.3iv
/usr/man/man3/SoIndexedLineSet.3iv
/usr/man/man3/SoIndexedNurbsCurve.3iv
/usr/man/man3/SoIndexedNurbsSurface.3iv
/usr/man/man3/SoIndexedShape.3iv
/usr/man/man3/SoIndexedTriangleStripSet.3iv
/usr/man/man3/SoInfo.3iv
/usr/man/man3/SoInput.3iv
/usr/man/man3/SoInteraction.3iv
/usr/man/man3/SoInteractionKit.3iv
/usr/man/man3/SoInterpolate.3iv
/usr/man/man3/SoInterpolateFloat.3iv
/usr/man/man3/SoInterpolateRotation.3iv
/usr/man/man3/SoInterpolateVec2f.3iv
/usr/man/man3/SoInterpolateVec3f.3iv
/usr/man/man3/SoInterpolateVec4f.3iv
/usr/man/man3/SoKeyboardEvent.3iv
/usr/man/man3/SoLOD.3iv
/usr/man/man3/SoLabel.3iv
/usr/man/man3/SoLevelOfDetail.3iv
/usr/man/man3/SoLight.3iv
/usr/man/man3/SoLightKit.3iv
/usr/man/man3/SoLightModel.3iv
/usr/man/man3/SoLineDetail.3iv
/usr/man/man3/SoLineHighlightRenderAction.3iv
/usr/man/man3/SoLineSet.3iv
/usr/man/man3/SoLinearProfile.3iv
/usr/man/man3/SoLocateHighlight.3iv
/usr/man/man3/SoLocation2Event.3iv
/usr/man/man3/SoMFBitMask.3iv
/usr/man/man3/SoMFBool.3iv
/usr/man/man3/SoMFColor.3iv
/usr/man/man3/SoMFEngine.3iv
/usr/man/man3/SoMFEnum.3iv
/usr/man/man3/SoMFFloat.3iv
/usr/man/man3/SoMFInt32.3iv
/usr/man/man3/SoMFLong.3iv
/usr/man/man3/SoMFMatrix.3iv
/usr/man/man3/SoMFName.3iv
/usr/man/man3/SoMFNode.3iv
/usr/man/man3/SoMFPath.3iv
/usr/man/man3/SoMFPlane.3iv
/usr/man/man3/SoMFRotation.3iv
/usr/man/man3/SoMFShort.3iv
/usr/man/man3/SoMFString.3iv
/usr/man/man3/SoMFTime.3iv
/usr/man/man3/SoMFUInt32.3iv
/usr/man/man3/SoMFULong.3iv
/usr/man/man3/SoMFUShort.3iv
/usr/man/man3/SoMFVec2f.3iv
/usr/man/man3/SoMFVec3f.3iv
/usr/man/man3/SoMFVec4f.3iv
/usr/man/man3/SoMField.3iv
/usr/man/man3/SoMaterial.3iv
/usr/man/man3/SoMaterialBinding.3iv
/usr/man/man3/SoMatrixTransform.3iv
/usr/man/man3/SoMemoryError.3iv
/usr/man/man3/SoMotion3Event.3iv
/usr/man/man3/SoMouseButtonEvent.3iv
/usr/man/man3/SoMultipleCopy.3iv
/usr/man/man3/SoNode.3iv
/usr/man/man3/SoNodeKit.3iv
/usr/man/man3/SoNodeKitDetail.3iv
/usr/man/man3/SoNodeKitListPart.3iv
/usr/man/man3/SoNodeKitPath.3iv
/usr/man/man3/SoNodeList.3iv
/usr/man/man3/SoNodeSensor.3iv
/usr/man/man3/SoNodekitCatalog.3iv
/usr/man/man3/SoNonIndexedShape.3iv
/usr/man/man3/SoNormal.3iv
/usr/man/man3/SoNormalBinding.3iv
/usr/man/man3/SoNurbsCurve.3iv
/usr/man/man3/SoNurbsProfile.3iv
/usr/man/man3/SoNurbsSurface.3iv
/usr/man/man3/SoOffscreenRenderer.3iv
/usr/man/man3/SoOnOff.3iv
/usr/man/man3/SoOneShot.3iv
/usr/man/man3/SoOneShotSensor.3iv
/usr/man/man3/SoOrthographicCamera.3iv
/usr/man/man3/SoOutput.3iv
/usr/man/man3/SoPackedColor.3iv
/usr/man/man3/SoPath.3iv
/usr/man/man3/SoPathList.3iv
/usr/man/man3/SoPathSensor.3iv
/usr/man/man3/SoPathSwitch.3iv
/usr/man/man3/SoPendulum.3iv
/usr/man/man3/SoPerspectiveCamera.3iv
/usr/man/man3/SoPickAction.3iv
/usr/man/man3/SoPickStyle.3iv
/usr/man/man3/SoPickedPoint.3iv
/usr/man/man3/SoPickedPointList.3iv
/usr/man/man3/SoPointDetail.3iv
/usr/man/man3/SoPointLight.3iv
/usr/man/man3/SoPointSet.3iv
/usr/man/man3/SoPrimitiveVertex.3iv
/usr/man/man3/SoProfile.3iv
/usr/man/man3/SoProfileCoordinate2.3iv
/usr/man/man3/SoProfileCoordinate3.3iv
/usr/man/man3/SoQuadMesh.3iv
/usr/man/man3/SoRayPickAction.3iv
/usr/man/man3/SoReadError.3iv
/usr/man/man3/SoResetTransform.3iv
/usr/man/man3/SoRotation.3iv
/usr/man/man3/SoRotationXYZ.3iv
/usr/man/man3/SoRotor.3iv
/usr/man/man3/SoSFBitMask.3iv
/usr/man/man3/SoSFBool.3iv
/usr/man/man3/SoSFColor.3iv
/usr/man/man3/SoSFEngine.3iv
/usr/man/man3/SoSFEnum.3iv
/usr/man/man3/SoSFFloat.3iv
/usr/man/man3/SoSFImage.3iv
/usr/man/man3/SoSFInt32.3iv
/usr/man/man3/SoSFLong.3iv
/usr/man/man3/SoSFMatrix.3iv
/usr/man/man3/SoSFName.3iv
/usr/man/man3/SoSFNode.3iv
/usr/man/man3/SoSFPath.3iv
/usr/man/man3/SoSFPlane.3iv
/usr/man/man3/SoSFRotation.3iv
/usr/man/man3/SoSFShort.3iv
/usr/man/man3/SoSFString.3iv
/usr/man/man3/SoSFTime.3iv
/usr/man/man3/SoSFTrigger.3iv
/usr/man/man3/SoSFUInt32.3iv
/usr/man/man3/SoSFULong.3iv
/usr/man/man3/SoSFUShort.3iv
/usr/man/man3/SoSFVec2f.3iv
/usr/man/man3/SoSFVec3f.3iv
/usr/man/man3/SoSFVec4f.3iv
/usr/man/man3/SoSField.3iv
/usr/man/man3/SoScale.3iv
/usr/man/man3/SoSceneKit.3iv
/usr/man/man3/SoSceneManager.3iv
/usr/man/man3/SoSearchAction.3iv
/usr/man/man3/SoSelectOne.3iv
/usr/man/man3/SoSelection.3iv
/usr/man/man3/SoSensor.3iv
/usr/man/man3/SoSeparator.3iv
/usr/man/man3/SoSeparatorKit.3iv
/usr/man/man3/SoShape.3iv
/usr/man/man3/SoShapeHints.3iv
/usr/man/man3/SoShapeKit.3iv
/usr/man/man3/SoShuttle.3iv
/usr/man/man3/SoSpaceballButtonEvent.3iv
/usr/man/man3/SoSphere.3iv
/usr/man/man3/SoSpotLight.3iv
/usr/man/man3/SoSurroundScale.3iv
/usr/man/man3/SoSwitch.3iv
/usr/man/man3/SoText2.3iv
/usr/man/man3/SoText3.3iv
/usr/man/man3/SoTextDetail.3iv
/usr/man/man3/SoTexture2.3iv
/usr/man/man3/SoTexture2Transform.3iv
/usr/man/man3/SoTextureCoordinate2.3iv
/usr/man/man3/SoTextureCoordinateBinding.3iv
/usr/man/man3/SoTextureCoordinateDefault.3iv
/usr/man/man3/SoTextureCoordinateEnvironment.3iv
/usr/man/man3/SoTextureCoordinateFunction.3iv
/usr/man/man3/SoTextureCoordinatePlane.3iv
/usr/man/man3/SoTimeCounter.3iv
/usr/man/man3/SoTimerQueueSensor.3iv
/usr/man/man3/SoTimerSensor.3iv
/usr/man/man3/SoTranReceiver.3iv
/usr/man/man3/SoTranSender.3iv
/usr/man/man3/SoTransform.3iv
/usr/man/man3/SoTransformSeparator.3iv
/usr/man/man3/SoTransformVec3f.3iv
/usr/man/man3/SoTransformation.3iv
/usr/man/man3/SoTranslation.3iv
/usr/man/man3/SoTriangleStripSet.3iv
/usr/man/man3/SoTriggerAny.3iv
/usr/man/man3/SoType.3iv
/usr/man/man3/SoTypeList.3iv
/usr/man/man3/SoUnits.3iv
/usr/man/man3/SoVertexProperty.3iv
/usr/man/man3/SoVertexShape.3iv
/usr/man/man3/SoWWWAnchor.3iv
/usr/man/man3/SoWWWInline.3iv
/usr/man/man3/SoWrapperKit.3iv
/usr/man/man3/SoWriteAction.3iv
/usr/man/man3/SoXt.3iv
/usr/man/man3/SoXtClipboard.3iv
/usr/man/man3/SoXtComponent.3iv
/usr/man/man3/SoXtConstrainedViewer.3iv
/usr/man/man3/SoXtDevice.3iv
/usr/man/man3/SoXtDirectionalLightEditor.3iv
/usr/man/man3/SoXtExaminerViewer.3iv
/usr/man/man3/SoXtFlyViewer.3iv
/usr/man/man3/SoXtFullViewer.3iv
/usr/man/man3/SoXtGLWidget.3iv
/usr/man/man3/SoXtInputFocus.3iv
/usr/man/man3/SoXtKeyboard.3iv
/usr/man/man3/SoXtLightSliderSet.3iv
/usr/man/man3/SoXtMaterialEditor.3iv
/usr/man/man3/SoXtMaterialList.3iv
/usr/man/man3/SoXtMaterialSliderSet.3iv
/usr/man/man3/SoXtMouse.3iv
/usr/man/man3/SoXtPlaneViewer.3iv
/usr/man/man3/SoXtPrintDialog.3iv
/usr/man/man3/SoXtRenderArea.3iv
/usr/man/man3/SoXtResource.3iv
/usr/man/man3/SoXtSliderSet.3iv
/usr/man/man3/SoXtSliderSetBase.3iv
/usr/man/man3/SoXtSpaceball.3iv
/usr/man/man3/SoXtTransformSliderSet.3iv
/usr/man/man3/SoXtViewer.3iv
/usr/man/man3/SoXtWalkViewer.3iv



