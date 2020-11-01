set(CMAKE_BUILD_TYPE Debug)

add_executable(drachma
  algs/cache/fifoAlg.cpp
  algs/cache/randomAlg.cpp
  algs/cache/replAlg.cpp
  algs/task/fcfsAlg.cpp
  algs/task/schedulingAlg.cpp
  device.cpp
  components/icap.cpp
  components/prc.cpp
  components/prrLevelController.cpp
  fileHandler.cpp
  main.cpp
	signalContext.cpp
  specs/application.cpp
	storage/memoryLevel.cpp
  storage/module.cpp
  storage/reconfigurableRegions.cpp
	storage/storageUnit.cpp
  traceToken.cpp
  userError.cpp
  wallet.cpp
)

target_compile_features(drachma PRIVATE cxx_std_17)

target_link_libraries(drachma
  PRIVATE cpp_graph_json
  PRIVATE ${Boost_LIBRARIES}
)
