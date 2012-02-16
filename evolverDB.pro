# usage: qmake -spec macx-xcode
# in Xcode 4, change:
# Architecture 64-bit Intel
# C/C++ Complier Version to "LLVM compiler 2.0"
# (or add -fpermissive flag)
# Enable OpenMP Support to "Yes"
# Build assumes mysql includes are in a directory, for mac:
# sudo ln -s /usr/local/include/mysql /usr/local/mysql/include
#
# Xcode 3 puts the executable in locomotor/build/Release
# 
VPATH = ./src ./src/neat
SOURCES += \
    EvolverDB.cpp \
    ArticulatedBody.cpp \
    HumanoidArticulatedBody.cpp \
    BodyState.cpp \
    Controller.cpp \
    ControllerFactory.cpp \
    NeatEvolver.cpp \
    PhysicsSimulator.cpp \
    Tardis.cpp \
    Utils.cpp \
    MySqlDBEvoReporter.cpp \
    MySqlDB.cpp \
    ParamsFromDB.cpp \
    StrategyFitnessFunc.cpp \
    FitnessFuncFactory.cpp \
    PopulationFitness.cpp \
    EvaluationFactory.cpp \
    EvaluationFactoryDB.cpp \
    NeatController.cpp \
    Evaluation.cpp \
    PhysicsSimulatorFactory.cpp \
    ArticulatedBodyFactory.cpp \
	networks.cpp \
    genetics.cpp \
    neatmain_ng.cpp \
    src/AcrobotArticulatedBody.cpp \
    src/AcrobotFitnessFunc.cpp
	#    src/AcrobotArticulatedBody.cpp \

HEADERS += \
    Locatable.hpp \
    ArticulatedBody.hpp \
    HumanoidArticulatedBody.hpp \
    PhysicsSimulator.hpp \
    PopulationFitness.hpp \
    BalanceFitnessFunc.hpp \
    ConstantFitnessFunc.hpp \
    FitnessFunc.hpp \
    FitnessFuncFactory.hpp \
    StepFitnessFunc.hpp \
    VelocityFitnessFunc.hpp \
    WalkFitnessFunc.hpp \
    StrategyFitnessFunc.hpp \
    ControllerFactory.hpp \
    Tardis.hpp \
    All.hpp \
    Else.hpp \
    ElseException.hpp \
    Math.hpp \
    Singleton.hpp \
    Utils.hpp \
    MySqlDBEvoReporter.hpp \
    MySqlDB.hpp \
    ParamsFromDB.hpp \
    Controller.hpp \
    EvoReporter.hpp \
    Evaluation.hpp \
    Evolver.hpp \
    NeatEvolver.hpp \
    DbPerformance.hpp \
    DBExperiment.h \
    EvaluationFactory.hpp \
    EvaluationFactoryDB.hpp \
    NeatController.hpp \
    PhysicsSimulatorFactory.hpp \
    ArticulatedBodyFactory.hpp \
    src/AcrobotArticulatedBody.hpp \
    src/AcrobotFitnessFunc.hpp \
    src/BodyState.hpp
	#    src/AcrobotArticulatedBody.hpp \

DESTDIR = bin
TARGET = evolverDB
LIBS += -lmysqlclient \
    -lode
CONFIG += debug_and_release \
    console \
    x86_64
CONFIG -= x86
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -fpermissive -fopenmp
INCLUDEPATH += ./src

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
        /opt/local/include

    LIBS += -L./lib/x86_64 \
        -L./lib \
        -L/usr/local/lib \
        -L/usr/local/mysql/lib \
        -L/opt/local/lib \
        -L/opt/local/lib/mysql
}
OTHER_FILES += DevelopmentNotes.txt
