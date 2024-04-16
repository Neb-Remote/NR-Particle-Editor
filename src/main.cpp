#include "App.hpp"
#include "Utils.hpp"

#include <SFML/GpuPreference.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

// Set so that discrete notebook/laptop GPUs are used for
// rendering
SFML_DEFINE_DISCRETE_GPU_PREFERENCE

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    try {
        App app;
        app.run();
    } catch (const std::runtime_error& e) {
        spdlog::error(e.what());
        spdlog::shutdown();
        std::abort();
    }

    return 0;
}
