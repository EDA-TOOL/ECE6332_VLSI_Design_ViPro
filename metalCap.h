#ifndef METALCAP_H_
#define METALCAP_H_

#include <map>

void createMetalCapMap (map<pair<int,string>, float*>&);

struct RC {
	float res;
	float cap;
};
typedef struct RC ResCap;

ResCap InterconnectRC(int techNum, string intcLevel, string metal, float* inputParam);

#endif/* METALCAP_H_ */
