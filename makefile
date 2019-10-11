# makefile for Drachma:
# a Reconfigurable Computing Cache Replacement Simulator
# written by James B. Nall

# Copyright (C) 2015-2017, All Rights Reserved

CXX = g++
CXXFLAGS = -std=c++11 -o

all: src/main.cpp src/globals.h src/application.h src/application.cpp src/device.h src/device.cpp \
	src/fileHandler.h src/fileHandler.cpp src/replAlg.h src/replAlg.cpp src/signalContext.h \
	src/signalContext.cpp src/traceToken.h src/traceToken.cpp src/wallet.h src/wallet.cpp \
	src/algs/fifoAlg.h src/algs/fifoAlg.cpp src/algs/randomAlg.h src/algs/randomAlg.cpp \
	src/components/icap.h src/components/icap.cpp src/components/prc.h src/components/prc.cpp \
	src/components/prrLevelController.h src/components/prrLevelController.cpp src/storage/memoryLevel.h \
	src/storage/memoryLevel.cpp src/storage/module.h src/storage/module.cpp \
	src/storage/reconfigurableRegions.h src/storage/reconfigurableRegions.cpp src/storage/storageUnit.h \
	src/storage/storageUnit.cpp

	cd src && \
	$(CXX) -g $(CXXFLAGS) ../drachma main.cpp globals.h application.h application.cpp device.h \
	device.cpp fileHandler.h fileHandler.cpp replAlg.h replAlg.cpp signalContext.h signalContext.cpp \
	traceToken.h traceToken.cpp wallet.h wallet.cpp algs/fifoAlg.h algs/fifoAlg.cpp algs/randomAlg.h \
	algs/randomAlg.cpp components/icap.h components/icap.cpp components/prc.h \
	components/prc.cpp components/prrLevelController.h components/prrLevelController.cpp \
	storage/memoryLevel.h storage/memoryLevel.cpp storage/module.h storage/module.cpp \
	storage/reconfigurableRegions.h storage/reconfigurableRegions.cpp storage/storageUnit.h \
	storage/storageUnit.cpp -lboost_regex

clean:
	rm -f *.o drachma
