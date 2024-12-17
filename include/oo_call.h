#ifndef OO_CALL_H
#define OO_CALL_H

#include "amxxmodule.h"

#include <vector>
#include <stack>
#include <string>

namespace oo
{
	using AmxxForward = int;
	using ArgDataList = std::vector<long>;
	using ArgTypeList = std::vector<int>;
	using ObjectHash = size_t;

	struct Call
	{
		AMX *amx;
		cell result;
		cell state;
		ObjectHash _this;
		const ArgTypeList *list; 
		ArgDataList *data;
		std::vector<cell> *celldata;
		std::vector<std::string> *strdata;
	};

	class CallStack
	{
	public:
		void Push(AMX *amx, ObjectHash _this, const ArgTypeList *list, ArgDataList *data, std::vector<cell> *celldata, std::vector<std::string> *strdata);
		void Pop();
		Call* Get();
		void Clear();

		static CallStack* Instance();

		CallStack(const CallStack&) = delete;
		CallStack& operator=(const CallStack&) = delete;

		CallStack() {}
	private:

		std::stack<Call> m_calls;
	};
}

#endif // OO_CALL_H