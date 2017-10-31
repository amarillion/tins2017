#ifndef _DISSOLVE_H_
#define _DISSOLVE_H_

#include <functional>

// forward declaration;
struct ALLEGRO_SHADER;

/**
 * Wrapper for a time-dependent shader
 * that produces a dissolve effect
 */
class DissolveEffect {

private:
	ALLEGRO_SHADER *shader;

public:
	DissolveEffect();

	/**
	 * time: from start to completely dissolved, between 0.0 and 1.0
	 * closure: drawing code to execute between enabling and disabling the shader.
	 */
	void withPattern(float time, std::function<void()> closure) {
		enable(time);
		closure();
		disable();
	}

	void enable(float time);
	void disable();

	ALLEGRO_SHADER *getShader() { return shader; }
};


#endif
