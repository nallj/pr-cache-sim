
create: build-dir
	cd build && cmake -DBoost_DIR=/usr/local/Cellar/boost/1.73.0/ .. && make && mv src/drachma ..

debug: build-dir
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug -DBoost_DIR=/usr/local/Cellar/boost/1.73.0/ .. && make && mv src/drachma ..

build-dir:
	mkdir -p build

clean:
	rm -rf drachma build
