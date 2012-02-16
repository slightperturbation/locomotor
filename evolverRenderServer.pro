# usage: qmake -spec macx-xcode
# in Xcode 4, change:
#         Architecture 64-bit Intel
#         C/C++ Complier Version to "LLVM compiler 2.0"
#             (or add -fpermissive flag)
#         Enable OpenMP Support to "Yes"
#
# Build assumes mysql includes are in a directory, for mac:
#         sudo ln -s /usr/local/include/mysql /usr/local/mysql/include
# 
SOURCES += \
	src/EvolverRender.cpp \
	src/ArticulatedBody.cpp \
	src/HumanoidArticulatedBody.cpp \
	src/BodyState.cpp \
	src/Controller.cpp \
	src/NeatEvolver.cpp \
	src/PhysicsSimulator.cpp \
	src/Tardis.cpp \
	src/Utils.cpp \
	src/MySqlDBEvoReporter.cpp \
	src/MySqlDB.cpp \
	src/ControllerFactory.cpp \
	src/ParamsFromDB.cpp \
	src/StrategyFitnessFunc.cpp \
	src/FitnessFuncFactory.cpp \
	src/PopulationFitness.cpp \
	src/neat/networks.cpp \
	src/neat/genetics.cpp \
	src/neat/neatmain_ng.cpp \
	src/EvaluationFactory.cpp \
	src/EvaluationFactoryDB.cpp \
	src/NeatController.cpp \
	src/PhysicsSimulatorFactory.cpp \
	src/ArticulatedBodyFactory.cpp \
	src/RibRenderer.cpp \
	src/Evaluation.cpp \
    src/AcrobotArticulatedBody.cpp
	
HEADERS += \
	src/Locatable.hpp \
	src/ArticulatedBody.hpp \
	src/HumanoidArticulatedBody.hpp \
	src/PhysicsSimulator.hpp \
	src/PopulationFitness.hpp \
	src/BalanceFitnessFunc.hpp \
	src/ConstantFitnessFunc.hpp \
	src/FitnessFunc.hpp \
	src/FitnessFuncFactory.hpp \
	src/StepFitnessFunc.hpp \
	src/VelocityFitnessFunc.hpp \
	src/WalkFitnessFunc.hpp \
	src/StrategyFitnessFunc.hpp \
	src/ControllerFactory.hpp \
	src/Tardis.hpp \
	src/All.hpp \
	src/Else.hpp \
	src/ElseException.hpp \
	src/Math.hpp \
	src/Singleton.hpp \
	src/Utils.hpp \
	src/MySqlDBEvoReporter.hpp \
	src/MySqlDB.hpp \
	src/ParamsFromDB.hpp \
	src/Controller.hpp \
	src/EvoReporter.hpp \
	src/Evaluation.hpp \
	src/Evolver.hpp \
	src/NeatEvolver.hpp \
	src/DbPerformance.hpp \
	src/DBExperiment.h \
	src/EvaluationFactory.hpp \
	src/EvaluationFactoryDB.hpp \
	src/NeatController.hpp \
	src/PhysicsSimulatorFactory.hpp \
	src/ArticulatedBodyFactory.hpp \
	src/Renderer.hpp \
	src/Renderable.hpp \
	src/RibRenderer.hpp \
    src/AcrobotArticulatedBody.hpp
	

DESTDIR = bin
TARGET = evolverRenderServer
LIBS += -lmysqlclient -lode -lri -ltiff
CONFIG += debug_and_release console x86_64
CONFIG -= x86
CONFIG -= app_bundle
CONFIG += qt
QMAKE_CXXFLAGS += -fpermissive
QMAKE_CXXFLAGS += -DdDOUBLE
QMAKE_CXXFLAGS += -fopenmp
QMAKE_CFLAGS += -DdDOUBLE

# Linux configs, ok for mac.  obviously no support for windows yet.
INCLUDEPATH += ./src
INCLUDEPATH += /usr/local/include  #for ode/ode.h, boost/*.hpp
INCLUDEPATH += /usr/local/pixie/include #for ri.h

macx:
{
	message("Mac OS X build using $$[QT_VERSION]")

    INCLUDEPATH += \
        /usr/include \
        /usr/local/ode \
        /usr/local/include/gmtl-0.6.0 \
        /usr/local/include \
        /usr/local/include/mysql \
        /usr/local/mysql/include \
        /opt/local/include \
        /opt/local/include/boost \
        /opt/pixie/include

    LIBS += -L./lib/x86_64 \
        -L./lib \
        -L/usr/local/lib \
        -L/usr/local/mysql/lib \
        -L/opt/local/lib \
        -L/opt/local/lib/mysql \
        -L/opt/pixie/lib

}


#message(Qt version: $$[QT_VERSION])
#message(Qt is installed in $$[QT_INSTALL_PREFIX])
#message(Qt resources can be found in the following locations:)
#message(Documentation: $$[QT_INSTALL_DOCS])
#message(Header files: $$[QT_INSTALL_HEADERS])
#message(Libraries: $$[QT_INSTALL_LIBS])
#message(Binary files (executables): $$[QT_INSTALL_BINS])
#message(Plugins: $$[QT_INSTALL_PLUGINS])
#message(Data files: $$[QT_INSTALL_DATA])
#message(Translation files: $$[QT_INSTALL_TRANSLATIONS])
#message(Settings: $$[QT_INSTALL_SETTINGS])
#message(Examples: $$[QT_INSTALL_EXAMPLES])
#message(Demonstrations: $$[QT_INSTALL_DEMOS])
