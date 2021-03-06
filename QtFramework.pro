QT += core gui widgets opengl

win32 {
    message("Windows platform...")

    INCLUDEPATH += $$PWD/Dependencies/Include
    DEPENDPATH += $$PWD/Dependencies/Include

    LIBS += -lopengl32 -lglu32

    CONFIG(release, debug|release): {
        contains(QT_ARCH, i386) {
            message("x86 (i.e., 32-bit) release build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86/" -lglew32
        } else {
            message("x86_64 (i.e., 64-bit) release build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86_64/" -lglew32
        }
    } else: CONFIG(debug, debug|release): {
        contains(QT_ARCH, i386) {
            message("x86 (i.e., 32-bit) debug build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86/" -lglew32
        } else {
            message("x86_64 (i.e., 64-bit) debug build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86_64" -lglew32
        }
    }

    msvc {
      QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
      QMAKE_CXXFLAGS_RELEASE *= -O2
    }
}

unix: !mac {
    message("Unix/Linux platform...")

    # for GLEW installed into /usr/lib/libGLEW.so or /usr/lib/glew.lib
    LIBS += -lGLEW -lGLU
}

mac {
    message("Macintosh platform...")

    # IMPORTANT: change the letters x, y, z in the next two lines
    # to the corresponding version numbers of the GLEW library
    # which was installed by using the command 'brew install glew'
    INCLUDEPATH += "/usr/local/Cellar/glew/x.y.z/include/"
    LIBS += -L"/usr/local/Cellar/glew/x.y.z/lib/" -lGLEW

    # the OpenGL library has to added as a framework
    LIBS += -framework OpenGL
}


FORMS += \
    GUI/MainWindow.ui \
    GUI/SideWidget.ui

HEADERS += \
    GUI/GLWidget.h \
    GUI/MainWindow.h \
    GUI/SideWidget.h \
    Core/Exceptions.h \
    Core/Constants.h \
    Core/DCoordinates3.h \
    Core/GenericCurves3.h \
    Core/Matrices.h \
    Core/RealSquareMatrices.h \
    Parametric/ParametricCurves3.h \
    SOQAH/BlendingFunctionUtil.h \
    SOQAH/SOQAHPatch3.h \
    Test/TestFunctions.h \
    Parametric/ParametricSurfaces3.h \
    Core/Colors4.h \
    Core/HCoordinates3.h \
    Core/Lights.h \
    Core/Materials.h \
    Core/TCoordinates4.h \
    Core/TriangularFaces.h \
    Core/TriangulatedMeshes3.h \
    Cyclic/CyclicCurves3.h \
    Core/LinearCombination3.h \
    Core/TensorProductSurfaces3.h \
    Core/ShaderPrograms.h \
    SOQAH/SOQAHArcs3.h \
    SOQAH/SOQAHCompositeCurve3.h \
    SOQAH/SOQAHCompositeSurface3.h

SOURCES += \
    GUI/GLWidget.cpp \
    GUI/MainWindow.cpp \
    GUI/SideWidget.cpp \
    Core/GenericCurves3.cpp \
    Core/RealSquareMatrices.cpp \
    Parametric/ParametricCurves3.cpp \
    SOQAH/BlendingFunctionUtil.cpp \
    SOQAH/SOQAHPatch3.cpp \
    Test/TestFunctions.cpp \
    main.cpp \
    Parametric/ParametricSurfaces3.cpp \
    Core/Lights.cpp \
    Core/Materials.cpp \
    Core/TriangulatedMeshes3.cpp \
    Cyclic/CyclicCurves3.cpp \
    Core/LinearCombination3.cpp \
    Core/TensorProductSurfaces3.cpp \
    Core/ShaderPrograms.cpp \
    SOQAH/SOQAHArcs3.cpp \
    SOQAH/SOQAHCompositeCurve3.cpp \
    SOQAH/SOQAHCompositeSurface3.cpp

