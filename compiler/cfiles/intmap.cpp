#include "intmap.h"
#include <map>

using namespace std;

using map_type= std::map<int, int>;

void init_intmap(intmap *m) {
    map<int, int> * cpp_map = new map<int, int>();
    m->data_structure = cpp_map;
}

void destroy_intmap(intmap *m) {
    // delete m->data_structure;
    // delete m;
}

int intmap_remove_upto(intmap* m, int key) {
    map_type & cpp_map = *static_cast<map_type*>(m->data_structure);
    
    auto first_element = cpp_map.begin();
    auto last_element = cpp_map.lower_bound(key);
    
    int answer = 0;
    while(first_element != last_element) {
        answer++;
        first_element++;
    }

    cpp_map.erase(cpp_map.begin(), cpp_map.lower_bound(key));

    return answer;
}

void intmap_insert(intmap *m, int key, int value) {
    map_type & cpp_map = *static_cast<map_type*>(m->data_structure);
    auto it = cpp_map.find(key);

    if (it != cpp_map.end()) {
        it->second = value;
    } else{
        cpp_map.insert(pair<int, int>(key, value));
    }
}

int intmap_get(intmap *m, int key, int *result) {
    map_type & cpp_map = *static_cast<map_type*>(m->data_structure);
    auto it = cpp_map.find(key);

    if(it != cpp_map.end()){
        *result = it->second;
        return true;
    }
    result = nullptr;
    return false;
}

void intmap_clear(intmap *m) {
    printf("intmap clear");
    map_type & cpp_map = *static_cast<map_type*>(m->data_structure);
    //cpp_map.erase(cpp_map.begin(), cpp_map.end());
}
