#include "Log.h"

namespace Messanger
{ 
    void Log::Initialize()
    {
        spdlog::set_pattern("%^[%T] %s-%L: %v%$ %#");
        m_Logger = spdlog::stdout_color_mt("Directx");
    	m_Logger->set_level(spdlog::level::level_enum::trace);
    }
   
}