#include <Wire.h>
#include <FaBo9Axis_MPU9250.h>
#include <BasicLinearAlgebra.h>
#include "MPU9250.h"

#include "quaternions.hpp"

#define FEEDBACK_MODE true

namespace aacm {

struct accelerometer {
  const double decay = 0.1;

  // MPU9259 devicw
  FaBo9Axis fabo_9axis;
  //MPU9250 mpu;
  
  double dTimePrev;
  double dTimeCurr;

  double force;
  double tau;

  quaternion qAverRot;
  quaternion qCurrRot;

  quaternion qCurrAccel;

  quaternion qInitOrient;
  quaternion qPrevOrient;
  quaternion qCurrOrient;

  quaternion qVel;
  quaternion qPos;
  quaternion qPosPP;

  quaternion qVelAverage;

  double motor_count = 0;
  int motorPin = 13;

  double t_prev_trough[4] = {0, 0, 0, 0};
  int rate_n = 0;
  double rate;

  // INTEGRATION
  double lambda;
  double omega;
  double I0, I1, I2, I3, I4; 

  double posPrev[3] = {0, 0, 0};
  double peak = 0.0;
  double diff = 0.0;
  double peak_prev = 0.0;

public:
  void initialize() {
    fabo_9axis = FaBo9Axis(0x68);
    float ax,ay,az;
    float gx,gy,gz;
    float mx,my,mz;
    float temp;

    fabo_9axis.readAccelXYZ(&ax,&ay,&az);
    fabo_9axis.readGyroXYZ(&gx,&gy,&gz);
    fabo_9axis.readMagnetXYZ(&mx,&my,&mz);
    fabo_9axis.readTemperature(&temp);

    force = 1.0;
    qAverRot = quaternion(0.0, gx, gy, gz);
    qCurrAccel = quaternion(0.0, ax, ay, az);
    dTimeCurr = micros();
    
    qInitOrient = quaternion(1.0, 0.0, 0.0, 0.0);
    qPrevOrient = quaternion(1.0, 0.0, 0.0, 0.0);
    qCurrOrient = quaternion(1.0, 0.0, 0.0, 0.0);

    qPos = quaternion(0.0, 0.0, 0.0, 0.0);
    qPosPP = quaternion(0.0, 0.0, 0.0, 0.0);
    qVel = quaternion(0.0, 0.0, 0.0, 0.0);
    qVelAverage = quaternion(0.0, 0.0, 0.0, 0.0);
    qCurrOrient = quaternion(0.0, 0.0, 0.0, 0.0);  

   // Integration
   lambda = 1.0; // 1/sec
   omega = 1.0; // 1/sec
   I0 = 0.0;
   I1 = 0.0;
   I2 = 0.0;
   I3 = 0.0;
   I4 = 0.0;
  }
    
  void update(){
    float ax,ay,az;
    float gx,gy,gz;
    float mx,my,mz;
    float temp;

    fabo_9axis.readAccelXYZ(&ax,&ay,&az);

     double dax = ax;
     double day = ay;
     double daz = az;

    double ddax = dax*1.0063649 - day*0.061666669 - daz*0.095601035 + 0.013704492 + 0.179 ; 
    double dday = dax*0.075509158 + day*0.98790271 - daz*0.017894963 + 0.10671609 + 0.037 ;
    double ddaz = dax*0.0864055 - day*0.044063031 + daz*0.9774707 + 0.020521007 + 0.0164;
    
    fabo_9axis.readGyroXYZ(&gx,&gy,&gz);

    gx = gx + 10.0;
    gy = gy + 4.5;
    gz = gz + 0.7;
    
    fabo_9axis.readMagnetXYZ(&mx,&my,&mz);
    fabo_9axis.readTemperature(&temp);
   
    setAcceleration(ddax, dday, ddaz);
    setRotation(gx, gy, gz);

    dTimePrev = dTimeCurr;
    dTimeCurr = micros();

    tau = 0.000001 * (dTimeCurr - dTimePrev);

    //double dd = sqrt(ax*ax+ay*ay+az*az);
    qAverRot = qAverRot + decay*tau*(qCurrRot - qAverRot);
  }

  void TestPrint(const quaternion& q, const char* wht = "Q =")
  {
    // Serial.println();
    Serial.print(wht);
    Serial.print("\t");
    Serial.print(q.a());
    Serial.print("\t");
    Serial.print(q.b());
    Serial.print("\t");
    Serial.print(q.c());
    Serial.print("\t");
    Serial.print(q.d());
    Serial.println();
  }

  void setAcceleration(double dax, double day, double daz)
  {
    qCurrAccel = quaternion(0.0, dax, day, daz);
  }

  void setRotation(double wx, double wy, double wz)
  {
    qCurrRot = quaternion(0.0,wx,wy,wz);
  }

  quaternion rotation()
  {
      BLA::Matrix<3,3> RMat;
      RMat.Fill(0.0);

      double dTotalWeight = 0.0;
      double w = 10.0;
      
      dTotalWeight += w;
      {
         double vB[3] = {0.0, 0.0, 1.0};
         double vA[3] = {qCurrAccel.b(), qCurrAccel.c(), qCurrAccel.d()};

        for(int i=0; i<3; ++i){
          for(int j=0; j<3; ++j){
            RMat(i,j) += w*vA[i]*vB[j];
          }
        }
      }

      RMat = RMat / (dTotalWeight*force*force);

      BLA::Matrix<4,4> CM;

      CM(0,0) = RMat(0,0)+RMat(1,1)+RMat(2,2);
      CM(0,1) = -RMat(1,2)+RMat(2,1);
      CM(0,2) = -RMat(2,0)+RMat(0,2);
      CM(0,3) = -RMat(0,1)+RMat(1,0);

      CM(1,0) = CM(0,1);
      CM(1,1) = 2*RMat(0,0) - CM(0,0);
      CM(1,2) = RMat(0,1)+RMat(1,0);
      CM(1,3) = RMat(0,2)+RMat(2,0);

      CM(2,0) = CM(0,2);
      CM(2,1) = CM(1,2);
      CM(2,2) = 2*RMat(1,1) - CM(0,0);
      CM(2,3) = RMat(1,2)+RMat(2,1);

      CM(3,0) = CM(0,3);
      CM(3,1) = CM(1,3);
      CM(3,2) = CM(2,3);
      CM(3,3) = 2*RMat(2,2) - CM(0,0);

      double dFudgeFactor = 1.0;

      double rtau = dFudgeFactor*0.5*tau*M_PI/180.0;
      quaternion qAnglVel = quaternion(1.0,0.0,0.0,0.0) + (rtau)*qCurrRot;

      qAnglVel = qPrevOrient*qAnglVel; 
      double wGyro = 0.1; // Weight for gyroscope

      // Solve eigenvalue problem
      quaternion qq = qPrevOrient;

      int nIter = 8;
      double dnorm;
      for(int ii=0; ii<nIter; ++ii)
      {
        double dv0 = CM(0,0)*qq.a() + CM(0,1)*qq.b() + CM(0,2)*qq.c() + CM(0,3)*qq.d(); 
        double dvx = CM(1,0)*qq.a() + CM(1,1)*qq.b() + CM(1,2)*qq.c() + CM(1,3)*qq.d(); 
        double dvy = CM(2,0)*qq.a() + CM(2,1)*qq.b() + CM(2,2)*qq.c() + CM(2,3)*qq.d(); 
        double dvz = CM(3,0)*qq.a() + CM(3,1)*qq.b() + CM(3,2)*qq.c() + CM(3,3)*qq.d(); 
        
        quaternion qres(dv0, dvx, dvy, dvz);

        qres = qq + 0.4*(qres - wGyro*qAnglVel);

        double dnorm = qabs(qres);
        qq = qres/dnorm;
      }


      qCurrOrient = qq;

      if ( false )
      {
         
          quaternion qr = conj(qCurrOrient)*qCurrAccel*(qCurrOrient);
          quaternion qf(0, 0, 0, force);

          quaternion qdiff = qr - qf;
          TestPrint( qr );
          TestPrint( qf );
          TestPrint( qdiff );
      }

      return qCurrOrient;
  }

  double integrate()
  {
    static double dt(0.0);
    static quaternion qaver(0.0, 0.00022, 0.00007, -0.0039);

    quaternion qTrueAccel = conj(qCurrOrient) * qCurrAccel * (qCurrOrient);
    
    qTrueAccel = qTrueAccel - quaternion(0.0, 0.0, 0.0, 1.0);

    static double dFF4 = 0.5;
    qaver = (1 - dFF4*tau)*qaver + dFF4*tau*qTrueAccel;
    qTrueAccel = qTrueAccel - qaver;

    quaternion qPosNN = 2*qPos - qPosPP; 
    qPosNN = qPosNN + (tau*tau)*qTrueAccel;
    qPosPP = qPos;
    qPos = qPosNN;

    const double dFF7 = 3.2; // use 4.0
    qPos = (1.0 - 1.01*dFF7 *tau)*qPos;
    qPosPP = (1.0 - dFF7 *tau)*qPosPP;

    if (posPrev[2] < posPrev[1] && posPrev[1] < posPrev[0] && qPos.d() > posPrev[0]) {
      peak_prev = peak;
      peak = qPos.d();
    }
    diff = peak - qPos.d();
    diff = 1.0 * diff;
    posPrev[2] = posPrev[1];
    posPrev[1] = posPrev[0];
    posPrev[0] = qPos.d();
    Serial.print(9820 * diff); // gravity 
    Serial.print(","); 

    if (FEEDBACK_MODE) {
      if (9820 * diff > 50) {
        digitalWrite(motorPin, HIGH);
        motor_count = 0;
      }
    
      motor_count = motor_count + 1;
      if (motor_count == 4) {
        digitalWrite(motorPin, LOW);
        motor_count = 0;
      }
    }

    rate_n = rate_n + 1;
    if (9820 * diff > 30  && rate_n > 15) { // < -4.0
        t_prev_trough[0] = t_prev_trough[1];
        t_prev_trough[1] = t_prev_trough[2];
        t_prev_trough[2] = t_prev_trough[3];
        t_prev_trough[3] = millis();
        rate_n = 0;
    }
    rate = ((t_prev_trough[3] - t_prev_trough[0]) / 3) / 1000;
    Serial.print(rate);
    Serial.print(","); 
    
    // Device inaccuracies - set a 0.05 tolerance.
    // Any drift past is concerning.
    if (rate < 0.95) {
      Serial.println("F");
    }
    else if (rate > 1.25) {
      Serial.println("S");
    }
    else {
      Serial.println("G");
    }

    dt += tau;

    return dt;
  }
};
}


void scanI2C() {
    byte error, address;
    int devicesCount = 0;

    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");
            devicesCount++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    if (devicesCount == 0) {
        Serial.println("No I2C devices found\n");
    } else {
        Serial.println("done\n");
    }
}

aacm::accelerometer ac;

void setup()
{
  Serial.begin(9600);

  Serial.println("RESET");
  Serial.println();

  Serial.println("configuring device.");

  Serial.print("Search Device: ");
  Serial.println((int) ac.fabo_9axis.searchDevice());

  if ( ac.fabo_9axis.begin() ) {
    Serial.println("configured FaBo 9Axis I2C Brick");
  } else {
    Serial.println("device error");
    while(1);
  }    
  
  ac.initialize();
}

unsigned long loop_start;
const int kPeriod10Hz = 10;

void loop() 
{
  loop_start = millis();

  float ax = 0; 
  float ay = 0; 
  float az = 0;
  ac.fabo_9axis.readAccelXYZ(&ax, &ay, &az); 

  float gx = 0; 
  float gy = 0; 
  float gz = 0; 
  ac.fabo_9axis.readGyroXYZ(&gx, &gy, &gz);

  Serial.print(millis()); 

  // Acceleration
  Serial.print(",");
  Serial.print(ax);
  Serial.print(",");
  Serial.print(ay); 
  Serial.print(",");
  Serial.print(az);
  Serial.print(",");

  // Gyroscope (Roll, Pitch, Yaw)
  Serial.print(gx);
  Serial.print(",");
  Serial.print(gy); 
  Serial.print(",");
  Serial.print(gz);
  Serial.print(",");
  
  // carry out measurments
  ac.update();

  // update orientation
  ac.qCurrOrient = ac.rotation();

  // calculate position
  double dTimeNow = ac.integrate();
}
