uniform sampler2D tex;

//varying float fragAlpha;

void main( void )
{ 
	//gl_FragColor = gl_Color;
   gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * gl_Color;
   //vec4 texture = texture2D(tex, gl_TexCoord[0].st);
   //gl_FragColor = clamp(vec4(1.0, 1.0, 1.0, texture.a * 1.0), 0.0, 1.0);
}