#version 330 core

in vec2 vUv;

uniform vec4  uMainColor;
uniform float uRadius;
uniform float uRectangleWidth;
uniform float uRectangleHeight;

out vec4 color;

float roundRectangle(vec2 position, vec2 rectangleSize, float radius) 
{
    return length(max(abs(position)-rectangleSize+vec2(radius,radius),0.0))-radius;
}

void main()
{
	// If radius is equal to zero, it means no 
	// round corner, so we simply returns the widget's color
	if(uRadius==0.0f)
	{
		color = uMainColor;
		return;
	}

	float realX = (vUv.x - 0.5f)*uRectangleWidth;
	float realY = (vUv.y - 0.5f)*uRectangleHeight;

	
	float distance = roundRectangle
	(
		vec2(realX, realY),
		vec2(uRectangleWidth*0.5, uRectangleHeight*0.5),
		 uRadius
	);

	distance= min(distance,0.0);

	// Smoothing a bit to add some anti aliasing
	
	float smoothStrength = 1.0;

	float smoothedAlpha =  smoothstep(0.0f, 1.0f,-distance/smoothStrength);
	
	vec4 c = uMainColor;
	c.a = smoothedAlpha;

	color = c;
}