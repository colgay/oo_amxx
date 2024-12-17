#include "oo_call.h"
#include "oo_defs.h"

namespace oo
{
	void CallStack::Push(AMX *amx, ObjectHash _this, const ArgTypeList *list, ArgDataList *data, std::vector<cell> *celldata, std::vector<std::string> *strdata)
	{
		m_calls.emplace(Call{amx, 0, OO_CONTINUE, _this, list, data, celldata, strdata});
	}

	void CallStack::Pop()
	{
		m_calls.pop();
	}

	Call* CallStack::Get()
	{
		return &m_calls.top();
	}

	void CallStack::Clear()
	{
		while (!m_calls.empty())
		{
			m_calls.pop();
		}
	}

	CallStack* CallStack::Instance()
	{
		static CallStack cs;
		return &cs;
	}
}