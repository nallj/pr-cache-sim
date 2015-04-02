#include <iostream>// includes

int main(int argc, char** argv){

	std::cout << std::endl << "Drachma : Reconfigurable Computing Cache Simulator" << std::endl;
    std::cout << std::endl << "Inputed " << (argc - 1) << " arguments. They are as follows" << std::endl;

    for (int i = 1; i < argc; ++i) {
        std::cout << "\t" << argv[i] << std::endl;
    }

    std::cout << std::endl << std::endl;

	return 0;
}
