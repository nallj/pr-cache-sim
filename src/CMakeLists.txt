set(CMAKE_BUILD_TYPE Debug)

add_executable(drachma
  algs/bitstream/bitstreamSelector.cpp
  algs/bitstream/lftFeSelector.cpp
  algs/cache/fifoAlg.cpp
  algs/cache/randomAlg.cpp
  algs/cache/replAlg.cpp
  algs/task/fcfsScheduler.cpp
  algs/task/taskScheduler.cpp
  device.cpp
  components/icap.cpp
  components/prc.cpp
  components/prrLevelController.cpp
  fileHandler.cpp
  main.cpp
	signalContext.cpp
  specs/application.cpp
  specs/application-yaml.cpp
	storage/memoryLevel.cpp
  storage/rrModule.cpp
  storage/reconfigurableRegions.cpp
	storage/storageUnit.cpp
  traceToken.cpp
  userError.cpp
  wallet.cpp
)

target_compile_features(drachma PRIVATE cxx_std_20)

target_link_libraries(drachma
  PRIVATE cpp_graph_json
  PRIVATE ${Boost_LIBRARIES}
  PRIVATE yaml-cpp
)
