/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#version 450

layout(location = 0) in vec2 in_position;

void main() {
	gl_Position = vec4(in_position, 0.0, 1.0);
}
