
add_executable(drachma
  main.cpp
  application.cpp
  device.cpp
  fileHandler.cpp
  algs/replAlg.cpp
	signalContext.cpp
  traceToken.cpp
  wallet.cpp
  algs/fifoAlg.cpp
  algs/randomAlg.cpp
  components/icap.cpp
  components/prc.cpp
  components/prrLevelController.cpp
	storage/memoryLevel.cpp
  storage/module.cpp
  storage/reconfigurableRegions.cpp
	storage/storageUnit.cpp
)

target_compile_features(drachma PRIVATE cxx_std_17)

target_link_libraries(drachma
  PRIVATE cpp_graph_json
  PRIVATE ${Boost_LIBRARIES}
)
