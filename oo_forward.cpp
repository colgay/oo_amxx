#include "oo_forward.h"
#include "oo_call.h"
#include "oo_defs.h"
//#include "oo_ustring.h"
#include "oo_asm.h"

#include <string>
#include <memory>

namespace oo
{
	AmxxForward Forward::Create(AMX *amx, const char *callback, const ArgTypeList *args_list)
	{
		auto num_args = (args_list != nullptr) ? args_list->size() : 0u;

		std::vector<long> args_vec;
		if (num_args > 0)
			args_vec.reserve(num_args);

		for (auto i = 0u; i < num_args; i++)
		{
			switch (args_list->at(i))
			{
				case OO_CELL:
					args_vec.emplace_back(FP_CELL);
					break;
				
				case OO_BYREF:
					args_vec.emplace_back(FP_CELL_BYREF);
					break;

				case OO_STRING:
					args_vec.emplace_back(FP_STRING);
					break;

				case OO_STRING_REF:
					args_vec.emplace_back(FP_STRINGEX);
					break;

				default:
					args_vec.emplace_back(FP_ARRAY);
					break;
			}
		}

		int start = static_cast<int>(num_args);
		long *args_data = (start > 0) ? args_vec.data() : nullptr;
		int esp_size = sizeof(cell) + (start * sizeof(cell)) + sizeof(char*) + sizeof(AMX*);
		AmxxForward result = AsmRegisterForward(amx, callback, args_data, start, esp_size);
		return result;
	}

	cell Forward::Execute(AmxxForward fwd)
	{
		auto num_args = (m_arg_list != nullptr) ? m_arg_list->size() : 0u;
		int start = static_cast<int>(num_args);
		long *arg_data = (start > 0) ? m_arg_data.data() : nullptr;

		int esp_size = m_esp_size + sizeof(cell);
		cell result = AsmExecuteForward(fwd, arg_data, start, esp_size);

		for (auto i = 0u; i < num_args; i++)
		{
			if (m_arg_list->at(i) == OO_STRING_REF)
			{
				const char *str = reinterpret_cast<char*>(arg_data[i]);
				MF_SetAmxString(m_amx, m_params[i], str, std::strlen(str));
			}
		}

		return result;
	}

	void Forward::PrepareArgsData()
	{
		m_esp_size = 0;

		auto arg_count = (m_arg_list != nullptr) ? m_arg_list->size() : 0;
		if (arg_count > 0)
		{
			m_arg_data.reserve(arg_count);
			m_cell_data.reserve(arg_count);
			m_str_data.reserve(arg_count);
		}

		for (size_t i = 0; i < arg_count; i++)
		{
			switch (m_arg_list->at(i))
			{
				case OO_CELL:
				{
					m_cell_data.emplace_back(*MF_GetAmxAddr(m_amx, m_params[i]));
					m_str_data.emplace_back("");
					m_arg_data.emplace_back(static_cast<long>(m_cell_data.back()));
					m_esp_size += sizeof(cell);
					break;
				}
				case OO_BYREF:
				{
					m_cell_data.emplace_back(0);
					m_str_data.emplace_back("");
					m_arg_data.emplace_back(reinterpret_cast<long>(MF_GetAmxAddr(m_amx, m_params[i])));
					m_esp_size += sizeof(cell*);
					break;
				}
				case OO_STRING:
				{
					int len = 0;
					m_cell_data.emplace_back(0);
					m_str_data.emplace_back(MF_GetAmxString(m_amx, m_params[i], 0, &len));
					m_arg_data.emplace_back(reinterpret_cast<long>(m_str_data.back().data()));
					m_esp_size += sizeof(char*);
					break;
				}
				case OO_STRING_REF:
				{
					int len = 0;
					m_cell_data.emplace_back(0);
					m_str_data.emplace_back(MF_GetAmxString(m_amx, m_params[i], 0, &len));
					auto &str = m_str_data.back();
					str.resize(std::max(len, 255));
					std::fill(str.begin() + len, str.end(), '\0');
					m_arg_data.emplace_back(reinterpret_cast<long>(str.data()));
					m_esp_size += sizeof(char*);
					break;
				}
				default:
				{
					m_cell_data.emplace_back(MF_PrepareCellArrayA(MF_GetAmxAddr(m_amx, m_params[i]), m_arg_list->at(i), true));
					m_str_data.emplace_back("");
					m_arg_data.emplace_back(static_cast<long>(m_cell_data.back()));
					m_esp_size += sizeof(cell);
					break;
				}
			}
		}
	}

	cell Forward::Call(AmxxForward fwd)
	{
		PrepareArgsData();
		CallStack::Instance()->Push(m_amx, m_this, m_arg_list, &m_arg_data, &m_cell_data, &m_str_data);

		cell ret = 0;
		if (ExecuteHooks(m_pre) < OO_SUPERCEDE)
		{
			ret = Execute(fwd);
			ExecuteHooks(m_post);
		}
		else
		{
			ret = CallStack::Instance()->Get()->result;
		}

		CallStack::Instance()->Pop();
		return ret;
	}

	cell Forward::ExecuteHooks(const HookList *hooks)
	{
		cell *ret = &CallStack::Instance()->Get()->state;

		if (hooks == nullptr)
			return *ret;

		for (auto hook : *hooks)
		{
			cell r = Execute(hook);
			*ret = (r > *ret) ? r : *ret;
			if (*ret >= OO_BREAK)
				break;
		}

		return *ret;
	}
}