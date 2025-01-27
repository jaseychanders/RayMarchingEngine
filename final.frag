/*
Builds a basic ray marching rendering engine that can display some basic shapes using sdfs and an approximation of
a Sierpinskies triangle. Allows for controls like smoothness, destructive interference and smooth combining.
Complete list of references in README
Jasey Chanders
*/

// Width and height of the window
uniform vec2 u_resolution;
//Location of camera
uniform vec4 u_eyePos;
//How many levels of the fractal are showing
uniform int u_fractal_evel;
//Location of the light source
uniform vec3 u_light_pos;
//Overall object combination method
uniform vec2 u_combine_method;

//Data for each of the objects
uniform vec4 u_sphere_data;
uniform vec4 u_sphere_loc;
uniform vec4 u_box_data;
uniform vec4 u_box_loc;
uniform vec4 u_stri_data;
uniform vec4 u_stri_loc;
uniform vec4 u_pyrmid_data;
uniform vec4 u_pyrmid_loc;
uniform vec4 u_prism_data;
uniform vec4 u_prism_loc;

//Data structure to hold the distance to the nearest object and the color of that object
struct Point {
    float dist;
    vec4 color;
};

// Ray Marching Constants
#define SCENE_DEPTH 100.0 //Furthest a ray may travel
#define MAX_STEPS 100 //Most number of steps a ray can take
#define EPSILON 0.001 //Distance from an object the ray may stop at
float size = 1.0; //Size of triangle

/*
Smooth min
Combination of 2 functions from https://www.youtube.com/watch?app=desktop&v=BNZtUB7yhX4
Original color mixing algorythim
*/
Point smoothMin(Point a, Point b, float k){

    float diff = (SCENE_DEPTH *4.0) * (((a.dist/SCENE_DEPTH) - (b.dist/SCENE_DEPTH)));

    if(diff > 0.0){
        //b is closer
        a.color = ((a.color * (0.5 - diff)) + (b.color * (1.0 - (0.5 - diff))));

    } else{
        //a is closer
        a.color = ((b.color * (0.5 - abs(diff))) + (a.color * (1.0 - (0.5 - abs(diff)))));
    }

    a.dist = -(log(exp(k * -a.dist) + exp(k * -b.dist))/k);
    return a;
}

//Custom strict min function to include color
Point minWithColor(Point a, Point b){
    if(a.dist < b.dist){
        return a;
    } else if (b.dist < a.dist){
        return b;
    }else {
        a.color = vec4((a.color + b.color)/2.0);
        return a;
    }
}

//Custom strict destructive combination function. Includes color
Point destructiveWithColor(Point a, Point b){
    a.dist = max(a.dist, -b.dist);
    return a;
}

//SDF of a sphere at an arbitrary location
//Sphere. xyz is position w is radius
//Math taken from several sources listed in readme
//Modified to allow for color
Point sdfSphere(vec4 sphere, vec4 color, vec3 rayPoint){
    return Point(length(rayPoint.xyz-sphere.xyz) - sphere.w, color);
}

//SDF of a box in arbitrary location with smoothing factor
//SDF from https://iquilezles.org/articles/distfunctions/
//Smoothing algorythim taken from idea sources in readme
//Addition moveable center location added by me
//Modified to allow for color
Point sdfBox( vec3 rayPoint, float smoothing, vec4 color, vec3 center, vec3 trCorner )
{
    rayPoint = rayPoint - center;
    vec3 q = abs(rayPoint ) + smoothing - trCorner;
    Point box = Point(length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0), color);
    box.dist -= smoothing;
    return box;
}

/*
SDF of a pyramid, defines the pyrimd based off the rayPoint
Modified from https://iquilezles.org/articles/distfunctions/
Modifications: allow for x,y,z offsets and mirroring across xz plane, smoothing, color
@param height, height of pyrmid
@param width, width of pyrmid
@param orientation, either 1.0 for upright or -1.0 for upside down
@param xOffset, the offset from center on x axis
@param yOffset, the offset from center on y axis
@param zOffset, the offset from center on z axis
@param rayPoint, the ray point being evaluated for distance
*/
Point sdfPyramid(float height, float width, float orientation, float xOffset, float yOffset, float zOffset, float smoothing, vec4 color, vec3 rayPoint)
{
    Point planeish = sdfBox(rayPoint, smoothing, color, vec3(xOffset, yOffset, zOffset), vec3(width/2.0, 0.0035, width/2.0));


    float m2 = height*height + 0.25;
    //My modifications are in the following block
    rayPoint.x = abs(rayPoint.x - xOffset) + smoothing; // add abs(y) for both top and bottom of pyrimid
    rayPoint.z = abs(rayPoint.z - zOffset) + smoothing; // add abs(y) for both top and bottom of pyrimid
    rayPoint.y = orientation * (rayPoint.y - yOffset) + smoothing; //Adjusts y based on offset and orientation
    rayPoint.xz = (rayPoint.z>rayPoint.x) ? rayPoint.zx : rayPoint.xz;
    rayPoint.xz -= width/2.0; //Scales by halfwidth

    //Unmodified from original
    vec3 q = vec3( rayPoint.z, height*rayPoint.y - 0.5*rayPoint.x, height*rayPoint.x + 0.5*rayPoint.y);

    float s = max(-q.x,0.0);
    float t = clamp( (q.y-0.5*rayPoint.z)/(m2+0.25), 0.0, 1.0 );

    float a = m2*(q.x+s)*(q.x+s) + q.y*q.y;
    float b = m2*(q.x+0.5*t)*(q.x+0.5*t) + (q.y-m2*t)*(q.y-m2*t);

    float d2 = min(q.y,-q.x*m2-q.y*0.5) > 0.0 ? 0.0 : min(a,b);


    Point pyrimid =  Point(sqrt( (d2+q.z*q.z)/m2 ) * sign(max(q.z,-rayPoint.y)), color);
    pyrimid.dist -= smoothing;
    return minWithColor(pyrimid, planeish);

}

/*
SDF of an upside down triangluar prism
Modified from https://www.shadertoy.com/view/Nld3DB
Modifications to allow for x,y,z offsets, mirroring across y axis and rotating about y axis and smoothing, color
@param width, width of prism
@param height, height of prism
@param orientation, either 1.0 for upright or -1.0 for upside down
@param xOffset, the offset from center on x axis
@param yOffset, the offset from center on y axis
@param zOffset, the offset from center on z axis
@param rayPoint, the ray point being evaluated for distance
*/
Point sdfPrism(float width, float height, float depth, int orientation, float xOffset, float yOffset, float zOffset, float smoothing, vec4 color, vec3 rayPoint) {

    rayPoint.x = rayPoint.x - xOffset; //adjust x
    rayPoint.z = rayPoint.z  - zOffset; //adjust z
    rayPoint.y = -rayPoint.y + yOffset; //adjust y

    //Flip axises for left-right prism - my addition
    orientation < 0 ? rayPoint.xz = rayPoint.zx: rayPoint.xz;

    rayPoint.x = abs(rayPoint.x);

    rayPoint.x = rayPoint.x + smoothing; //adjust x
    rayPoint.z = rayPoint.z + smoothing; //adjust z
    rayPoint.y = rayPoint.y + smoothing; //adjust y

    //Unmodified from original sdf
    rayPoint.xy -= vec2(width/2.0 , -height/2.0);
    vec2 e = vec2(-width/2.0, height/2.0);
    vec2 q = rayPoint.xy - e * clamp(dot(rayPoint.xy, e) / dot(e, e), 0.0, 1.0);
    float d1 = length(q);
    if (max(q.x, q.y) < 0.0) {
        d1 = -min(d1, rayPoint.y);
    }
    float d2 = abs(rayPoint.z) - depth/2.0;
    Point prism = Point(length(max(vec2(d1, d2), 0.0)) + min(max(d1, d2), 0.0), color);
    prism.dist -= smoothing;
    return prism;
}

/*
Approximation of a Serpenski's Pyramids at variying levels of fractlization
Factal is built using recursive (not written recursivly) sdfs for a pyrimid and two upside down trianglar prisims at
a right angle to eachother to form a cross
With this method I can only go to 4 levels of fractlization before it slows the shader down too much
*/

/*
1st level fractilzation of a Serpinski's pyrimd
Fully original code
@param size, size of base pyrimid
@param xOffset, the offset from center on x axis
@param yOffset, the offset from center on y axis
@param zOffset, the offset from center on z axis
@param rayPoint, the ray point being evaluated for distance
*/
Point fracalize_lvl1(float size, float xOffset, float yOffset, float zOffset, vec4 color, vec3 rayPoint){
    Point pyrmid = sdfPyramid(size, size, size, xOffset, yOffset, zOffset, 0.0, color, rayPoint);
    Point prism4 = sdfPrism(size/2.0, size, size, 1, xOffset, yOffset, zOffset, 0.0, color, rayPoint);
    Point prism5 = sdfPrism(size/2.0, size, size, -1, xOffset, yOffset, zOffset, 0.0, color, rayPoint);
    float prisms = min(prism4.dist, prism5.dist);

    return Point(max(pyrmid.dist, - prisms), color);

}

/*
2nd level fractilzation of a Serpinski's pyrimd
Fully original code
@param size, scaling factor
@param xOffset, the offset from center on x axis
@param yOffset, the offset from center on y axis
@param zOffset, the offset from center on z axis
@param rayPoint, the ray point being evaluated for distance
*/
Point fracalize_lvl2(float size, float xOffset, float yOffset, float zOffset, vec4 color, vec3 rayPoint){

    Point lv2_1 = fracalize_lvl1(1.0/size, xOffset + (0.00)/size, yOffset + (1.00)/size, zOffset + (0.00)/size, color, rayPoint); //top
    Point lv2_2 = fracalize_lvl1(1.0/size, xOffset + (0.50)/size, yOffset + (0.00)/size, zOffset - (0.50)/size, color, rayPoint); //Front right
    Point lv2_3 = fracalize_lvl1(1.0/size, xOffset - (0.50)/size, yOffset + (0.00)/size, zOffset - (0.50)/size, color, rayPoint); //Front left
    Point lv2_4 = fracalize_lvl1(1.0/size, xOffset + (0.50)/size, yOffset + (0.00)/size, zOffset + (0.50)/size, color, rayPoint); //back right
    Point lv2_5 = fracalize_lvl1(1.0/size, xOffset - (0.50)/size, yOffset + (0.00)/size, zOffset + (0.50)/size, color, rayPoint); //back left
    float com_lv2_1 = min(lv2_1.dist, lv2_2.dist);
    float com_lv2_2 = min(lv2_3.dist, lv2_4.dist);
    float com_lv2_3 = min(lv2_5.dist, com_lv2_1);
    float com_lv2_4 = min(com_lv2_2, com_lv2_3);

    return Point(com_lv2_4, color);
}
/*
3rd level fractilzation of a Serpinski's pyrimd
Fully original code
@param size, scaling factor
@param xOffset, the offset from center on x axis
@param yOffset, the offset from center on y axis
@param zOffset, the offset from center on z axis
@param rayPoint, the ray point being evaluated for distance
*/
Point fracalize_lvl3(float scale, float xOffset, float yOffset, float zOffset, vec4 color, vec3 rayPoint){

    Point lv2_1 = fracalize_lvl2(4.0 * scale, xOffset + 0.00/scale, yOffset + 0.50/scale, zOffset + 0.00/scale, color, rayPoint);
    Point lv2_2 = fracalize_lvl2(4.0 * scale, xOffset + 0.25/scale, yOffset + 0.00/scale, zOffset + 0.25/scale, color, rayPoint);
    Point lv2_3 = fracalize_lvl2(4.0 * scale, xOffset - 0.25/scale, yOffset + 0.00/scale, zOffset + 0.25/scale, color, rayPoint);
    Point lv2_4 = fracalize_lvl2(4.0 * scale, xOffset + 0.25/scale, yOffset + 0.00/scale, zOffset - 0.25/scale, color, rayPoint);
    Point lv2_5 = fracalize_lvl2(4.0 * scale, xOffset - 0.25/scale, yOffset + 0.00/scale, zOffset - 0.25/scale, color, rayPoint);

    float com_lv2_1 = min(lv2_1.dist, lv2_2.dist);
    float com_lv2_2 = min(lv2_3.dist, lv2_4.dist);
    float com_lv2_3 = min(lv2_5.dist, com_lv2_1);
    float com_lv2_4 = min(com_lv2_2, com_lv2_3);

    return Point(com_lv2_4, color);
}

//Returns the distance from a rayPoint to the closest object and the color of that object
//Uses object data passed form the c code to determine which objects to render and how to render them
//Fully original code
//Ideas of how to combine objects taken from sources in Readme
Point getDist(vec3 rayPoint){

    //Base point to add all other shapes to
    Point total = sdfBox(rayPoint, 1.0, vec4(1.0, 1.0, 1.0, 1.0), vec3(0.0, 0.0,0.0), vec3(0.05, 0.05, 0.05));

    //Sphere, simplemin, Constructive
    if(u_sphere_data.x >= 0.5){
        Point sphere = sdfSphere(vec4(u_sphere_loc.xyz, u_sphere_data.z), vec4(1.0, 0.0, 0.0, 1.0), rayPoint);
        total = minWithColor(total, sphere);
    }

    //Box Smoothmin Constructive
    if(u_box_data.x >= 0.5 && u_combine_method.x >= 0.5 && u_box_data.w < 0.5){
        Point box = sdfBox(rayPoint, u_box_data.y, vec4(0.0, 0.30, 1.0, 1.0), u_box_loc.xyz, vec3(u_box_data.z, u_box_data.z, u_box_data.z));
        total = smoothMin(total, box, u_combine_method.y);
    }

    //Box simple min constructive
    if(u_box_data.x >= 0.5 && u_combine_method.x < 0.5 && u_box_data.w < 0.5){
        Point box = sdfBox(rayPoint, u_box_data.y, vec4(0.0, 0.30, 1.0, 1.0), u_box_loc.xyz, vec3(u_box_data.z, u_box_data.z, u_box_data.z));
        total = minWithColor(total, box);
    }

    //Box simple min destructive
    if(u_box_data.x >= 0.5 && u_combine_method.x < 0.5 && u_box_data.w >= 0.5){
        Point box = sdfBox(rayPoint, u_box_data.y, vec4(0.0, 0.0, 1.0, 1.0), u_box_loc.xyz, vec3(u_box_data.z, u_box_data.z, u_box_data.z));
        total = destructiveWithColor(total, box);
    }

    //Pyrmid smooth min constructive
    if(u_pyrmid_data.x >= 0.5 && u_combine_method.x >= 0.5){
        Point pyrmid = sdfPyramid(u_pyrmid_data.z, u_pyrmid_data.z, u_pyrmid_data.z, u_pyrmid_loc.x, u_pyrmid_loc.y, u_pyrmid_loc.z, u_pyrmid_data.y, vec4(1.0, 1.0, 0.0, 1.0), rayPoint);
        total = smoothMin(total, pyrmid, u_combine_method.y);
    }

    //Pyrmid simple min constructive
    if(u_pyrmid_data.x >= 0.5 && u_combine_method.x < 0.5){
        Point pyrmid = sdfPyramid(u_pyrmid_data.z, u_pyrmid_data.z, 1.0, u_pyrmid_loc.x, u_pyrmid_loc.y, u_pyrmid_loc.z, u_pyrmid_data.y, vec4(1.0, 1.0, 0.0, 1.0), rayPoint);
        total = minWithColor(total, pyrmid);
    }

    //Prism smooth min constructive
    if(u_prism_data.x >= 0.5 && u_combine_method.x >= 0.5){
        Point prism = sdfPrism(u_prism_data.z, u_prism_data.z, u_prism_data.z, 1, u_prism_loc.x, u_prism_loc.y, u_prism_loc.z, u_prism_data.y, vec4(0.0, 1.0, 1.0, 1.0), rayPoint);
        total = smoothMin(total, prism, u_combine_method.y);
    }

    //Prism simple min constructive
    if(u_prism_data.x >= 0.5 && u_combine_method.x < 0.5){
        Point prism = sdfPrism(u_prism_data.z, u_prism_data.z, u_prism_data.z, 1, u_prism_loc.x, u_prism_loc.y, u_prism_loc.z, u_prism_data.y, vec4(0.0, 1.0, 1.0, 1.0), rayPoint);
        total = minWithColor(total, prism);
    }

    vec4 color = vec4(0.0, 1.0, 0.0, 1.0);

    //Fractal, smooth min, constructive
    if(u_stri_data.x >= 0.5 && u_combine_method.x >= 0.5){
        Point fractal;
        //Different fractal levels
        if (u_fractal_evel == 1) {
            fractal = sdfPyramid(size, size, size, u_stri_loc.x, u_stri_loc.y, u_stri_loc.z, 0.0, color, rayPoint);
        } else if (u_fractal_evel == 2) {
            fractal = fracalize_lvl1(size, u_stri_loc.x, u_stri_loc.y, u_stri_loc.z, color, rayPoint);
        } else if (u_fractal_evel == 3) {
            fractal = fracalize_lvl2(2.0, u_stri_loc.x, u_stri_loc.y, u_stri_loc.z, color, rayPoint);
        } else if (u_fractal_evel == 4) {
            fractal = fracalize_lvl3(1.0, u_stri_loc.x, u_stri_loc.y, u_stri_loc.z, color, rayPoint);
        }
        total = smoothMin(total, fractal, u_combine_method.y);
    }

    //Fractal, simple min constructive
    if(u_stri_data.x >= 0.5 && u_combine_method.x < 0.5){
        Point fractal;
        //Different fractal levels
        if (u_fractal_evel == 1) {
            fractal = sdfPyramid(size, size, size, u_stri_loc.x, u_stri_loc.y, u_stri_loc.z, 0.0, color, rayPoint);
        } else if (u_fractal_evel == 2) {
            fractal = fracalize_lvl1(size, u_stri_loc.x, u_stri_loc.y, u_stri_loc.z, color, rayPoint);
        } else if (u_fractal_evel == 3) {
            fractal = fracalize_lvl2(2.0, u_stri_loc.x, u_stri_loc.y, u_stri_loc.z, color, rayPoint);
        } else if (u_fractal_evel == 4) {
            fractal = fracalize_lvl3(1.0, u_stri_loc.x, u_stri_loc.y, u_stri_loc.z, color, rayPoint);
        }
        total = minWithColor(total, fractal);
    }
    //Returns the distance to the closest object and the color to render it as
    return total;
}

//Gets the lighting for every point in the scene and calculates normals for each pixel that contains an object
//Modified from https://timcoster.com/2020/02/11/raymarching-shader-pt1-glsl/
//Modifications: allow for light position to be moved and not move with time, combine lighting and normal functions, refactor, color
Point getLight(vec3 eye, vec3 rayDirection, float distToClosestObject)
{
    vec3 rayPoint = eye + rayDirection * distToClosestObject;

    //get normals
    Point point = getDist(rayPoint);
    vec2 eps = vec2(EPSILON,0);
    //take the difference between the distance to the point and a tiny dist away in each xyz direction
    vec3 normal = point.dist - vec3(
        getDist(rayPoint-eps.xyy).dist,
        getDist(rayPoint-eps.yxy).dist,
        getDist(rayPoint-eps.yyx).dist);
    vec3 normalVec = normalize(normal);

    // Light
    vec3 lightPos = eye + u_light_pos; // Light Position
    vec3 lightVec = normalize(lightPos-rayPoint); // Vector to Light


    float dif = dot(normalVec,lightVec); // get where light vector and normal vector aligh
    point.dist = clamp(dif,0.0,1.0);

    return point; //Enforce bounds
}

//Marches a ray from the eye to a location on the screen and returns the distance to the first collision
//Modified from https://timcoster.com/2020/02/11/raymarching-shader-pt1-glsl/
//Modifications: refactor, commenting and slight simplification, allows for the handling of colors
float rayMarch(vec3 eye, vec3 uv){
    float rayLen = 0.0; //Current length of exploring ray

    //Loops until one of the following exit conditions it met: too many steps, hit back of scene, hit object
    for(int i=0;i<MAX_STEPS;i++){

        //Maxmium known safe distance along the vector towards uv found in last iteration
        vec3 rayPoint = eye + (uv * rayLen);

        //Distance to the closest object to the point, gives us max safe travel dist
        float distToClosestObject = getDist(rayPoint).dist;

        //Increase our rayLength by the known safe distance
        rayLen += distToClosestObject;

        //Exit if at scene depth or close enough to object
        if(rayLen > SCENE_DEPTH || distToClosestObject < EPSILON) break;
    }

    //Returns the length of an array along the vector pointing to uv before the first collision is met or SCENE_DEPTH
    return rayLen;
}

//Caluclate the lookAt matrix that allows for camera movement
//From slides with modification as it appears the slides say F and U should be normalized but in my code I had to flip
//them to F and S
mat3 lookAt(vec3 eye, vec3 focusPoint, vec3 upDir){
    vec3 F = normalize(focusPoint - eye);
    vec3 S = normalize(cross(F, upDir));
    vec3 U = cross(S, F);
    return mat3(S, U, F);
}

//Main Function
//Modified from https://timcoster.com/2020/02/11/raymarching-shader-pt1-glsl/
//Modifications: Moving camera location, color, removing background pixels, simplified lighting and normals
void main(){
    //Scale screen coords to be [-1, 1]
    vec2 uv = (gl_FragCoord.xy-0.5*u_resolution.xy)/u_resolution.y;
    //Eye position and origin of the rays
    vec3 eye = vec3(u_eyePos.xyz);
    vec3 lookat = vec3(0.0, 0.0,0.0);
    vec3 up = vec3(0.0,u_eyePos.w,0.0);
    //Vector from eye to screen coordiate
    mat3 rayDirectionMatrix = lookAt(eye, lookat, up);
    vec3 rayDirection = rayDirectionMatrix * normalize(vec3(uv.xy, 1.0));
    //Make it a unit vector
    rayDirection = normalize(rayDirection);
    //Distance to first collision
    float distToClosestObject = rayMarch(eye, rayDirection);

    //Light
    Point light = getLight(eye, rayDirection, distToClosestObject); // Diffuse lighting
    vec4 color = vec4(light.dist * light.color);

    //Render the pixel with coloring according to it's distance and ignore anything at the back of the scene
    if(distToClosestObject >= SCENE_DEPTH){
        discard;
    } else {
        gl_FragColor = color;

    }
}