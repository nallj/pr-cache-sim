# makefile for Drachma:
# a Reconfigurable Computing Cache Replacement Simulator
# written by James B. Nall

# Copyright (C) 2015-2016, All Rights Reserved

# src/components/prrLevelController.cpp src/storage/reconfigurableRegions.cpp src/storage/memoryLevel.cpp src/storage/module.cpp
drachma: src/main.cpp src/globals.h src/wallet.h src/fileHandler.h src/fileHandler.cpp src/wallet.cpp src/device.h src/device.cpp src/storage/storageUnit.h src/storage/storageUnit.cpp src/replAlg.h src/traceToken.h src/traceToken.cpp src/storage/module.h src/algs/randomAlg.h src/algs/randomAlg.cpp src/algs/fifoAlg.h src/algs/fifoAlg.cpp src/signalContext.h src/components/icap.h src/components/prc.h src/application.h src/storage/memoryLevel.h src/storage/reconfigurableRegions.h src/components/prrLevelController.h

# src/components/prrLevelController.cpp src/storage/reconfigurableRegions.cpp src/storage/memoryLevel.cpp src/storage/module.cpp
	g++ -std=c++11  -o drachma src/main.cpp src/globals.h src/wallet.h src/fileHandler.h src/fileHandler.cpp src/wallet.cpp src/device.h src/device.cpp src/storage/storageUnit.h src/storage/storageUnit.cpp src/replAlg.h src/traceToken.h src/traceToken.cpp src/storage/module.h src/algs/randomAlg.h src/algs/randomAlg.cpp src/algs/fifoAlg.h src/signalContext.h src/algs/fifoAlg.cpp  src/components/icap.h src/components/prc.h src/application.h src/storage/memoryLevel.h src/storage/reconfigurableRegions.h src/components/prrLevelController.h -lboost_regex
