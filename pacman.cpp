/*
gcc code.c -framework GLUT -framework OpenGL

 Still required additons to the file readin functiopn to make it platform independant	X
Needs changes to the lighting
Need changes to block
new coll detection algorithm X
error checking on map reload X
ghosts through escape routes X
rotate pm X
text to show score
cd with pac and shost so he dies
power pills?
score variable X
help system
demo animation
*/
#define GL_SILENCE_DEPRECATION
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>

GLfloat vertices[][3] = {{1.0,1.0,0.0},{-1.0,1.0,0.0},{-1.0,-1.0,0.0},{1.0,-1.0,0.0},
						{1.0,1.0,2.0},{-1.0,1.0,2.0},{-1.0,-1.0,2.0},{1.0,-1.0,2.0}};
GLfloat color[][3]= {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0},{1.0,1.0,0.0},{1.0,0.0,1.0},
						{0.0,1.0,1.0},{0.0,0.0,0.0},{1.0,1.0,1.0}};

double angle = -360.0;
//pacs position
double pacDestX, pacDestY, pacDestZ,rot,pacCurX,pacCurY,pacCurZ;
//direction varables for pacman
int curdirn=0;
int nextdirn=0;
//varibles for pacmans mouth
double mouthgap = 40;
double gap = 5;
//varible for view
bool isoview = false;
//movment for pacman vars
int step;
int stepmax=4;
double offset=(0.5);
//var to count the number of pills read in and keep track of how many have been eaten
int numPills=0;
int eatenPills=0;
//int to keep trakc of the score obtained so far
int score=0;
//bool to indeicate if we are in the tour
bool tour=false;
int tourCount;
//arrauy of moves for the tour. its a 2d 20x4 array. the 4 vars are posx,posz,current direction and rotation of pm
int tourArr[20][4]={{2,-10,4,270},{2,-8,1,180},{2,-6,1,180},{2,-4,1,180},{2,-2,1,180},{4,-2,4,270},{6,-2,4,270},{8,-2,4,270},{10,-2,4,270},{12,-2,4,270}
				   ,{14,-2,4,270},{},{},{},{},{},{},{},{},{}};

//used to find a good map position
double v1,v2,v3,v4,v5,v6,v7,v8,v9;
//used to find good lighting settings
float l1,l2,l3,l4,l5,l6,l7,l8,l9;
double inc = 0.5;

//timer function stuff
GLfloat timevalue= 0.0;
//contains the map for pacman to navigate
char map[21][21];
//contains the positions of the ghosts
double ghostCurPos[][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
double ghostDestPos[][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
int ghostDirn [4]; 
int ghostStep [4];
//ghost rotation vars
int ghostRot[4];
//varibale to keep ghost speeds, 2 elements as one ghost faster then others
int ghostSpeed[2];

FILE* out;

//function to read in data from map.txt and place it in
//the map variable
//returns 0 if all ok, else -1
int readmap(void){	  
    FILE* file=fopen("map.txt","r");
	char c = ' ';
	if(file==NULL){
		return -1;
	}
	numPills=0;
    for(int i=0;i<21;i++){
		//routine adjustment to make compatible over all platforms
		while(c!='@'){
			c=fgetc(file);
			if(c==EOF){
				//error
				return -1;
			}
		}
        for(int j=0;j<21;j++){
			c = fgetc(file);
            map[i][j]=c;
			if(c==EOF){
				//error occured
				return -1;
			}
			if(c=='.'){
				numPills++;
			}
        }
        //read the crlf from end
        //fgetc(file);
		//fgetc(file);
    }
	eatenPills=0;
	return 0;
}

void printout(){
	//print stuff out
    printf("Pacman stuff\n");
	for(int a =0;a<21;a++){
        for(int b=0;b<21;b++){
            printf("%c",map[a][b]);
        }
        printf("\n");
    }
}

void myInit(void){
	glClearColor(0.0,0.0,0.0,0.0);
	glPointSize(1.0);
}

//draws my cylinders
void cylinder(int maincolor,int topcolor, int bottomcolor,double radius,
								 double height, int skirt, double x, double y, double z){
	double pi = 3.14159;
	double x1,y1,z1,x2,z2,y2,j;
	double stepSize = (2*pi)/90;
	//set the drawcolor
	glColor3fv(color[maincolor]);
		glBegin(GL_QUADS);
			for( j=0;j<=2*pi;j+=stepSize){
				x1 =x+ (radius*cos(j+stepSize));
				z1= z+(radius*sin(j+stepSize));
				y1 = y+(0.5*height);
				
				x2 = x+(radius*cos(j));
				z2 = z+(radius*sin(j));
				if(skirt==1){

				y2 = (y-(0.5*height))+(0.2*sin(7*j));
				}else{
					y2 = y-(0.5*height);
				}
				glVertex3d(x1,y2,z1); 	  	  	  
				glVertex3d(x1,y1,z1);	 	 	 	 
				glVertex3d(x2,y1,z2);
				glVertex3d(x2,y2,z2);	 	 	 	 	 

		}	 	 	 
		glEnd();
		//draw the top of the cylinder
		glBegin(GL_TRIANGLE_FAN);//LINE_STRIP);
			glColor3fv(color[topcolor]);
			for(j=0;j<=pi*2;j+=0.05){
				x1 =x+( radius*cos(j));
				y1 = y+(0.5*height);
				z1 =z+ (radius*sin(j));
				glVertex3d(x1,y1,z1);	  	  	  
			}
			x1 = x+(radius*cos(0));
			y1 = y+(0.5*height);
			z1 = z+(radius*sin(0));
			glVertex3d(x1,y1,z1);	 	 	 
		glEnd();
		//draw bottom of cylinder
		glBegin(GL_TRIANGLE_FAN);//LINE_STRIP);
			glColor3fv(color[bottomcolor]);
			for(j=0;j<=pi*2;j+=0.05){
				x1 = x+(radius*cos(j));
				if(skirt==1){
					y1 = y-(0.5*height)+0.2;
				}else{
					y1 = y-(0.5*height);
				}
				z1 =z+( radius*sin(j));
				glVertex3d(x1,y1,z1);	  	  	  
			}
			x1 =x+( radius*cos(0));
			z1 =z+( radius*sin(0));
			glVertex3d(x1,y1,z1);	 	 	 
		glEnd();
}

void sphere(double radius,double opengap,int maincolor,int endcolor,double x1,double y1, double z1){
	double c = (3.14159)/180.0;
	double x,y,z,theta;
	double endgap=10.0;
	double endpoint=90-endgap;
	glColor3fv(color[maincolor]);
	for(double phi=-endpoint;phi<endpoint;phi+=endgap){
		glBegin(GL_QUAD_STRIP);//QUAD_STRIP);
		glNormal3d(x1,y1,z1);
		for(theta=-180.0+(opengap/2);theta<=180.0-(opengap/2);theta+=10.0){
			y= y1+(radius*(sin(c*theta)*cos(c*phi)));
			z =z1+(radius*(cos(c*theta)*cos(c*phi)));
			x =x1+(radius*sin(c*phi));
			glVertex3d(x,y,z);
			y= y1+(radius*(sin(c*theta)*cos(c*(phi+endgap))));
			z =z1+(radius*(cos(c*theta)*cos(c*(phi+endgap))));
			x =x1+(radius*(sin(c*(phi+endgap))));
			glVertex3d(x,y,z);
		}
		glEnd();
	}
	//draw and point
	glColor3fv(color[endcolor]);
	glBegin(GL_TRIANGLE_FAN);
	
	x=x1+radius;y=y1;z=z1;
	glVertex3d(x,y,z);
	x=x1+(radius*sin(c*endpoint));
	for(theta=-180.0+(opengap/2);theta<=180.0-(opengap/2);theta+=10.0){
		y=y1+(radius*( sin(c*theta)*cos(c*endpoint)));
		z=z1+(radius*( cos(c*theta)*cos(c*endpoint)));
		glVertex3d(x,y,z);
	}
	glEnd();


	glBegin(GL_TRIANGLE_FAN);
	x=x1-radius;y=y1;z=z1;
	glVertex3d(x,y,z);
	
	x=x1+(radius*sin(c*(-endpoint)));
	for(theta=-180.0+(opengap/2);theta<=180.0-(opengap/2);theta+=10.0){
		y=y1+(radius*( sin(c*theta)*cos(c*-endpoint)));
		z=z1+(radius*( cos(c*theta)*cos(c*-endpoint)));
		glVertex3d(x,y,z);
	}
	glEnd();
}

//draws pacmans mouth
void pacmouth(double radius,double opengap,int maincolor,double x1,double y1, double z1){
	double x,y,z;
	double phi,theta;
	double endgap=2.0;
	glBegin(GL_LINES);
		glColor3fv(color[6]);
		glVertex3d(x1+radius,y1,z1);
		glVertex3d(x1-radius,y1,z1);
	glEnd();
	
	glColor3fv(color[0]);
	double c = (3.14159)/180.0;
	
	for(phi=-90.0;phi<90.0;phi+=endgap){
	    glColor3d(cos(c*phi),0.0,0.0);
		glBegin(GL_QUAD_STRIP);
		theta=-180.0+(opengap/2);
			y= y1+(radius*(sin(c*theta)*cos(c*phi)));
			z =z1+(radius*(cos(c*theta)*cos(c*phi)));
			x =x1+(radius*sin(c*phi));
			glVertex3d(x,y,z);
			glVertex3d(x,y1,z1);
			y= y1+(radius*(sin(c*theta)*cos(c*(phi+endgap))));
			z =z1+(radius*(cos(c*theta)*cos(c*(phi+endgap))));
			x =x1+(radius*(sin(c*(phi+endgap))));
			glVertex3d(x,y,z);
			glVertex3d(x,y1,z1);
		glEnd();
	}

	glColor3d(0.75,0.75,0.0);
	for(phi=-90.0;phi<90.0;phi+=endgap){
		glBegin(GL_QUAD_STRIP);
		theta=180.0-(opengap/2);
			y= y1+(radius*(sin(c*theta)*cos(c*phi)));
			z =z1+(radius*(cos(c*theta)*cos(c*phi)));
			x =x1+(radius*sin(c*phi));
			
			glVertex3d(x,y,z);
			glVertex3d(x,y1,z1);
			y= y1+(radius*(sin(c*theta)*cos(c*(phi+endgap))));
			z =z1+(radius*(cos(c*theta)*cos(c*(phi+endgap))));
			x =x1+(radius*(sin(c*(phi+endgap))));
			glVertex3d(x,y,z);
			glVertex3d(x,y1,z1);
		glEnd();
	}
}


void ghost(int color,double x, double y, double z){
	sphere(1.0,0,color,color,x+0,y+0.5,z+0);
	cylinder(color,color,color,1,1,true,x+0,y+0,z+0);
	//draw eyes
	sphere(0.25,0,7,7,x+(-0.25),y+1.0,z+(-0.75));
	sphere(0.25,0,7,7,x+0.25,y+1.0,z+(-0.75));	
	//draw eye dots
	sphere(0.1,0,6,6,x+(-0.25),y+1.0,z+(-0.95));	
	sphere(0.1,0,6,6,x+0.25,y+1.0,z+(-0.95));	 	
}

void pacman(double x, double y, double z){
	//by default mouth = 40
	sphere(1,mouthgap,3,3,x+0,y+0,z+0);
	pacmouth(1,mouthgap,3,x+0,y+0,z+0);
	//draw eyes
	sphere(0.25,0,7,7,x+0.25,y+0.5,z+(-0.6));
	sphere(0.25,0,7,7,x+(-0.25),y+0.5,(z+-0.6));
	//draw eye dots
	sphere(0.1,0,6,6,x+0.25,y+0.5,z+(-0.8));
	sphere(0.1,0,6,6,x+(-0.25),y+0.5,(z+-0.8));
}

void myDisplay(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	if(!isoview){
		gluLookAt(0.0,2.0,0.0,
			0.0,0.0,0.0,	
			0.0,0.0,1.0);	   
	}else{
		gluLookAt(v1,v2,v3,v4,v5,v6,v7,v8,v9);	   
	}
	
	//draw the map
	for(int i=0;i<21;i++){
		for(int j=0;j<21;j++){
			glPushMatrix();
			glTranslatef(-10+j,0,10-i);
			if(map[i][j]=='#'){
				glColor3fv(color[2]);
				glutSolidCube(1.0);
			}else{
				if(map[i][j]=='.'){
					glPushMatrix();
					glColor3f(0xdb,0xd8,0x41);
					glutSolidSphere(0.125,10,10);
					glPopMatrix();
				}else{
					if(map[i][j]=='?'){
						glColor3fv(color[0]);
						glutSolidCube(1.0);
					}

			}
			}
			glPopMatrix();
		}
	}
	
	glPushMatrix();
	glScalef(0.5,0.5,0.5);
	//draw pacman
	glPushMatrix();
		glTranslatef(pacCurX,pacCurY,pacCurZ);
		glRotatef(rot,0,1,0);
		pacman(0,0,0);	
		glTranslatef(-pacCurX,-pacCurY,-pacCurZ);	   	   
	glPopMatrix();			
	//pacman(pacCurX,pacCurY,pacCurZ);
	//draw ghosts
	glPushMatrix();
		glTranslatef(ghostCurPos[0][0],ghostCurPos[0][1],ghostCurPos[0][2]);
		glRotatef(ghostRot[0],0,1,0);
		ghost(4,0,0,0);	
		glTranslatef(-ghostCurPos[0][0],-ghostCurPos[0][1],-ghostCurPos[0][2]);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(ghostCurPos[1][0],ghostCurPos[1][1],ghostCurPos[1][2]);
		glRotatef(ghostRot[1],0,1,0);
		ghost(0,0,0,0);	
		glTranslatef(-ghostCurPos[1][0],-ghostCurPos[1][1],-ghostCurPos[1][2]);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(ghostCurPos[2][0],ghostCurPos[2][1],ghostCurPos[2][2]);
		glRotatef(ghostRot[2],0,1,0);
		ghost(5,0,0,0);	
		glTranslatef(-ghostCurPos[2][0],-ghostCurPos[2][1],-ghostCurPos[2][2]);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(ghostCurPos[3][0],ghostCurPos[3][1],ghostCurPos[3][2]);
		glRotatef(ghostRot[3],0,1,0);
		ghost(1,0,0,0);	
		glTranslatef(-ghostCurPos[3][0],-ghostCurPos[3][1],-ghostCurPos[3][2]);
	glPopMatrix();


	//ghost(4,ghostCurPos[0][0],ghostCurPos[0][1],ghostCurPos[0][2]);
	//ghost(0,ghostCurPos[1][0],ghostCurPos[1][1],ghostCurPos[1][2]);
	//ghost(5,ghostCurPos[2][0],ghostCurPos[2][1],ghostCurPos[2][2]);
	//ghost(1,ghostCurPos[3][0],ghostCurPos[3][1],ghostCurPos[3][2]);
	//ghost(0,3,0,-3);
	//ghost(5,-3,0,3);
	//ghost(1,-3,0,-3);
	glPopMatrix();
	
	glFlush();
	glutSwapBuffers();
}


void normalIdleFunc(){
}

//returns false is pos is not taken up with block
bool checkpos(double x,double y,double z,bool pac,int ghostNum){
	int px,pz;
	px = ((int)(x/2)+10);
	pz = 20-((int)(z/2)+10);
	
	if(pac){
		char t = map[pz][px];
		map[pz][px]='P';
		//printout();
		map[pz][px]=t;
		//printf("px %d,pz %d x %f z %f\n",px,pz,x,z);
	}
	if((map[pz][px]=='#')||(map[pz][px]=='?')){//||(!pac&&map[pz][px]=='t')){
		return true;   
	}
	if(map[pz][px]=='T'&&pac){
		//got to end of escape area
		if(px==20){
			pacDestX=-18;
		}else{
			pacDestX=18;
		}
		return true;
	}
	//code that will send ghosts through escape zone
	if(map[pz][px]=='T'&&!pac){
		if(px==20){
			ghostCurPos[ghostNum][0]=-18;
		}else{
			ghostCurPos[ghostNum][0]=18;
		}
	}	
	return false;	 
}


//function to move shosts in a hopefully smooth manor between parts of the grid
void smoothGhostMove(int ghost){
	//ghost cur shost next
	//check to see of the varibles are not equal, this would mean that we a moving between points
	if((ghostDestPos[ghost][0]!=ghostCurPos[ghost][0])||(ghostDestPos[ghost][2]!=ghostCurPos[ghost][2])){
		//animate the ghost
		switch(ghostDirn[ghost]){
			case 0:
				//go up
				ghostCurPos[ghost][2]+=0.2;				
				break;
			case 1:
				ghostCurPos[ghost][0]-=0.2;
				break;
			case 2:
				ghostCurPos[ghost][2]-=0.2;
				break;
			case 3:
				ghostCurPos[ghost][0]+=0.2;
				break;
		};
		//update steps taken
		ghostStep[ghost]++;
		if(ghostStep[ghost]==10){
			ghostStep[ghost]=0;
			ghostCurPos[ghost][0]=ghostDestPos[ghost][0];
			ghostCurPos[ghost][2]=ghostDestPos[ghost][2];
		}
	}else{
		//reset step again
		ghostStep[ghost]=0;
		//generate next postion to goto
		int dir=rand()%4;		
		switch(dir){
			case 0:
				if(checkpos(ghostDestPos[ghost][0],ghostDestPos[ghost][1],ghostDestPos[ghost][2]+2,false,ghost)==false){
					ghostDestPos[ghost][2]+=2;
					ghostDirn[ghost]=0;
					ghostRot[ghost]=180;
				}
				break;
			case 1:
				if(checkpos(ghostDestPos[ghost][0]-2,ghostDestPos[ghost][1],ghostDestPos[ghost][2],false,ghost)==false){
					ghostDestPos[ghost][0]-=2;
					ghostDirn[ghost]=1;
					ghostRot[ghost]=90;
				}																		 
				break;
			case 2:
				if(checkpos(ghostDestPos[ghost][0],ghostDestPos[ghost][1],ghostDestPos[ghost][2]-2,false,ghost)==false){
					ghostDestPos[ghost][2]-=2;
					ghostDirn[ghost]=2;
					ghostRot[ghost]=0;
				}																		
				break;
			case 3:
				if(checkpos(ghostDestPos[ghost][0]+2,ghostDestPos[ghost][1],ghostDestPos[ghost][2],false,ghost)==false){
					ghostDestPos[ghost][0]+=2;
					ghostDirn[ghost]=3;
					ghostRot[ghost]=270;
				}													 
				break;
		};
	}
}

//function that will move the ghosts around at specifies intervals
void ghostTimer(int ghost){
	//switch on the ghost, this determines which one to move
	switch(ghost){
		case 0:
			//this is pinky, she's purpleish
			glutTimerFunc(ghostSpeed[1],ghostTimer,0);
			smoothGhostMove(0);
			break;
		case 1:
			//this is clyde, he is faster than other ghosts
			glutTimerFunc(ghostSpeed[0],ghostTimer,1);
			smoothGhostMove(1);
			break;
		case 2:
			glutTimerFunc(ghostSpeed[1],ghostTimer,2);
			smoothGhostMove(2);
			break;
		case 3:
			glutTimerFunc(ghostSpeed[1],ghostTimer,3);
			smoothGhostMove(3);
			break;
	};	
}


//function that moves the pacman around the screen
void pacTimer(int option){
	//update the time vlaue
	timevalue=timevalue+0.01;
	//register call back
	glutTimerFunc(10,pacTimer,1);
	if((pacCurX!=pacDestX)||(pacCurZ!=pacDestZ)){
		//make pac's move
		switch(curdirn){
			case 0:
				break;
			case 1:
				pacCurZ=pacCurZ+offset;
			break;
			case 2:
				pacCurX=pacCurX-offset;
				break;
			case 3:
				pacCurZ=pacCurZ-offset;
				break;
			case 4:
				pacCurX=pacCurX+offset;
				break;
			};
		//printf("%f, %f\n",pacCurX,offset);
		step++;
		//check to see if a pill is in the way and that we are half way into the 
		//square, if so then remove the pill
		int px,pz;
		px = ((int)(pacDestX/2)+10);
		pz = 20-((int)(pacDestZ/2)+10);
		if(map[pz][px]=='.'&&(step>(stepmax/2))){
			map[pz][px]=' ';
			eatenPills++;
			score++;
			//check to see of we have eaten all the pil;s
			if(numPills==(eatenPills)){
				ghostSpeed[0]=(int)(ghostSpeed[0]*0.95);
				ghostSpeed[1]=(int)(ghostSpeed[1]*0.95);
				printf("Ghost Speed is %d and Clyde Speed is %d\n",ghostSpeed[1],ghostSpeed[0]);
				//re read map
				if(readmap()<0){
					printf("A problem occured when reading the map file(map.txt). \nThe program cannot continue. Contact sdp100@ecs.soton.ac.uk \n");
					exit(0);
				}
			}
		}
		if(step==stepmax){
			step=0;
			pacCurX=pacDestX;
			pacCurZ=pacDestZ;
		}
	}else{
		//reset the value of step
		step=0;
		//check to see if were going in the same direction as before
		//if(curdirn==nextdirn){
			//keep going in same direction. this could also indicate no
			//key presses
			//check to see if there is a block in the way
			
		//}else{
			//we need to change direction
			switch(nextdirn){
			case 0:
				//do nothing
				break;
			case 1:
				//we wanna go up so check square up, if false then move there
				if(checkpos(pacDestX,pacDestY,pacDestZ+2,true,0)==false){
					pacDestZ+=2;
					curdirn=1;					
				}
				rot=180;
				break;
			case 2:
				//we wanna go left so check square left, if false then move there
				if(checkpos(pacDestX-2,pacDestY,pacDestZ,true,0)==false){
					pacDestX-=2;
					curdirn=2;					
				}
				rot=90;
				break;
			case 3:
				//we wanna go down so check square down, if false then move there
				if(checkpos(pacDestX,pacDestY,pacDestZ-2,true,0)==false){
					pacDestZ-=2;
					curdirn=3;					
				}
				rot=0;
				break;
			case 4:
				//we wanna go right so check square right, if false then move there
				if(checkpos(pacDestX+2,pacDestY,pacDestZ,true,0)==false){
					pacDestX+=2;
					curdirn=4;					
				}
				rot=270;
				break;

			default:
				//do nothing
				break;
		
			};
		//}		
	}
}


//function to operate the tour functionality
void pacTourTimer(int var){
	if(tourCount>10){
		tour=false;
		//reset the pac pos and map when done
		pacCurZ=-10.0;
		pacCurX=0.0;
		pacCurY=0.0;
		pacDestZ=-10.0;
		pacDestX=0.0;
		pacDestY=0.0;
		curdirn=0;
		nextdirn=0;
		rot=0;
		score=0;
		//attempt to read from the map file
			if(readmap()<0){
				//a problem occured.
				printf("The map file (map.txt) could not be read from! \n The program cannot continue! Contact sdp100@ecs.soton.ac.uk");
				exit(0);
			}							 
	}else{
		glutTimerFunc(500,pacTourTimer,0);
		pacDestX=tourArr[tourCount][0];
		pacDestZ=tourArr[tourCount][1];
		curdirn=tourArr[tourCount][2];
		rot=tourArr[tourCount][3];
		tourCount++;
	}
}

//animates pm's mouth and moves ghosts around
void pacIdleFunc(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if((mouthgap>35) || (mouthgap<0)){
		gap=-gap;
	}
	mouthgap+=gap;
	glutPostRedisplay();	
}

void mySpecialKeyboard(int key,int x, int y){
	//if tour == false then all keys
	if(tour==false){
	switch(key){
			case GLUT_KEY_UP:
				nextdirn=1;
	    		break;
			case GLUT_KEY_LEFT:
				nextdirn=2;
				break;
			case GLUT_KEY_DOWN:
				nextdirn=3;
				break;
			case GLUT_KEY_RIGHT:
				nextdirn=4;
				break;
			default:
				//do nothing
				break;
		};
	}
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY){
	
	switch(theKey){
	case 'Q':
	case 'q':
		printf("You have scored %d points!!! Well Done!!\n",score);
		exit(0);
		break;
	
	case 'P':
	case 'p':
		isoview=!isoview;
		break;
	
	case 'o':
	case 'O':
		inc=-inc;
		break;
	case 'T':
	case 't':
		tour=true;
		tourCount=0;
		//reset the pac pos and map when done
        pacCurZ=-10.0;
        pacCurX=0.0;
        pacCurY=0.0;
        pacDestZ=-10.0;
        pacDestX=0.0;
        pacDestY=0.0;
		curdirn=0;
		nextdirn=0;
        //attempt to read from the map file
        if(readmap()<0){
	        //a problem occured.
	        printf("The map file (map.txt) could not be read from! \n The program cannot continue! Contact sdp100@ecs.soton.ac.uk");
	        exit(0);
	    }
		glutTimerFunc(500,pacTourTimer,0);																																	
		//v1=v1+inc;
		//l1+=inc;

		//do the animated tour
		break;	
	case 'y':
		//v2=v2+inc;
		l1+=inc;
		break;
	case 'u':
		//v3=v3+inc;
		l3+=inc;
		break;
	case 'g':
		//v4=v4+inc;
		l4+=inc;
	//	break;
	case 'h':
		//v5=v5+inc;
		l5+=inc;
		break;
	case 'j':
		//v6=v6+inc;
		l6+=inc;
		break;
	case 'b':
		//v7=v7+inc;
		l7+=inc;
		break;
	case 'n':
		//v8=v8+inc;
		l8+=inc;
		break;
	case 'm':
		//v9=v9+inc;
		l9+=inc;
		break;
	default:
		break;
	}
	//printout values
	//printf("v1=%f,v2=%f,v3=%f \n",v1,v2,v3);
	//printf("v4=%f,v5=%f,v6=%f \n",v4,v5,v6);
	//printf("v7=%f,v8=%f,v9=%f \n\n\n",v7,v8,v9);
	printf("l1=%f,l2=%f,l3=%f \n",l1,l2,l3);
	printf("l4=%f,l5=%f,l6=%f \n",l4,l5,l6);
	printf("l7=%f,l8=%f,l9=%f \n\n\n",l7,l8,l9);
	 
	glutPostRedisplay();
}

void myReshape(int width, int height){
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(width<=height){
		glOrtho(10.0,-10.0,-10.0*(GLfloat)height/(GLfloat)width,
							10.0*(GLfloat)height/(GLfloat)width,-10.0,200.0);
	}else{
		glOrtho(10.0*(GLfloat)width/(GLfloat)height,
							-10.0*(GLfloat)width/(GLfloat)height,-10.0,10.0,-10.0,200.0);
	}
	glMatrixMode(GL_MODELVIEW);
}


void myMouse(int button,int state,int mouseX, int mouseY){
}

//main function
int main(int argc, char** argv){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(400,400);
	glutInitWindowPosition(150,100);
	glutCreateWindow("CM303 Pacman Game");
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();	   
	
	//register call back funtions
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(pacIdleFunc);
	
	//attempt to read from the map file
	if(readmap()<0){
		//a problem occured.
		printf("The map file (map.txt) could not be read from! \n The program cannot continue! Contact sdp100@ecs.soton.ac.uk");
		exit(0);
	}
		
	glEnable(GL_DEPTH_TEST);
	
	//lighting	
    GLfloat specular[4] = {1.0,1.0,1.0,1.0};
    GLfloat ambience[4] = {0.33f,0.33f,0.33f,1.0};
	GLfloat diffuse[4] = {0.6f,0.6f,0.6f,1.0};
    GLfloat pos[4] = {0.0,0.0,5.0,1.0};
	
    GLfloat mat_shininess[1] = {25.0};
    GLfloat mat_specular[4] = {1,1,1,1};
    GLfloat mat_emission[4] = {0,0,0,1};

    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambience);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	//end of lighting 
	
	myInit();
	pacCurZ=-10.0;
	pacCurX=0.0;
	pacCurY=0.0;
	pacDestZ=-10.0;
	pacDestX=0.0;
	pacDestY=0.0;

	//enable arrow keys
	glutSpecialFunc(mySpecialKeyboard);

	v1=1.5;v2=8.5;v3=-5;v4=0;v5=0;v6=0;v7=0;v8=1;v9=0;
	//setup ghot speed var
	ghostSpeed[0]=25;
	ghostSpeed[1]=50;
	//register timer functions for pacman movement and ghosts
	glutTimerFunc(50,pacTimer,1);
	glutTimerFunc(10,ghostTimer,0);
	glutTimerFunc(10,ghostTimer,1);
	glutTimerFunc(10,ghostTimer,2);
	glutTimerFunc(10,ghostTimer,3);
	glutMainLoop();
}

