/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#version 450

layout(location = 0) in vec3 inColor;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(inColor, 1.0);
}
