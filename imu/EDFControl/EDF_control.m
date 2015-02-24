CoeFriction=0.6;

Fw=50;
theta=90;
thetarad=theta*(pi/180);

Ff=Fw*sin(thetarad);
Fs=((Fw*sin(thetarad))-(0.6*Fw*cos(thetarad)))/0.6;
fprintf('Friction force needed = %d\n',Ff);
fprintf('Suction force = %d\n',Fs);
%%disp(sprintf('Suction angle = %d\n',theta));