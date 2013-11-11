varying vec3 normal, lightDirection, lightDir, eyeVec, halfVector;
varying vec4 position;
varying vec4 diffuse,ambientGlobal, ambient;
varying float dist;

vec2 encode(vec3 n)
{
	return normalize(n.xy) * sqrt(n.z * 0.5 + 0.5);
}

void main()
{
	vec3 n,halfV,viewV,ldir;
	float NdotL,NdotHV;
	vec4 color = ambientGlobal;
	float att;
	n = normalize(normal);					//fragment shader不能写入一个varying变量
	NdotL = max(dot(n,normalize(lightDir)),0.0);
	if (NdotL > 0.0) {
		att = 1.0 / (gl_LightSource[0].constantAttenuation +
				gl_LightSource[0].linearAttenuation * dist +
				gl_LightSource[0].quadraticAttenuation * dist * dist);
		color += att * (diffuse * NdotL + ambient);
		halfV = normalize(halfVector);

		NdotHV = max(dot(n,halfV),0.0);
		color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular *
						pow(NdotHV,gl_FrontMaterial.shininess);
	}
	float intensity = max(dot(normal, lightDirection), 0.0);
	gl_FragColor = vec4(encode(normalize(normal)), position.z * position.z * 0.001, NdotL);
}