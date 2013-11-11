varying vec4 position;
attribute vec3 curvatureDirection;
attribute vec3 minCurvatureDirection;
varying vec3 curvature,minCurvature;

void main()
{
	curvature = normalize(vec3(gl_ModelViewMatrix * vec4(curvatureDirection, 0.0)));
	minCurvature = normalize(vec3(gl_ModelViewMatrix * vec4(minCurvatureDirection,0.0)));
	gl_Position = ftransform();
	position = gl_Position;
}