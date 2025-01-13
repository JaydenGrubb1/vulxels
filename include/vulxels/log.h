/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <spdlog/spdlog.h>

#define VX_LOG(...) spdlog::info(__VA_ARGS__)
#define VX_WARN(...) spdlog::warn(__VA_ARGS__)
#define VX_ERROR(...) spdlog::error(__VA_ARGS__)
#define VX_CRIT(...) spdlog::critical(__VA_ARGS__)
#define VX_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define VX_TRACE(...) spdlog::trace(__VA_ARGS__)
