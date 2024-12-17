#ifndef OO_FORWARD_H
#define OO_FORWARD_H

#include <vector>
#include <any>
#include <string>

#include "amxxmodule.h"

namespace oo
{
	using AmxxForward = int;
	using ArgTypeList = std::vector<int>;
	using HookList = std::vector<AmxxForward>;
	using ObjectHash = size_t;
	using ArgDataList = std::vector<long>;

	class Forward
	{
	public:
		Forward(AMX *amx, cell *params, const HookList *pre, const HookList *post, ObjectHash _this, const ArgTypeList *args)
			: m_amx(amx), m_params(params), m_pre(pre), m_post(post), m_this(_this), m_arg_list(args)
		{}

		static AmxxForward Create(AMX *amx, const char *callback, const ArgTypeList *args=nullptr);

		void PrepareArgsData();

		cell Call(AmxxForward fwd);
		cell ExecuteHooks(const HookList *hooks);
		cell Execute(AmxxForward fwd);

	private:
		AMX *m_amx;
		cell *m_params;
		const HookList *m_pre;
		const HookList *m_post;
		ObjectHash m_this;
		const ArgTypeList *m_arg_list;
		int m_esp_size;
		ArgDataList m_arg_data;
		std::vector<cell> m_cell_data;
		std::vector<std::string> m_str_data;
	};
}

#endif