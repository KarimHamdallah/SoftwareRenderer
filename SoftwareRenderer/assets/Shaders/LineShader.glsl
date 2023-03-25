#type vertex

#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in int aEntityID;

uniform mat4 u_ProjectionView;


out vec4 v_Color;
out flat int v_EntityID;

void main()
{
   v_Color = aColor;
   v_EntityID = aEntityID;
   gl_Position = u_ProjectionView * vec4(aPosition, 1.0);
}

#type fragment


#version 460 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int MousePickingID;
        
in vec4 v_Color;
in flat int v_EntityID;

void main()
{    
    // Output to screen
    FragColor = v_Color;
    MousePickingID = v_EntityID;
}