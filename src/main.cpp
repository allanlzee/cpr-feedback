
#include <Wire.h>
#include <FaBo9Axis_MPU9250.h>
#include <BasicLinearAlgebra.h>

#include "quaternions.hpp"


namespace aacm {

struct accelerometer {
  const double decay = 0.1;

  // MPU9259 devicw
  FaBo9Axis fabo_9axis;
  
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

  // int nCount;
  // double vx, vy, vz;
  // double vvx, vvy, vvz;

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

/* //Calibration
    nCount = 0;
    vx = 0.0;
    vy = 0.0;
    vz = 0.0;
    vvx = 0.0;
    vvy = 0.0;
    vvz = 0.0;
   */

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

/*
    ax = dax*1.0063649 - day*0.061666669 - daz*0.095601035 + 0.013704492;
    ay = dax*0.075509158 + day*0.98790271 - daz*0.017894963 + 0.10671609;
    az = dax*0.0864055 - day*0.044063031 + daz*0.9774707 + 0.020521007;
*/

    /*
    BLA::Matrix<3,3> mCalibration = {1.00601190779139,-0.0102187128058640,0.0335777290836605,
                                     0.0854333897034126,0.902724594438707,0.131758961655363,
                                     -0.0504916131664662,-0.0286381862563277,0.996059633488924};
    fabo_9axis.readAccelXYZ(&ax,&ay,&az);
    BLA::Matrix<3> vAccel = {ax,ay,az};
    vAccel = (mCalibration)*vAccel;
    ax = vAccel(0);
    ay = vAccel(1);
    az = vAccel(2);
    */
    
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

    double dd = sqrt(ax*ax+ay*ay+az*az);
    // force += decay*tau*(dd - force);
    qAverRot = qAverRot + decay*tau*(qCurrRot - qAverRot);

    // Serial.print("F = ");
    // Serial.print(dd);
    // Serial.print("   \t");

    // Calculate calibration matrix
    /*
    if ( dTimeCurr > 2000 )
    {
      ++nCount;
      vx = ((nCount-1)*vx+ax)/nCount;
      vy = ((nCount-1)*vy+ay)/nCount;
      vz = ((nCount-1)*vz+az)/nCount;      
    }

    quaternion qtmp(0.0, 1000*vx, 1000*vy, 1000*vz);
    TestPrint( qtmp, "ACCEL ");
    */
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
    // Serial.print("\t");
    // Serial.print(qabs(q));
    Serial.println();
  }

  void setAcceleration(double dax, double day, double daz)
  {
    qCurrAccel = quaternion(0.0, dax, day, daz);
  }

  void setRotation(double wx, double wy, double wz)
  {
    qCurrRot = quaternion(0.0,wx,wy,wz);
    // TestPrint(1000*qCurrRot, "qAnglVel");
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

      RMat /= (dTotalWeight*force*force);

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

/*
      Serial.print("CM \n");
      for(int i=0; i<4; ++i)
      {
      for(int j=0; j<4; ++j){
        Serial.print(CM(i,j));
        Serial.print("\t");
      }
      Serial.print("\n");
      }
*/
       

      
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

        // TestPrint(qq);
        // TestPrint(qres);
        double dnorm = qabs(qres);
        qq = qres/dnorm;
        // TestPrint(qq);
        // Serial.print ("EV = ");
        // Serial.print(dnorm);
        // Serial.print ("\n");   
      }


      qCurrOrient = qq;


      //TEST
      if ( false )
      {
         
          quaternion qr = qCurrOrient.conj()*qCurrAccel*(qCurrOrient);
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

    quaternion qTrueAccel = qCurrOrient.conj() * qCurrAccel * (qCurrOrient);
    
    qTrueAccel = qTrueAccel - quaternion(0.0, 0.0, 0.0, 1.0);
    // TestPrint(1000*qTrueAccel, "Accel");

    static double dFF4 = 0.5;
    qaver = (1 - dFF4*tau)*qaver + dFF4*tau*qTrueAccel;
    qTrueAccel = qTrueAccel - qaver;
    // qTrueAccel = (1.0 - 0.4*tau)*qTrueAccel;
    
    // quaternion qshift=qCurrOrient*quaternion(0.0, 0.0, 0.0, qaver.d())*conj(qCurrOrient);

    // const double dFF2 = 0.1;
    // qVelAverage = (1 - dFF2*tau)*qVelAverage +  dFF2*tau*qVel;

    // qVel = qVel + tau*qTrueAccel;
    // qPos = qPos + tau*(qVel-qVelAverage);
    quaternion qPosNN = 2*qPos - qPosPP; 
    qPosNN = qPosNN + (tau*tau)*qTrueAccel;
    qPosPP = qPos;
    qPos = qPosNN;
    
    // TestPrint(1000*qTrueAccel, "Accel");
    // TestPrint(1000*qaver, "Shift");
    // TestPrint(1000*qCurrOrient, "Orientation");
    // TestPrint((9820.0)*qPos, "Position");
    // TestPrint((9820.0)*qVel, "Velocity");
    // Serial.println(force);
    // Serial.println(9820.0*qPos.d());
    // TestPrint((qCurrRot-qAverRot), "angular_velocity");
    // Decay
    // Serial.println(1000*tau);
    // TestPrint(quaternion(0.0, angl(0), angl(1), angl(2)), "angle");

    // fudge factors
    // qVel = (1.0 - 2.0*tau)*qVel;
    const double dFF7 = 3.2; // use 4.0
    qPos = (1.0 - 1.01*dFF7 *tau)*qPos;
    qPosPP = (1.0 - dFF7 *tau)*qPosPP;


    /* static double am_peak(0.0);
    if ( qPos.d() > am_peak )
    {
      am_peak = qPos.d();
    }

    diff = am_peak - qPos.d();
    am_peak *= (1-0.2*tau);*/
 

    if (posPrev[2] < posPrev[1] && posPrev[1] < posPrev[0] && qPos.d() > posPrev[0]) {
      peak_prev = peak;
      peak = qPos.d();
    }
    diff = peak - qPos.d();
    diff = 1.2 * diff;
    posPrev[2] = posPrev[1];
    posPrev[1] = posPrev[0];
    posPrev[0] = qPos.d();
    // Serial.println(9820 * diff);

    if (9820 * diff > 50) {
      // digitalWrite(motorPin, HIGH);
      // Serial.println(9820 * diff);
      motor_count = 0;
    }
    motor_count = motor_count + 1;
    if (motor_count == 10) {
      digitalWrite(motorPin, LOW);
      //Serial.println(freeMemory());
      motor_count = 0;
    }
    /* 
    Serial.print((9820.0) * peak);
    Serial.print("\t");
    Serial.print((9820.0) * peak_prev);
    Serial.print("\t");
    Serial.print((9820.0) * (diff + 1.2 * (peak_prev - peak)));
    Serial.print("\t");
    Serial.print(50.0);
    Serial.println();
    */
    
    // quaternion qVelNext = qVel + tau*qTrueAccel;
    // quaternion qPosNext = qPos + 0.5*tau*(qVel + qVelNext);
    // qPos = qPos + tau*qVelAverage;

    
    // qVel = qVelNext;
    // qPos = qPosNext;

    rate_n = rate_n + 1;
    if (9820 * diff > 40  && rate_n > 20) { // < -4.0
        t_prev_trough[0] = t_prev_trough[1];
        t_prev_trough[1] = t_prev_trough[2];
        t_prev_trough[2] = t_prev_trough[3];
        t_prev_trough[3] = millis();
        rate_n = 0;
    }
    rate = ((t_prev_trough[3] - t_prev_trough[0]) / 3) / 1000;
    // Serial.println(rate);
    Serial.println(9820 * diff);
    Serial.println(millis());
    if (rate > 0.63) {
      Serial.println("Too slow");
    }
    else if (rate < 0.48) {
      Serial.println("Too fast");
    }
    else {
      Serial.println("Good");
    }

    dt += tau;

    // What units are tau
    
    return dt;
  }
  
};

}

aacm::accelerometer ac;

void setup()
{
  Serial.begin(115200);
  // Serial.begin(1000000);
  Serial.println("RESET");
  Serial.println();

  Serial.println("configuring device.");

  //AM: fabo_9axis.dumpConfig();

  if ( ac.fabo_9axis.begin() ) {
    Serial.println("configured FaBo 9Axis I2C Brick");
  } else {
    Serial.println("device error");
    while(1);
  }    
  
  ac.initialize();
}

void loop() 
{
  // carry out measurments

  ac.update();

  // update orientation

  ac.qCurrOrient = ac.rotation();


  // calculate position

  double dTimeNow = ac.integrate();
}
