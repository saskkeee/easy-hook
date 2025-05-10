#pragma once

#include <type_traits>
#include "minhook/include/MinHook.h"

struct conv_tag_t {};

struct fastcall_tag_t : conv_tag_t {};
struct cdecl_tag_t : conv_tag_t {};
struct stdcall_tag_t : conv_tag_t {};
struct noconv_tag_t : conv_tag_t {};

template<typename Ret,typename Convention, typename... Args>
struct func_call_helper_t;

template<typename Ret,typename... Args>
struct func_call_helper_t<Ret, fastcall_tag_t, Args...> {
	using type = Ret(__fastcall*)(Args...);
};

template<typename Ret, typename... Args>
struct func_call_helper_t<Ret, stdcall_tag_t, Args...> {
	using type = Ret(__stdcall*)(Args...);
};

template<typename Ret, typename... Args>
struct func_call_helper_t<Ret, cdecl_tag_t, Args...> {
	using type = Ret(__cdecl*)(Args...);
};

template<typename Ret, typename... Args>
struct func_call_helper_t<Ret, noconv_tag_t, Args...> {
	using type = Ret(*)(Args...);
};

template<typename T>
concept is_function = std::is_function<T>::value;

class easy_hook
{
public:
	static bool init() 
	{
		return MH_Initialize() == MH_OK;
	}

	template<typename FnType,typename = std::enable_if_t<is_function<FnType>>>
	FnType* as() noexcept
	{
		return reinterpret_cast<FnType*>(fnPtr);
	}

	template<typename ReturnType,typename ConventionTag = noconv_tag_t,typename... Args,typename = std::enable_if_t<std::is_base_of<conv_tag_t,ConventionTag>::value>>
	ReturnType call(Args... args) noexcept
	{
		using fnType = typename func_call_helper_t<ReturnType, ConventionTag, Args...>::type;

		return reinterpret_cast<fnType>(fnPtr)(std::forward<Args>(args)...);
	}

#ifdef LOG


#define LOG_SUCCESS() std::cout << " [ success ] "
#define LOG_ERROR() std::cout << " [ error ] "

public:
	bool hook(void* pTarget, void* pDetour,const std::string_view hookName) noexcept
	{
		bool result = this->hook(pTarget, pDetour);

		if (!result)
		{
			LOG_ERROR() << "failed to hook " << hookName << "\n";
			return false;
		}

		LOG_SUCCESS() << "hooked " << hookName << "\n";

		return true;
	}

	bool virtual_hook(void* pTarget, unsigned int index, void* pDetour,const std::string_view hookName)
	{
		bool result = this->virtual_hook(pTarget,index,pDetour);

		if (!result)
		{
			LOG_ERROR() << "failed to virtual-hook " << hookName << "\n";
			return false;
		}
		
		LOG_SUCCESS() << "hooked " << hookName << "\n";

		return true;
	}

private:
#endif
	bool hook(void* pTarget,void* pDetour) noexcept
	{
		return MH_CreateHook(pTarget, pDetour, reinterpret_cast<void**>(&fnPtr)) == MH_OK && 
			MH_EnableHook(pTarget) == MH_OK;
	}

	bool virtual_hook(void* pTarget,unsigned int index, void* pDetour)
	{
		void** vmt = *reinterpret_cast<void***>(pTarget);
		return MH_CreateHook(vmt[index], pDetour, reinterpret_cast<void**>(&fnPtr)) == MH_OK &&
			MH_EnableHook(vmt[index]) == MH_OK;
	}
private:
	void* fnPtr;
};