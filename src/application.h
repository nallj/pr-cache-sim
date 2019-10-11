#ifndef APPLICATION
#define APPLICATION

#include <map>
#include <vector>

#include "components/icap.h"
#include "components/prc.h"

class application {

	icap* application_icap_;
	prc* application_prc_;

	double simulator_speed_;
	double static_region_speed_;

	std::multimap<unsigned, unsigned>* static_modules_;
	std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>* reconfigurable_modules_;

public:
	std::string name_;
	std::string file_;

	application();
	application(
		icap* application_icap,
		prc* application_prc,
		double static_region_speed,
		std::multimap<unsigned, unsigned>* static_modules,
		std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>* reconfigurable_modules,
		double fastest_module_speed
	);
	~application();


	void printApplicationDetails();

	icap* getIcap();
	prc* getPrc();

	double getSimulatorSpeed();
	unsigned getStaticRegionCount();
	double getStaticRegionSpeed();
	std::multimap<unsigned, unsigned>* getStaticModules();
	unsigned getReconfigurableRegionCount();
	unsigned getReconfigurableRegionBitstreamSize(unsigned region_id);
	std::vector<unsigned> getReconfigurableRegionBitstreamSizes();
	double getReconfigurableRegionSpeed(unsigned region_id, unsigned module_id);
	std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>* getReconfigurableModules();

	void printDetails(unsigned indents);
};

#endif
