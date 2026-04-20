#pragma once

#include "run_context.hpp"

namespace kpl
{
    class IStage
    {
        public:
            virtual ~IStage() = default;

            virtual void execute(RunContext &context) = 0;
    };
}