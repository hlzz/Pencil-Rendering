uniform sampler2D texSrc;
uniform sampler2D texCur;
uniform sampler3D texPencil;
uniform vec2 offset[9];

vec3 decode(vec2 n)
{
	vec3 normal;
	normal.z = dot(n, n) * 2.0 - 1.0;
	normal.xy = normalize(n) * sqrt(1 - normal.z * normal.z);
	return normal;
}

vec2 rotateDirections(vec2 dir, vec2 uv)					//旋转theta度
{
	return vec2(dir.x * uv.x - dir.y * uv.y,
				dir.x * uv.y + dir.y * uv.x);
}

void main(void) 
{
	float sample[9];
	vec4 curvature = texture2D(texCur,gl_TexCoord[0].st);
	vec3 c = decode(curvature.xy);							//主曲率方向
	vec3 z = vec3(0.0,0.0,1.0);
	vec3 r = vec3(1.0,0.0,0.0);
	vec3 t1 = c - dot(c,z)*z;
	float theta1 = acos(dot(t1,r)/length(t1));
	vec3 mc = decode(curvature.zw);
	vec3 t2 = mc - dot(mc,z)*z;
	float theta2 = acos(dot(t2,r)/length(t2));

	vec4 gBuffer = texture2D(texSrc, gl_TexCoord[0].st);
	vec3 normal = normalize(decode(texture2D(texSrc, gl_TexCoord[0].st).rg));
	float sum = 0.0;
	for (int i = 0; i < 9; i++) 
	{
		vec3 n = normalize(decode(texture2D(texSrc, gl_TexCoord[0].st + offset[i]).rg));		//附近点的法向量
		sample[i] = texture2D(texSrc, gl_TexCoord[0].st + offset[i]).b;			//深度值z
		sum += dot(normal, n);
	}
	sum /= 9;

//    -1 -2 -1       1 0 -1  
// H = 0  0  0   V = 2 0 -2 
//     1  2  1       1 0 -1 
// 
// result = sqrt(H^2 + V^2) 

	float horizEdge = sample[2] + (2.0*sample[5]) + sample[8] - 
					(sample[0] + (2.0*sample[3]) + sample[6]); 
 
	float vertEdge = sample[0] + (2.0*sample[1]) + sample[2] - 
					(sample[6] + (2.0*sample[7]) + sample[8]);

	//int temp = gBuffer.a * 32;
	float slice = gBuffer.a;				//纹理的层次——深度
	vec4 color = vec4(0.0,0.0,0.0,0.0);
	//slice = 0.1;
	//vec3 texCoor = vec3(gl_TexCoord[0].st, 0.6);
	color += texture3D(texPencil, vec3(5 * rotateDirections(vec2(cos(theta1), sin(theta1)), gl_TexCoord[0].st), slice));
	color += texture3D(texPencil, vec3(5 * rotateDirections(vec2(cos(0.2), sin(0.2)), gl_TexCoord[0].st), slice))*0.5;
	color += texture3D(texPencil, vec3(5 * rotateDirections(vec2(cos(-2.6), sin(-2.6)), gl_TexCoord[0].st), slice))*0.5;
	color *= 0.5;
	color *= color;					//增强对比度

	if (gBuffer.b < 1.0)
		if(sum<0.4)
		{
			gl_FragColor = vec4(0.0,0.0,0.0,1.0);
		}
		else
		{
			//gl_FragColor = vec4(1.0 - sqrt((horizEdge * horizEdge) + (vertEdge * vertEdge)) + sum - 2.0 + color);
			gl_FragColor = color;
		}
	else
		gl_FragColor = vec4(1.0 - sqrt((horizEdge * horizEdge) + (vertEdge * vertEdge)));
}