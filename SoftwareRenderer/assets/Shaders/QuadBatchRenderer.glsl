#type vertex

#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aTexID;
layout (location = 4) in float aTilingFactor;
layout (location = 5) in int aEntityID;

uniform mat4 u_ProjectionView;

out vec2 v_TexCoords;
out vec4 v_Color;
out flat float v_TexID;
out float v_TilingFactor;
out flat int v_EntityID;

void main()
{
   v_TexCoords = aTexCoords;
   v_Color = aColor;
   v_TexID = aTexID;
   v_TilingFactor = aTilingFactor;
   v_EntityID = aEntityID;
   gl_Position = u_ProjectionView * vec4(aPosition, 1.0);
}

#type fragment

#version 460 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int MousePickingID;
        
in vec2 v_TexCoords;
in vec4 v_Color;
in flat float v_TexID;
in float v_TilingFactor;
in flat int v_EntityID;

uniform sampler2D u_Textures[32];

void main()
{
   int index = int(v_TexID);
   FragColor = texture(u_Textures[index], v_TexCoords * v_TilingFactor) * v_Color;
   MousePickingID = v_EntityID;
}