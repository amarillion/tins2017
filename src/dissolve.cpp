#include "dissolve.h"
#include <allegro5/allegro.h>
#include <cstdio>

DissolveEffect::DissolveEffect()
{
	shader = al_create_shader(ALLEGRO_SHADER_AUTO);
	assert(shader);
/*
	const char *checker_vertex_shader_src =
		"attribute vec4 al_pos;"
		"uniform mat4 al_projview_matrix;"
		"void main()"
		"{"
		"	gl_Position = al_projview_matrix * al_pos;"
		"}";
*/
	const char *checker_pixel_shader_src =

		"#version 130\n"
		"uniform float uTime;"
		"uniform sampler2D al_tex;\n"
		"uniform bool al_use_tex;\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"

		"void main(void)"
		"{"
		"	vec2 st = gl_FragCoord.xy;"
		"	vec3 color = vec3(0.0);"
		"	float checkSize = 3.0;"
		"	st /= checkSize;"
		"	float step = 20.0;"
		"	float cellNo = mod(floor(st.x), 8.0) + 8.0 * mod(floor(st.y), 8.0);"
		"	float fmodResult = mod (floor(cellNo / step) + mod(cellNo, step) * step, 64.0);"
		"	if (fmodResult / 64.0 < uTime) {"
		"		discard;"
		"	}"
		"	if ( al_use_tex )\n"
		"		gl_FragColor = varying_color * texture2D( al_tex , varying_texcoord);\n"
		"	else\n"
		"		gl_FragColor = varying_color;\n"
		"}";

	bool ok;

	ok = al_attach_shader_source(shader, ALLEGRO_PIXEL_SHADER, checker_pixel_shader_src);
	//TODO: assert with message format...
	if (!ok) printf ("al_attach_shader_source failed: %s\n", al_get_shader_log(shader));
	assert(ok);

	ok = al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER,
			al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_VERTEX_SHADER));

	//TODO: assert with message format...
	if (!ok) printf ("al_attach_shader_source failed: %s\n", al_get_shader_log(shader));
	assert(ok);

	ok = al_build_shader(shader);
	//TODO: assert with message...
	if (!ok) printf ("al_build_shader failed: %s\n", al_get_shader_log(shader));
	assert(ok);
}

void DissolveEffect::enable(float time) {
	al_use_shader(shader);
	al_set_shader_float("uTime", time);
}

void DissolveEffect::disable() {
	al_use_shader(NULL);
}
