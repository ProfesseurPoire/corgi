#version 330 core

in vec2 vUv;

uniform vec4  uMainColor;
uniform float uRadius;
uniform float uRectangleWidth;
uniform float uRectangleHeight;

out vec4 color;

float roundRectangle(vec2 position, vec2 rectangleSize, float radius) 
{
    return length(max(abs(position)-rectangleSize+radius,0.0));
}

void main()
{
	float halfRectangleWidth  = uRectangleWidth/2.0f;
	float halfRectangleHeight = uRectangleHeight/2.0f;

	float realX = ((vUv.x*2.0f)-1.0f)*halfRectangleWidth;
	float realY = ((vUv.y*2.0f)-1.0f)*halfRectangleHeight;

	float distance = roundRectangle
	(
		vec2(realX, realY),
		vec2(halfRectangleWidth, halfRectangleHeight),
		 uRadius
	);

	if(distance>uRadius)
	{
		discard;
	}

	if(distance==0)
	{
		color = uMainColor;
		return;
	}

	float alpha = uRadius - distance;
		
	
	//distance= min(distance,0.0);

	// Smoothing a bit to add some anti aliasing

	float smoothedAlpha = smoothstep(0.0f, 1.0f,alpha);
	
	vec4 c = uMainColor;
	c.a = smoothedAlpha *c.a;

	color = c;
}