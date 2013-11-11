varying vec3 normal, lightDirection, lightDir, eyeVec, halfVector;	//法线，光线方向
varying vec4 position;						//顶点位置，之后编码编程2维向量
varying vec4 diffuse,ambientGlobal,ambient;	//光源的一些属性
varying float dist;							//光源到顶点的距离,用于计算衰减
attribute vec3 curvatureDirection;			//主曲率方向
attribute vec3 minCurvatureDirection;

void main()
{
	vec4 ecPos;
	vec3 aux;
	normal = normalize(gl_NormalMatrix * gl_Normal);
	ecPos = gl_ModelViewMatrix * gl_Vertex;						//顶点位置
	aux = vec3(gl_LightSource[0].position-ecPos);
	lightDir = normalize(aux);
	dist = length(aux);
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);

	//计算diffuse,ambient和globalAmbient
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

	lightDirection = normalize(vec3(gl_LightSource[0].position));

	gl_Position = ftransform();
	position = gl_Position;
}