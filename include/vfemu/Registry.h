
#ifndef VFEMU_REGISTRY_H
#define VFEMU_REGISTRY_H

#include <list>
#include <vfemu/types.h>


namespace vfemu {


/*
template<typename T>
concept Keyable = requires (T a) {
	a.name;
};
*/


template <typename T> 
class Registry {
public:
	inline T* get(const char* name) {
		for (auto v : list) {
			if (v->name == name) {
				return v;
			}
		}
		return nullptr;
	}

	inline Status add(T* v) {
		for (auto _v : list) {
			if (_v->name == v->name) {
				return Status::ERR_EXIST;
			}
		}
		list.push_front(v);
		return Status::SUCCESS;
	}

	inline Status remove(T* v) {
		list.remove(v);
		return Status::SUCCESS;
	}

private:
	std::list<T*> list;
};

	
} // namespace vfemu


#endif
