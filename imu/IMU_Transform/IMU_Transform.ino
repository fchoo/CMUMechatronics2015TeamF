// #include <MatrixMath.h>
#include <math.h>


// code to handle transformation of IMU to get orientation at center
// of robot chassis


// Define location of IMU with respect to center of robot chassis
//
//  ||-----------||
//        ^ y
//        |
//        z--> x
//
//  ||-----------||
//
// roll = roation about y-axis
// pitch = rotation about x-axis
// yaw = rotation about z-axis
//---------------------------------------------------------------------------


// Helper functions
#define ToRad(x) ((x)*0.01745329252)  // *pi/180
#define ToDeg(x) ((x)*57.2957795131)  // *180/pi
#define pi 3.1415926

// setup to pass array from function to main loop
const int N = 4;
const int M = 4;
float T[N][M];

// Define location of IMU with respect to origin at the center of the chassis
float xr = -0.1175;      //[m]
float yr = 0.13;    //[m]
float zr = 0.035;       //[m]

float T_imu[4][4];
float T_offset[4][4];


void setup()
{
  Serial.begin(115200);

  AHRS_Init();
}


void loop()
{
  delay(250);
  // Get IMU data
  Read_AHRS();
  float roll = ToRad(Get_Roll());
  float pitch = ToRad(Get_Pitch());
  float yaw = ToRad(Get_Yaw());
  Serial.println(" ");
  Serial.print("PRE: ");
  Serial.print("roll: ");
  Serial.print(Get_Roll());
  Serial.print(" pitch: ");
  Serial.print(Get_Pitch());
  Serial.print(" yaw: ");
  Serial.println(Get_Yaw());

  float R_x[4][4];
  float R_y[4][4];
  float R_z[4][4];
  // Generate Rotation Matrix describing orientation of IMU and robot
  RotMat(pitch, (float *)R_x, 'x');
  RotMat(roll, (float *)R_y, 'y');
  RotMat(yaw, (float *)R_z, 'z');

  float T1[4][4];
  float R_imu[4][4];
  float T_robot[4][4] = {{1.0,0.0,0.0,0.0},{0.0,1.0,0.0,0.0},{0.0,0.0,1.0,0.0},{-xr,-yr,-zr,1.0}};
  // now use MatrixMath library to do some transformations
  MatrixMult((float*)R_x,(float*) R_y,4,4,4,(float*) T1);
  MatrixMult((float*)T1,(float*) R_z,4,4,4,(float*) R_imu);

  float G_imu[4][4];
  MatrixMult((float*)R_imu,(float*) T_robot,4,4,4,(float*) G_imu);

  // call function to take in transformation matrix and spit out roll, pitch, and yaw of the robot's center
  float pose[3];
  fullPose(G_imu, (float *) pose);

  Serial.print("POS: ");
  Serial.print("roll: ");
  Serial.print(ToDeg(pose[0]));
  Serial.print(" pitch: ");
  Serial.print(ToDeg(pose[1]));
  Serial.print(" yaw: ");
  Serial.println(ToDeg(pose[2]));

}


// takes an arbitray rotation matrix and calculates the euler angles
// alpha, beta, and gamma (roll, pitch, yaw)
void fullPose(float R[4][4], float* pose)
{
  // solve for beta first
  float beta = asin(R[1][3]);

  // solve for alpha
  float y1 = -(R[2][3])/(cos(beta));
  float x1 = (R[3][3])/(cos(beta));
  float alpha = atan2(y1,x1);

  // solve for gamma
  float y2 = -(R[1][2])/(cos(beta));
  float x2 = (R[1][1])/(cos(beta));
  float gamma = atan2(y2,x2);

 pose[0] = alpha;
 pose[1] = beta;
 pose[2] = gamma;

}


// create a rotation matrix with from specified euler angle
void RotMat(float theta, float* R, char axis)
{

  if (axis == 'x') {
    float temp[4][4] = {{1.0,0.0,0.0,0.0},{0.0,(cos(theta)),(-sin(theta)),0.0},{0.0,(sin(theta)),(cos(theta)),0.0},{0.0,0.0,0.0,1.0}};
    MatrixCopy((float*)temp,4,4,R);
  }
  else if (axis == 'y') {
    float temp[4][4] = {{cos(theta),0.0,sin(theta),0.0},{0.0,1,0.0,0.0},{-sin(theta),0.0,cos(theta),0.0},{0.0,0.0,0.0,1.0}};
    MatrixCopy((float*)temp,4,4,R);
  }
  else if (axis == 'z') {
    float temp[4][4] = {{cos(theta),-sin(theta),0.0,0.0},{sin(theta),cos(theta),0.0,0.0},{0.0,0.0,1.0,0.0},{0.0,0.0,0.0,1.0}};
    MatrixCopy((float*)temp,4,4,R);
  }
  else {
    float temp[4][4] = {{1.0,0.0,0.0,0.0},{0.0,1.0,0.0,0.0},{0.0,0.0,1.0,0.0},{0.0,0.0,0.0,1.0}};
    MatrixCopy((float*)temp,4,4,R);
  }
}

void MatrixMult(float* A, float* B, int m, int p, int n, float* C)
{
// A = input matrix (m x p)
// B = input matrix (p x n)
// m = number of rows in A
// p = number of columns in A = number of rows in B
// n = number of columns in B
// C = output matrix = A*B (m x n)
  int i, j, k;
  for (i=0;i<m;i++)
    for(j=0;j<n;j++)
    {
      C[n*i+j]=0;
      for (k=0;k<p;k++)
        C[n*i+j]= C[n*i+j]+A[p*i+k]*B[n*k+j];
    }
}

void MatrixCopy(float* A, int n, int m, float* B)
{
  int i, j, k;
  for (i=0;i<m;i++)
    for(j=0;j<n;j++)
    {
      B[n*i+j] = A[n*i+j];
    }
}

void MatrixPrint(float* A, int m, int n){
  // A = input matrix (m x n)
  int i,j;
  Serial.println(' ');
  for (i=0; i<m; i++){
    for (j=0;j<n;j++){
      Serial.print(A[n*i+j]);
      Serial.print("\t");
    }
    Serial.println(' ');
  }
}
