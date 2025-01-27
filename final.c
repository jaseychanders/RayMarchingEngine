//Final Project - Jasey Chanders
/*
Builds a basic ray marching rendering engine that can display some basic shapes using sdfs and an approximation of
a Sierpinskies triangle. Allows for controls like smoothness, destructive interference and smooth combining.
The majority of this file is controls. This file does not render any objects save a box the size of the screen to
give the frag shader pixels to work with.
Complete list of references in README
Jasey Chanders
*/

#include "CSCIx229.h" //Using the provided libraries

double asp=1;//  Aspect ratio
double dim=1; //Dimentions
double screenWidth=800;
double screenHeight=500;
//Shader I am using
int shader = 0;
//Camera location
double xEye=0.0, yEye=0.0, zEye=0.0, up = 0.0;
//View angles
int th = 0;
int ph = 15;
//Fractal levels
int fractalLevel = 4;
//Location of the light
float xLight=0.0, yLight=0.0, zLight=0.0;
//Shape selected for editing
int selectedShape = 2;
//Simple combining or smooth min
float combineMethod = 0.0;
float smoothkFactor = 25.0;
//Data for each shape
float shapeData[5][2][4] =
    //x, y, z, null,   viz, smoothness, size, interference
    {{{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 0.3, 0.0}}, //sphere
      {{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 0.2, 0.0}}, //box
      {{0.0, -0.50, 0.0, 0.0},  {1.0, 0.0, 1.0, 0.0}}, //s triangle
      {{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 0.3, 0.0}}, //pyrmid
      {{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 0.3, 0.0}}, // prism
};

//Displays a blank box and all the control text
//Basic functionality from class examples
void display(){

   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glLoadIdentity();

   //Calculate the eye position based on view angles
   //From class examples
   xEye = -2*dim*Sin(th)*Cos(ph);
   yEye = 2*dim*Sin(ph);
   zEye = 2*dim*Cos(th)*Cos(ph);
   up = Cos(ph);

   //Pass data to frag shader
   int resolution = glGetUniformLocation(shader, "u_resolution");
   int eyePos = glGetUniformLocation(shader, "u_eyePos");
   int u_fractal_level = glGetUniformLocation(shader, "u_fractal_evel");
   int u_light_pos = glGetUniformLocation(shader, "u_light_pos");
   int u_sphere_data = glGetUniformLocation(shader, "u_sphere_data");
   int u_sphere_loc = glGetUniformLocation(shader, "u_sphere_loc");
   int u_box_data = glGetUniformLocation(shader, "u_box_data");
   int u_box_loc = glGetUniformLocation(shader, "u_box_loc");
   int u_stri_data = glGetUniformLocation(shader, "u_stri_data");
   int u_stri_loc = glGetUniformLocation(shader, "u_stri_loc");
   int u_pyrmid_data = glGetUniformLocation(shader, "u_pyrmid_data");
   int u_pyrmid_loc = glGetUniformLocation(shader, "u_pyrmid_loc");
   int u_prism_data = glGetUniformLocation(shader, "u_prism_data");
   int u_prism_loc = glGetUniformLocation(shader, "u_prism_loc");
   int u_combine_method = glGetUniformLocation(shader, "u_combine_method");

   glUseProgram(shader);
   glUniform2f(resolution,screenWidth,screenHeight);
   glUniform4f(eyePos,xEye,yEye,zEye, up);
   glUniform1i(u_fractal_level,fractalLevel);
   glUniform3f(u_light_pos, xLight, yLight, zLight);
   glUniform4f(u_sphere_loc, shapeData[0][0][0], shapeData[0][0][1], shapeData[0][0][2], shapeData[0][0][3]);
   glUniform4f(u_sphere_data, shapeData[0][1][0], shapeData[0][1][1], shapeData[0][1][2], shapeData[0][1][3]);

   glUniform4f(u_box_loc, shapeData[1][0][0], shapeData[1][0][1], shapeData[1][0][2], shapeData[1][0][3]);
   glUniform4f(u_box_data, shapeData[1][1][0], shapeData[1][1][1], shapeData[1][1][2], shapeData[1][1][3]);

   glUniform4f(u_stri_loc, shapeData[2][0][0], shapeData[2][0][1], shapeData[2][0][2],  shapeData[2][0][3]);
   glUniform4f(u_stri_data, shapeData[2][1][0], shapeData[2][1][1], shapeData[2][1][2], shapeData[2][1][3]);

   glUniform4f(u_pyrmid_loc, shapeData[3][0][0], shapeData[3][0][1], shapeData[3][0][2], shapeData[3][0][3]);
   glUniform4f(u_pyrmid_data, shapeData[3][1][0], shapeData[3][1][1], shapeData[3][1][2], shapeData[3][1][3]);

   glUniform4f(u_prism_loc, shapeData[4][0][0], shapeData[4][0][1], shapeData[4][0][2], shapeData[4][0][3]);
   glUniform4f(u_prism_data, shapeData[4][1][0], shapeData[4][1][1], shapeData[4][1][2], shapeData[4][1][3]);

   glUniform2f(u_combine_method, combineMethod, smoothkFactor);

   glLoadIdentity();

    // draw a basic square so the frag shader has pixels to manipluate
    glBegin(GL_QUADS);
    glVertex3d(-1.0, -1.0, 0.0);
    glVertex3d(-1.0, 1.0, 0.0);
    glVertex3d(1.0, 1.0, 0.0);
    glVertex3d(1.0, -1.0, 0.0);
    glEnd();
   glUseProgram(0); //stop using the shader

   //Draw all the control text overlayed on the screen
   glColor3f(0.7, 0.7, 0.7);

   glColor3f(1.0, 1.0, 0.0);
   glWindowPos2i(screenWidth - 220, 40);
   Print("arrow keys to change camera position");

   glColor3f(0.0, 1.0, 1.0);
   glWindowPos2i(screenWidth/2.0 -90, screenHeight - 20);
   Print("Custom Ray March Rendering Engine");

   glColor3f(1.0, 1.0, 0.0);
   glWindowPos2i(5, 40);
   Print("Combine Method: %s  :  x: toggle ", combineMethod==0.0?"simple min":"smooth min");

   glColor3f(1.0, 1.0, 1.0);

   if(combineMethod== 1.0){
      glWindowPos2i(5, 20);
      Print("<-l Smoothness Factor : %.2f  p->", smoothkFactor);
   }

   //shapes
   glColor3f(1.0, 1.0, 0.0);
   glWindowPos2i(5, screenHeight -20);
   Print("Use 'n' to select a shape to edit");

   glColor3f(1.0, 1.0, 1.0);
   if(selectedShape == 0) glColor3f(0.25, 1.0, 1.0);
   glWindowPos2i(5, screenHeight -50);
   Print("Sphere");
   glColor3f(1.0, 1.0, 1.0);

   if(selectedShape == 1) glColor3f(0.25, 1.0, 1.0);
   glWindowPos2i(5, screenHeight -70);
   Print("Rectangle");
   glColor3f(1.0, 1.0, 1.0);

   if(selectedShape == 2) glColor3f(0.25, 1.0, 1.0);
   glWindowPos2i(5, screenHeight -90);
   Print("Sierpinski's Triangle");
   glColor3f(1.0, 1.0, 1.0);

   if(selectedShape == 3) glColor3f(0.25, 1.0, 1.0);
   glWindowPos2i(5, screenHeight -110);
   Print("Pyrmid");
   glColor3f(1.0, 1.0, 1.0);

   if(selectedShape == 4) glColor3f(0.25, 1.0, 1.0);
   glWindowPos2i(5, screenHeight -130);
   Print("prism");
   glColor3f(1.0, 1.0, 1.0);

   //Hot keys
   glColor3f(1.0, 1.0, 0.0);
   glWindowPos2i(5, screenHeight -180);
   Print("Hot Keys");

   glColor3f(1.0, 1.0, 1.0);
   glWindowPos2i(5, screenHeight -200);
   Print("0: Reset Camera");

   glWindowPos2i(5, screenHeight -220);
   Print("1: Object Addition");

   glWindowPos2i(5, screenHeight -240);
   Print("2: Smooth Addition");

   glWindowPos2i(5, screenHeight -260);
   Print("3: Smoothness");

   glWindowPos2i(5, screenHeight -280);
   Print("4: Distructive Addition");

   glWindowPos2i(5, screenHeight -300);
   Print("5: Smooth Addition + fractal");


   glWindowPos2i(5, screenHeight -320);
   Print("5: Christmas Tree - Kinda");


   //Properties
   glColor3f(1.0, 1.0, 0.0);
   glWindowPos2i(screenWidth - 120, screenHeight -20);
   Print("Object Properties");
   glColor3f(1.0, 1.0, 1.0);

   glWindowPos2i(screenWidth - 100, screenHeight -50);
   Print("Visable");

   glWindowPos2i(screenWidth - 120, screenHeight -65);
   Print("v:toggle  %s", shapeData[selectedShape][1][0] == 1.0? "Visible" : "Hidden");

   glWindowPos2i(screenWidth - 80, screenHeight -85);
   Print("X");

   glWindowPos2i(screenWidth - 115, screenHeight -100);
   Print("<-a  %.2f  d->", shapeData[selectedShape][0][0]);


   glWindowPos2i(screenWidth - 80, screenHeight -120);
   Print("Y");

   glWindowPos2i(screenWidth - 115, screenHeight -135);
   Print("<-w  %.2f  s->", shapeData[selectedShape][0][1]);

   glWindowPos2i(screenWidth - 80, screenHeight -155);
   Print("Z");

   glWindowPos2i(screenWidth - 115, screenHeight -170);
   Print("<-e  %.2f  z->", shapeData[selectedShape][0][2]);

   if(selectedShape == 1){
      glWindowPos2i(screenWidth - 100, screenHeight -280);
      Print("Interference");

      glWindowPos2i(screenWidth - 135, screenHeight -295);
      Print("%s :  r: toggle", shapeData[selectedShape][1][3]==0.0? "Constructive": "Destructive");
   }

   if(selectedShape == 2){
      glWindowPos2i(screenWidth - 120, screenHeight -205);
      Print("Fractal Level");

      glWindowPos2i(screenWidth - 105, screenHeight -220);
      Print("%d:  m: toggle", fractalLevel);
   }

   if(selectedShape != 2){
      glWindowPos2i(screenWidth - 80, screenHeight -205);
      Print("Size");

      glWindowPos2i(screenWidth - 95, screenHeight -220);
      Print("-  %.2f  +", shapeData[selectedShape][1][2]);
   }

   if(selectedShape != 0 && selectedShape != 2){
      glWindowPos2i(screenWidth - 105, screenHeight -245);
      Print("Smoothness");

      glWindowPos2i(screenWidth - 115, screenHeight -260);
      Print("<-g  %.2f  t->", shapeData[selectedShape][1][1]);
   }

   ErrCheck("display");
   glFlush();
   glutSwapBuffers();


}

/*
 *  Read text file
 *  From class examples
 */
char* ReadText(char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 *  From class examples
 */
void PrintShaderLog(int obj,char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

/*
 *  Print Program Log
 *  From class examples
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}


/*
 *  Create Shader
 *  From class examples
 */
int CreateShader(GLenum type,char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 *  From class examples
 */
int CreateShaderProg(char* VertFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

//Arrow key controls
//From class examples
void special(int key,int x,int y){
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP){
      ph += 5;
      if (ph > 90){
          ph = 90;
      }
   }
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN){
         ph -= 5;
      if (ph < -90){
         ph = -90;
      }
   }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;

   Project(0,asp,dim);
   glutPostRedisplay();
}

//Keyboard controls
void key(unsigned char ch,int x,int y){
  //Switch mode
  if (ch == 27) exit(0);
  else if (ch == 'm'){
      fractalLevel += 1;
      if (fractalLevel >4)
        fractalLevel = 1;
  } else if (ch == 'w'){ //move object +y
      shapeData[selectedShape][0][1] += 0.1;
      if (shapeData[selectedShape][0][1] >= 1.0) shapeData[selectedShape][0][1] = 1.0;
  } else if (ch == 's'){ //move object -y
        shapeData[selectedShape][0][1] -= 0.1;
        if (shapeData[selectedShape][0][1] <= -1.0) shapeData[selectedShape][0][1] = -1.0;
  } else if (ch == 'a'){ //move object -x
        shapeData[selectedShape][0][0] -= 0.1;
        if (shapeData[selectedShape][0][0] <= -1.0) shapeData[selectedShape][0][0] = -1.0;
  } else if (ch == 'd'){ //move object +x
        shapeData[selectedShape][0][0] += 0.1;
        if (shapeData[selectedShape][0][0] >= 1.0) shapeData[selectedShape][0][0] = 1.0;
  } else if (ch == 'z'){ //move object +z
        shapeData[selectedShape][0][2] += 0.1;
        if (shapeData[selectedShape][0][2] >= 1.0) shapeData[selectedShape][0][2] = 1.0;
  } else if (ch == 'e'){ //move object -z
        shapeData[selectedShape][0][2] -= 0.1;
        if (shapeData[selectedShape][0][2] <= -1.0) shapeData[selectedShape][0][2] = -1.0;
  }else if (ch == 'g'){ //decrease smoothing
     shapeData[selectedShape][1][1] -= 0.01;
     if (shapeData[selectedShape][1][1] <= 0.0) shapeData[selectedShape][1][1] = 0.0;
  } else if (ch == 't'){ //incrase smoothing
     shapeData[selectedShape][1][1] += 0.01;
     if (shapeData[selectedShape][1][1] >= 0.1) shapeData[selectedShape][1][1] = 0.1;
  }else if (ch == 'n'){ //Select a shape
     selectedShape +=   1;
     if(selectedShape > 4){
        selectedShape = 0;
     }
  }else if (ch == 'v'){ //Toggle visiblity of a shape
     shapeData[selectedShape][1][0] += 1.0;
     if(shapeData[selectedShape][1][0] > 1.0){shapeData[selectedShape][1][0] = 0.0;}
  } else if (ch == 'x'){ //change render method
     combineMethod +=  1.0;
     if(combineMethod > 1.0){combineMethod = 0.0;}
     if(combineMethod == 1.0){
        shapeData[0][1][3] = 0.0;
        shapeData[1][1][3] = 0.0;
        shapeData[2][1][3] = 0.0;
        shapeData[3][1][3] = 0.0;
        shapeData[4][1][3] = 0.0;
     }
  }else if (ch == '+' || ch == '='){ //Increase size of shape
     shapeData[selectedShape][1][2] += 0.1;
     if(shapeData[selectedShape][1][2] > 1.0){shapeData[selectedShape][1][2] = 1.0;}
  }else if (ch == '-'){ //Decrease size of shape
     shapeData[selectedShape][1][2] -= 0.1;
     if(shapeData[selectedShape][1][2] < 0.0){shapeData[selectedShape][1][2] = 0.0;}
  }else if (ch == 'l'){ //Increase k factor which decreases smoothmin effect
     smoothkFactor +=  1.0;
     if(smoothkFactor > 50.0){smoothkFactor = 50.0;}
  }else if (ch == 'p'){ //Decrease k factor which increases smoothmin effect
     smoothkFactor -=  1.0;
     if(smoothkFactor < 3.0){smoothkFactor = 3.0;}
  }else if (ch == 'r'){ //Change shape to destructive interference
    if(combineMethod == 0.0){
       shapeData[selectedShape][1][3] += 1.0;
       if(shapeData[selectedShape][1][3] > 1.0){shapeData[selectedShape][1][3] = 0.0;}
    }
    //Hot keys
  }else if (ch == '0'){ //reset view
     th = 0;
     ph = 0;
  }else if (ch == '5'){ //Melting triangle

     //x, y, z, null  viz, smoothness, size, interference
     float newData[5][2][4] = {
        {{0.0, 0.8, 0.0, 0.0},  {1.0, 0.0, 0.3, 0.0}}, //sphere
        {{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 0.0, 0.0}}, //box
        {{0.0, -0.20, 0.0, 0.0},  {1.0, 0.0, 1.0, 0.0}}, //s triangle
        {{0.0, 0.00, 0.0, 0.0},  {0.0, 0.0, 0.8, 0.0}}, //pyrmid
        {{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 0.8, 0.0}}, // prism
        };

     memcpy(shapeData, newData, sizeof(newData));

     combineMethod = 1.0;
     smoothkFactor = 25.0;
     selectedShape = 0;
  }else if (ch == '4'){ //Destructive interference

     //x, y, z, null  viz, smoothness, size, interference
     float newData[5][2][4] = {
        {{0.0, 0.0, 0.0, 0.0},  {1.0, 0.0, 0.3, 0.0}}, //sphere
        {{-0.30, -0.0, 0.0, 0.0},  {1.0, 0.0, 0.2, 0.0}}, //box
        {{0.0, -0.50, 0.0, 0.0},  {0.0, 0.0, 1.0, 0.0}}, //s triangle
        {{0.0, 0.00, 0.0, 0.0},  {0.0, 0.0, 0.8, 0.0}}, //pyrmid
        {{0.0, 0.20, 0.0, 0.0},  {0.0, 0.0, 0.8, 0.0}}, // prism
        };

     memcpy(shapeData, newData, sizeof(newData));

     combineMethod = 0.0;
     shapeData[1][1][3] = 1.0;
     selectedShape = 1;

     ph = 0;
     th = 30;
  }else if (ch == '2'){ //Smooth min

     //x, y, z, null  viz, smoothness, size, interference
     float newData[5][2][4] = {
        {{0.20, -0.30, 0.30, 0.0},  {0.0, 0.0, 0.1, 0.0}}, //sphere
        {{-0.20, -0.30, 0.30, 0.0},  {0.0, 0.0, 0.1, 0.0}}, //box
        {{0.0, -0.50, 0.0, 0.0},  {0.0, 0.0, 1.0, 0.0}}, //s triangle
        {{0.0, 0.00, 0.0, 0.0},  {1.0, 0.0, 0.8, 0.0}}, //pyrmid
        {{0.0, 0.20, 0.0, 0.0},  {1.0, 0.0, 0.8, 0.0}}, // prism
        };

     memcpy(shapeData, newData, sizeof(newData));

     combineMethod = 1.0;
     smoothkFactor = 25.0;
     selectedShape = 3;

     ph = 0;
     th = 110;
  }else if (ch == '6'){ //Christmas Tree

     //x, y, z, null  viz, smoothness, size, interference
     float newData[5][2][4] = {
       {{0.20, -0.30, 0.30, 0.0},  {1.0, 0.0, 0.1, 0.0}}, //sphere
       {{0.0, -0.60, 0.00, 0.0},  {1.0, 0.0, 0.1, 0.0}}, //box
       {{0.0, -0.50, 0.0, 0.0},  {1.0, 0.0, 1.0, 0.0}}, //s triangle
       {{0.0, 0.50, 0.0, 0.0},  {1.0, 0.0, 0.4, 0.0}}, //pyrmid
       {{0.0, 0.40, 0.0, 0.0},  {1.0, 0.0, 0.4, 0.0}}, // prism
       };

     memcpy(shapeData, newData, sizeof(newData));

     combineMethod = 0.0;
     combineMethod = 0.0;
     ph = 0;
     th = 0;
     selectedShape = 0;
  }else if (ch == '1'){ //Merge shapes

     //x, y, z, null  viz, smoothness, size, interference
     float newData[5][2][4] = {
        {{0.0, 0.0, 0.0, 0.0},  {1.0, 0.0, 0.3, 0.0}}, //sphere
      {{-0.30, 0.0, 0.0, 0.0},  {1.0, 0.0, 0.2, 0.0}}, //box
      {{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 1.0, 0.0}}, //s triangle
      {{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 0.3, 0.0}}, //pyrmid
      {{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 0.3, 0.0}}, // prism
        };

     memcpy(shapeData, newData, sizeof(newData));

     combineMethod = 0.0;
     ph = 10;
     th = 30;
     selectedShape = 1;
  }else if (ch == '3'){ //smoothing box

     //x, y, z, null  viz, smoothness, size, interference
     float newData[5][2][4] = {
        {{0.0, 0.0, 0.0, 0.0},  {0.0, 0.0, 0.1, 0.0}}, //sphere
        {{0.0, 0.0, 0.0, 0.0},  {1.0, 0.06, 0.4, 0.0}}, //box
        {{0.0, -0.50, 0.0, 0.0},  {0.0, 0.0, 1.0, 0.0}}, //s triangle
        {{0.0, 0.00, 0.0, 0.0},  {0.0, 0.0, 0.8, 0.0}}, //pyrmid
        {{0.0, 0.20, 0.0, 0.0},  {0.0, 0.0, 0.8, 0.0}}, // prism
        };

     memcpy(shapeData, newData, sizeof(newData));

     combineMethod = 0.0;
     selectedShape = 1;

     ph = 30;
     th = 30;
  }

   Project(0,asp,dim);
   glutPostRedisplay();
}

//Reshape function from examples
// Modifications: tracking screen size for shader use
void reshape(int width,int height){

   screenWidth = width<800?800:width;
   screenHeight = height<500?500:height;

   asp = (screenHeight>0) ? (double)screenWidth/screenHeight : 1;
   glutReshapeWindow(screenWidth, screenHeight); //From ChatGPT
   //  Set the viewport to the entire window
   glViewport(0,0, RES*screenWidth,RES*screenHeight);
   //  Set projection
   Project(0,asp,dim);

}


//Main from examples
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(screenWidth,screenHeight);
   glutCreateWindow("Final Project - Jasey Chanders");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif

   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   shader = CreateShaderProg("final.vert","final.frag");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}