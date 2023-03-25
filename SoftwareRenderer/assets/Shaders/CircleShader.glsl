#type vertex

#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aLocalPosition;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aThickness;
layout (location = 4) in float aFade;
layout (location = 5) in int aEntityID;

uniform mat4 u_ProjectionView;


out vec3 v_LocalPosition;
out vec4 v_Color;
out flat float v_Thickness;
out flat float v_Fade;
out flat int v_EntityID;

void main()
{
   v_LocalPosition = aLocalPosition;
   v_Color = aColor;
   v_Thickness = aThickness;
   v_Fade = aFade;
   v_EntityID = aEntityID;
   gl_Position = u_ProjectionView * vec4(aPosition, 1.0);
}

#type fragment


#version 460 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int MousePickingID;
        
in vec3 v_LocalPosition;
in vec4 v_Color;
in flat float v_Thickness;
in flat float v_Fade;
in flat int v_EntityID;

float LerpVlaue(float edge0, float edge1, float x)
{
  // return vlaue from 0 to 1 repersent position of x value in between edge0 and edge1
  // 0.0 >> x = edge0
  // 1.0 >> x = edge1
  // 0.5 >> x in half way between them
  // otherwise return x
  
  float t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return t * t * (3.0 - 2.0 * t);
}

void main()
{       
    // Render Circle With Raduis 1.0
    float Raduis = 1.0;
    float distance = length(v_LocalPosition);
    
    float col = 0.0;
    // if inside circle
    if(distance < Raduis && distance > 1.0 - v_Thickness)
    {
      col = LerpVlaue(Raduis, Raduis - v_Fade, distance) *
            LerpVlaue(1.0 - v_Thickness, 1.0 - v_Thickness + v_Fade, distance);
    }
    
    if(col == 0) // pixel is outside the circle
       discard;

    vec3 FinalColor = v_Color.rgb * col;
    
    // Output to screen
    FragColor = vec4(FinalColor, v_Color.a);
    MousePickingID = v_EntityID;
}