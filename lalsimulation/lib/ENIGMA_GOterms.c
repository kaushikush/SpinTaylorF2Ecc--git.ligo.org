#include <lal/SphericalHarmonics.h>
#include <math.h>

static COMPLEX16 Complex(REAL8 real, REAL8 imag){
	COMPLEX16 z = real + imag * I;
	return z;
 }


//H22
static COMPLEX16 hl_2_m_2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //delta = sqrt(1-4*Nu)
    //vpnorder = x_PNorder *2

    //1PN contribution
    if(vpnorder == 2){
        return ((-2*mass*Nu*sqrt(M_PI/5.)*(21*pow(mass,2)*(-10 + Nu) - 27*(-1 + 3*Nu)*pow(r,2)*(PhiDOT*r 
        - Complex(0,1)*rDOT)*pow(PhiDOT*r + Complex(0,1)*rDOT,3) + mass*r*((11 + 156*Nu)*pow(PhiDOT,2)*pow(r,2) 
        + Complex(0,10)*(5 + 27*Nu)*PhiDOT*r*rDOT - 3*(15 + 32*Nu)*pow(rDOT,2))))/(21.*pow(r,2)*R))*cpolar(1,-2*Phi);
    }

    //2PN contribution
    else if(vpnorder == 4){
        return -((mass*Nu*sqrt(M_PI/5.)*(6*pow(mass,3)*(3028 + 1267*Nu + 158*pow(Nu,2)) 
        + 9*(83 - 589*Nu + 1111*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r - Complex(0,1)*rDOT,2)*pow(PhiDOT*r + Complex(0,1)*rDOT,4) 
        + pow(mass,2)*r*((-11891 - 36575*Nu + 13133*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) + Complex(0,8)*(-773 - 3767*Nu 
        + 2852*pow(Nu,2))*PhiDOT*r*rDOT - 6*(-619 + 2789*Nu + 934*pow(Nu,2))*pow(rDOT,2)) - 3*mass*pow(r,2)*(2*(-835 
        - 19*Nu + 2995*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) + Complex(0,6)*(-433 - 721*Nu + 1703*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + 6*(-33 + 1014*Nu + 232*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,4)*(-863 + 1462*Nu + 2954*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) - 3*(-557 + 664*Nu 
        + 1712*pow(Nu,2))*pow(rDOT,4))))/(378.*pow(r,3)*R)) * cpolar(1,-2*Phi);
    }

     //2.5PN contribution
    else if(vpnorder == 5){
        return ((4*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/5.)*(mass*(Complex(0,1404)*PhiDOT*r 
        - 2*rDOT) + 3*r*(Complex(0,316)*pow(PhiDOT,3)*pow(r,3) 
        + 847*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - Complex(0,184)*PhiDOT*r*pow(rDOT,2) + 122*pow(rDOT,3))))/(105.*pow(r,3)*R)) * cpolar(1,-2*Phi);
    }

    //3PN contribution
    else{
        return (-(mass*Nu*sqrt(M_PI/5.)*(4*pow(mass,4)*(-8203424 + 2180250*pow(Nu,2) 
        + 592600*pow(Nu,3) + 15*Nu*(-5503804 + 142065*pow(M_PI,2))) - 2700*(-507 + 6101*Nu - 25050*pow(Nu,2) 
        + 34525*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r - Complex(0,1)*rDOT,3)*pow(PhiDOT*r + Complex(0,1)*rDOT,5) 
        + pow(mass,3)*r*(pow(PhiDOT,2)*(337510808 - 198882000*pow(Nu,2) + 56294600*pow(Nu,3) + Nu*(183074880 
        - 6392925*pow(M_PI,2)))*pow(r,2) + Complex(0,110)*PhiDOT*(-5498800 - 785120*pow(Nu,2) + 909200*pow(Nu,3) 
        + 3*Nu*(-1849216 + 38745*pow(M_PI,2)))*r*rDOT + 2*(51172744 - 94929000*pow(Nu,2) - 5092400*pow(Nu,3) 
        + 45*Nu*(2794864 + 142065*pow(M_PI,2)))*pow(rDOT,2)) - 20*pow(mass,2)*pow(r,2)*((-986439 + 1873255*Nu 
        - 9961400*pow(Nu,2) + 6704345*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) + Complex(0,4)*(-273687 - 978610*Nu 
        - 4599055*pow(Nu,2) + 2783005*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT + (-181719 + 19395325*Nu 
        + 8237980*pow(Nu,2) + 2612735*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,8)*(-234312 
        + 1541140*Nu + 1230325*pow(Nu,2) + 1828625*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) - 3*(-370268 + 1085140*Nu 
        + 2004715*pow(Nu,2) + 1810425*pow(Nu,3))*pow(rDOT,4)) + 300*mass*pow(r,3)*(4*(12203 - 36427*Nu 
        - 27334*pow(Nu,2) + 149187*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) + Complex(0,2)*(44093 - 68279*Nu 
        - 295346*pow(Nu,2) + 541693*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + 2*(27432 - 202474*Nu 
        + 247505*pow(Nu,2) + 394771*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) + Complex(0,2)*(97069 
        - 383990*Nu - 8741*pow(Nu,2) + 1264800*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) + (-42811 
        + 53992*Nu + 309136*pow(Nu,2) - 470840*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) 
        + Complex(0,2)*(51699 - 252256*Nu + 131150*pow(Nu,2) + 681160*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        - 3*(16743 - 75104*Nu + 26920*pow(Nu,2) + 207200*pow(Nu,3))*pow(rDOT,6))))/(831600.*pow(r,4)*R)) * cpolar(1,-2*Phi);
    }
   
}

static COMPLEX16 hl_2_m_min2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //delta = sqrt(1-4*Nu)
    //vpnorder = x_PNorder *2

    //1PN contribution
    if(vpnorder == 2){
    return ((2*mass*Nu*sqrt(M_PI/5.)*(-21*pow(mass,2)*(-10 + Nu) 
    + 27*(-1 + 3*Nu)*pow(r,2)*pow(PhiDOT*r - Complex(0,1)*rDOT,3)*(PhiDOT*r 
    + Complex(0,1)*rDOT) + mass*r*(-((11 + 156*Nu)*pow(PhiDOT,2)*pow(r,2)) 
    + Complex(0,10)*(5 + 27*Nu)*PhiDOT*r*rDOT + 3*(15 + 32*Nu)*pow(rDOT,2))))/(21.*pow(r,2)*R))*cpolar(1,2*Phi);
    }

    //2PN contribution
    else if(vpnorder == 4){
        return ((mass*Nu*sqrt(M_PI/5.)*(-6*pow(mass,3)*(3028 + 1267*Nu 
        + 158*pow(Nu,2)) - 9*(83 - 589*Nu + 1111*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,4)*pow(PhiDOT*r + Complex(0,1)*rDOT,2) + pow(mass,2)*r*((11891
         + 36575*Nu - 13133*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) + Complex(0,8)*(-773 
         - 3767*Nu + 2852*pow(Nu,2))*PhiDOT*r*rDOT + 6*(-619 + 2789*Nu 
         + 934*pow(Nu,2))*pow(rDOT,2)) + 3*mass*pow(r,2)*(2*(-835 - 19*Nu 
         + 2995*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) - Complex(0,6)*(-433 
         - 721*Nu + 1703*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT + 6*(-33 
         + 1014*Nu + 232*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
         - Complex(0,4)*(-863 + 1462*Nu + 2954*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) 
        - 3*(-557 + 664*Nu + 1712*pow(Nu,2))*pow(rDOT,4))))/(378.*pow(r,3)*R)) * cpolar(1,2*Phi);
    }

    //2.5PN contribution
    else if(vpnorder == 5){
        return ((4*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/5.)*(mass*(Complex(0,-1404)*PhiDOT*r - 2*rDOT) 
        + 3*r*(Complex(0,-316)*pow(PhiDOT,3)*pow(r,3) 
        + 847*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + Complex(0,184)*PhiDOT*r*pow(rDOT,2) + 122*pow(rDOT,3))))/(105.*pow(r,3)*R)) * cpolar(1,2*Phi);
    }
    
    //3PN contribution
    else{
        return (-(mass*Nu*sqrt(M_PI/5.)*(4*pow(mass,4)*(-8203424 + 2180250*pow(Nu,2) 
        + 592600*pow(Nu,3) + 15*Nu*(-5503804 + 142065*pow(M_PI,2))) - 2700*(-507 + 6101*Nu - 25050*pow(Nu,2) 
        + 34525*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r - Complex(0,1)*rDOT,5)*pow(PhiDOT*r + Complex(0,1)*rDOT,3) 
        + pow(mass,3)*r*(pow(PhiDOT,2)*(337510808 - 198882000*pow(Nu,2) + 56294600*pow(Nu,3) + Nu*(183074880 
        - 6392925*pow(M_PI,2)))*pow(r,2) - Complex(0,110)*PhiDOT*(-5498800 - 785120*pow(Nu,2) + 909200*pow(Nu,3) 
        + 3*Nu*(-1849216 + 38745*pow(M_PI,2)))*r*rDOT + 2*(51172744 - 94929000*pow(Nu,2) - 5092400*pow(Nu,3) 
        + 45*Nu*(2794864 + 142065*pow(M_PI,2)))*pow(rDOT,2)) - 20*pow(mass,2)*pow(r,2)*((-986439 + 1873255*Nu 
        - 9961400*pow(Nu,2) + 6704345*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) - Complex(0,4)*(-273687 - 978610*Nu 
        - 4599055*pow(Nu,2) + 2783005*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT + (-181719 + 19395325*Nu 
        + 8237980*pow(Nu,2) + 2612735*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,8)*(-234312 
        + 1541140*Nu + 1230325*pow(Nu,2) + 1828625*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) - 3*(-370268 + 1085140*Nu 
        + 2004715*pow(Nu,2) + 1810425*pow(Nu,3))*pow(rDOT,4)) + 300*mass*pow(r,3)*(4*(12203 - 36427*Nu 
        - 27334*pow(Nu,2) + 149187*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) - Complex(0,2)*(44093 - 68279*Nu 
        - 295346*pow(Nu,2) + 541693*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + 2*(27432 - 202474*Nu 
        + 247505*pow(Nu,2) + 394771*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) - Complex(0,2)*(97069 
        - 383990*Nu - 8741*pow(Nu,2) + 1264800*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) + (-42811 
        + 53992*Nu + 309136*pow(Nu,2) - 470840*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) 
        - Complex(0,2)*(51699 - 252256*Nu + 131150*pow(Nu,2) + 681160*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        - 3*(16743 - 75104*Nu + 26920*pow(Nu,2) + 207200*pow(Nu,3))*pow(rDOT,6))))/(831600.*pow(r,4)*R)) * cpolar(1,2*Phi);
    }
    
}

//H21
static COMPLEX16 hl_2_m_1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);
    //delta = sqrt(1-4*Nu)
    //vpnorder = x_PNorder *2

    //0.5 PN contribution
    if(vpnorder == 1){
        return (-8*delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/5.)*(cos(Phi) - 
        Complex(0,1)*sin(Phi)))/(3.*R);
    }

    //1.5PN contribution
    else if(vpnorder == 3){
        return ((-2*delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/5.)*(4*mass*(-9 + 11*Nu) 
        + r*((19 - 24*Nu)*pow(PhiDOT,2)*pow(r,2) + Complex(0,2)*(83 + 2*Nu)*PhiDOT*r*rDOT 
        + 2*(-33 + 10*Nu)*pow(rDOT,2))))/(21.*r*R))*cpolar(1,-1*Phi);
    }

    //2.5PN contribution
    else if(vpnorder == 5){
        return ((delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/5.)*(-10*pow(mass,2)*(31 - 205*Nu + 111*pow(Nu,2)) 
        + 2*mass*r*((-197 + 5*Nu + 660*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) + Complex(0,1)*(-3167 - 5278*Nu 
        + 201*pow(Nu,2))*PhiDOT*r*rDOT + 8*(202 + 587*Nu - 177*pow(Nu,2))*pow(rDOT,2)) - 3*pow(r,2)*((152 - 692*Nu 
        + 333*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) + Complex(0,2)*(308 - 1607*Nu 
        + 111*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT - 3*(75 - 560*Nu 
        + 68*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,2)*(-265 
        + 526*Nu + 18*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) 
        + (-241 + 550*Nu - 264*pow(Nu,2))*pow(rDOT,4))))/(189.*pow(r,2)*R))*cpolar(1,-1*Phi);
     }

    //3PN contribution
    else{
        return ((4*delta*pow(mass,3)*pow(Nu,2)*PhiDOT*sqrt(M_PI/5.)*(mass*(Complex(0,195)*PhiDOT*r + 946*rDOT) 
        + 9*r*(Complex(0,270)*pow(PhiDOT,3)*pow(r,3) + 483*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - Complex(0,580)*PhiDOT*r*pow(rDOT,2) - 42*pow(rDOT,3))))/(315.*pow(r,2)*R))*cpolar(1,-1*Phi);
    }
    
}

static COMPLEX16 hl_2_m_min1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);
    //delta = sqrt(1-4*Nu)

    //0.5PN contribution
    if(vpnorder == 1){
        return (-8*delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/5.)*(cos(Phi) + Complex(0,1)*sin(Phi)))/(3.*R);
    }
    
    //1.5PN contribution
    else if(vpnorder == 3){
        return ((2*delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/5.)*(4*mass*(9 - 11*Nu) 
        + r*((-19 + 24*Nu)*pow(PhiDOT,2)*pow(r,2) + Complex(0,2)*(83 
        + 2*Nu)*PhiDOT*r*rDOT + 2*(33 - 10*Nu)*pow(rDOT,2))))/(21.*r*R))*cpolar(1,Phi);
    }

    //2.5PN contribution
    else if(vpnorder == 5){
        return ((delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/5.)*(-10*pow(mass,2)*(31 - 205*Nu + 111*pow(Nu,2)) 
        + 2*mass*r*((-197 + 5*Nu + 660*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) - Complex(0,1)*(-3167 - 5278*Nu 
        + 201*pow(Nu,2))*PhiDOT*r*rDOT + 8*(202 + 587*Nu - 177*pow(Nu,2))*pow(rDOT,2)) - 3*pow(r,2)*((152 
        - 692*Nu + 333*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) - Complex(0,2)*(308 - 1607*Nu 
        + 111*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT - 3*(75 - 560*Nu 
        + 68*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,2)*(-265 + 526*Nu 
        + 18*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) + (-241 + 550*Nu - 264*pow(Nu,2))*pow(rDOT,4))))/(189.*pow(r,2)*R))*cpolar(1,Phi);
    }

    //3PN contribution
    else{
        return ((4*delta*pow(mass,3)*pow(Nu,2)*PhiDOT*sqrt(M_PI/5.)*(mass*(Complex(0,-195)*PhiDOT*r 
        + 946*rDOT) + 9*r*(Complex(0,-270)*pow(PhiDOT,3)*pow(r,3) + 483*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + Complex(0,580)*PhiDOT*r*pow(rDOT,2) - 42*pow(rDOT,3))))/(315.*pow(r,2)*R))*cpolar(1,Phi);
    }
}

//H31
static COMPLEX16 hl_3_m_1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);
    //0.5PN contribution
    if(vpnorder == 1){
    return -(delta*mass*Nu*sqrt((2*M_PI)/35.)*(-6*r*(PhiDOT*r - Complex(0,1)*rDOT)*
         pow(PhiDOT*r + Complex(0,1)*rDOT,2) + mass*(7*PhiDOT*r + Complex(0,12)*rDOT))*
      (cos(Phi) - Complex(0,1)*sin(Phi)))/(3.*r*R);
    }

    //1.5PN contribution
    else if(vpnorder == 3){
        return ((delta*mass*Nu*sqrt(M_PI/70.)*(-6*(-5 + 19*Nu)*pow(r,2)*pow(PhiDOT*r - Complex(0,1)*rDOT,2)*pow(PhiDOT*r 
        + Complex(0,1)*rDOT,3) + 2*pow(mass,2)*((101 - 43*Nu)*PhiDOT*r + Complex(0,1)*(109 - 86*Nu)*rDOT) + 3*mass*r*(4*(-9 
        + 14*Nu)*pow(PhiDOT,3)*pow(r,3) + Complex(0,6)*(2 
        + 9*Nu)*pow(PhiDOT,2)*pow(r,2)*rDOT + (33 + 62*Nu)*PhiDOT*r*pow(rDOT,2) + Complex(0,4)*(8
         + 17*Nu)*pow(rDOT,3))))/(9.*pow(r,2)*R))*cpolar(1,-1*Phi);
    }

    //2.5PN contribution
    else if(vpnorder == 5){
        return ((delta*mass*Nu*sqrt(M_PI/70.)*(18*(183 - 1579*Nu + 3387*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,3)*pow(PhiDOT*r + Complex(0,1)*rDOT,4) - 2*pow(mass,3)*((26473 - 27451*Nu 
        + 9921*pow(Nu,2))*PhiDOT*r + Complex(0,12)*(623 - 732*Nu + 1913*pow(Nu,2))*rDOT) 
        + 2*pow(mass,2)*r*((-8641 - 59189*Nu + 31959*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) 
        + Complex(0,1)*(-32635 - 29345*Nu + 29541*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 44*(-256 + 781*Nu + 840*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) + Complex(0,6)*(-756 + 8238*Nu 
        + 7357*pow(Nu,2))*pow(rDOT,3)) - 3*mass*pow(r,2)*(2*(-2479 - 4505*Nu 
        + 16785*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) + Complex(0,4)*(-817 - 1220*Nu 
        + 7449*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + 6*(-1679 + 1469*Nu 
        + 12233*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) + Complex(0,32)*(-460 + 421*Nu 
        + 2514*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) + (-9851 + 17954*Nu
        + 40968*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        + Complex(0,12)*(-771 + 1126*Nu + 3616*pow(Nu,2))*pow(rDOT,5))))/(1188.*pow(r,3)*R))*cpolar(1,-1*Phi);
    }

    //3PN contribution
    else{
        return ((Complex(0,0.022222222222222223)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/70.)*(668*pow(mass,2) 
        - 2*mass*r*(727*pow(PhiDOT,2)*pow(r,2) - Complex(0,99)*PhiDOT*r*rDOT + 452*pow(rDOT,2)) 
        + pow(r,2)*(-499*pow(PhiDOT,4)*pow(r,4) + Complex(0,1534)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + 3072*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,680)*PhiDOT*r*pow(rDOT,3) + 1000*pow(rDOT,4))))/(pow(r,4)*R))*cpolar(1,-1*Phi);
    }

}

static COMPLEX16 hl_3_m_min1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);
    //0.5PN contribution

    if(vpnorder == 1){
    return (delta*mass*Nu*sqrt((2*M_PI)/35.)*(-6*r*pow(PhiDOT*r - Complex(0,1)*rDOT,2)*
        (PhiDOT*r + Complex(0,1)*rDOT) + mass*(7*PhiDOT*r - Complex(0,12)*rDOT))*
     (cos(Phi) + Complex(0,1)*sin(Phi)))/(3.*r*R);
     }

    //1.5PN contribution
    else if(vpnorder == 3){
        return ((delta*mass*Nu*sqrt(M_PI/70.)*(6*(-5 + 19*Nu)*pow(r,2)*pow(PhiDOT*r - Complex(0,1)*rDOT,3)*pow(PhiDOT*r 
        + Complex(0,1)*rDOT,2) + 2*pow(mass,2)*((-101 + 43*Nu)*PhiDOT*r + Complex(0,1)*(109 - 86*Nu)*rDOT) - 3*mass*r*(4*(-9 
        + 14*Nu)*pow(PhiDOT,3)*pow(r,3) - Complex(0,6)*(2 
        + 9*Nu)*pow(PhiDOT,2)*pow(r,2)*rDOT + (33 + 62*Nu)*PhiDOT*r*pow(rDOT,2) 
        - Complex(0,4)*(8 + 17*Nu)*pow(rDOT,3))))/(9.*pow(r,2)*R))*cpolar(1,1*Phi);
    }

    //2.5PN contribution
    else if(vpnorder == 5){
        return ((delta*mass*Nu*sqrt(M_PI/70.)*(-18*(183 - 1579*Nu + 3387*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,4)*pow(PhiDOT*r + Complex(0,1)*rDOT,3) + 2*pow(mass,3)*((26473 - 27451*Nu 
        + 9921*pow(Nu,2))*PhiDOT*r - Complex(0,12)*(623 - 732*Nu + 1913*pow(Nu,2))*rDOT) - 2*pow(mass,2)*r*((-8641 
        - 59189*Nu + 31959*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) - Complex(0,1)*(-32635 - 29345*Nu 
        + 29541*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT + 44*(-256 + 781*Nu + 840*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) 
        - Complex(0,6)*(-756 + 8238*Nu + 7357*pow(Nu,2))*pow(rDOT,3)) + 3*mass*pow(r,2)*(2*(-2479 
        - 4505*Nu + 16785*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) - Complex(0,4)*(-817 - 1220*Nu 
        + 7449*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + 6*(-1679 + 1469*Nu 
        + 12233*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - Complex(0,32)*(-460 
        + 421*Nu + 2514*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) + (-9851 + 17954*Nu 
        + 40968*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        - Complex(0,12)*(-771 + 1126*Nu + 3616*pow(Nu,2))*pow(rDOT,5))))/(1188.*pow(r,3)*R))*cpolar(1,1*Phi);
    }
    //3PN contribution
    else{
        return ((Complex(0,0.022222222222222223)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/70.)*(668*pow(mass,2) 
        - 2*mass*r*(727*pow(PhiDOT,2)*pow(r,2) + Complex(0,99)*PhiDOT*r*rDOT 
        + 452*pow(rDOT,2)) + pow(r,2)*(-499*pow(PhiDOT,4)*pow(r,4) - Complex(0,1534)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + 3072*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,680)*PhiDOT*r*pow(rDOT,3) + 1000*pow(rDOT,4))))/(pow(r,4)*R))*cpolar(1,1*Phi);
    }


}

//H33
static COMPLEX16 hl_3_m_3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);
    //0.5PN contribution
    if(vpnorder==1){
        return -((delta*mass*Nu*sqrt((2*M_PI)/21.)*(2*r*pow(PhiDOT*r + Complex(0,1)*rDOT,3) + 
         mass*(7*PhiDOT*r + Complex(0,4)*rDOT))*(cos(3*Phi) - Complex(0,1)*sin(3*Phi)))/(r*R));
    }

    //1.5PN contribution
    else if(vpnorder==3){
        return ((delta*mass*Nu*sqrt(M_PI/42.)*(6*(-5 + 19*Nu)*pow(r,2)*(PhiDOT*r - Complex(0,1)*rDOT)*pow(PhiDOT*r 
        + Complex(0,1)*rDOT,4) + 2*pow(mass,2)*(3*(101 - 43*Nu)*PhiDOT*r + Complex(0,1)*(109 - 86*Nu)*rDOT) 
        - 3*mass*r*(12*(1 + 4*Nu)*pow(PhiDOT,3)*pow(r,3) + Complex(0,6)*(14 + 31*Nu)*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - 3*(33 + 62*Nu)*PhiDOT*r*pow(rDOT,2) - Complex(0,4)*(8 + 17*Nu)*pow(rDOT,3))))/(9.*pow(r,2)*R))*cpolar(1,-3*Phi);
    }

    //2.5PN contribution
    else if(vpnorder==5){
        return ((delta*mass*Nu*sqrt(M_PI/42.)*(-30*(183 - 1579*Nu + 3387*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,2)*pow(PhiDOT*r + Complex(0,1)*rDOT,5) - 10*pow(mass,3)*((26473 - 27451*Nu 
        + 9921*pow(Nu,2))*PhiDOT*r + Complex(0,4)*(623 - 732*Nu + 1913*pow(Nu,2))*rDOT) 
        - 2*pow(mass,2)*r*(11*(-5353 - 13493*Nu + 4671*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) 
        + Complex(0,1)*(-75243 - 142713*Nu + 192821*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - 220*(-256 + 781*Nu + 840*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) - Complex(0,10)*(-756 + 8238*Nu 
        + 7357*pow(Nu,2))*pow(rDOT,3)) + 3*mass*pow(r,2)*(2*(-7633 + 9137*Nu 
        + 28911*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) + Complex(0,4)*(-8149 + 1576*Nu 
        + 43533*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + 2*(9297 + 19517*Nu 
        - 64839*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) + Complex(0,32)*(-1288 
        + 3667*Nu + 4056*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) - 5*(-9851 + 17954*Nu 
        + 40968*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        - Complex(0,20)*(-771 + 1126*Nu + 3616*pow(Nu,2))*pow(rDOT,5))))/(1980.*pow(r,3)*R))*cpolar(1,-3*Phi);
    }

    //3PN contribution
    else{
        return ((Complex(0,0.022222222222222223)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/42.)*(668*pow(mass,2) 
        + 2*mass*r*(4081*pow(PhiDOT,2)*pow(r,2) + Complex(0,297)*PhiDOT*r*rDOT - 452*pow(rDOT,2)) 
        + 5*pow(r,2)*(1329*pow(PhiDOT,4)*pow(r,4) - Complex(0,2926)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - 384*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,408)*PhiDOT*r*pow(rDOT,3) + 200*pow(rDOT,4))))/(pow(r,4)*R))*cpolar(1,-3*Phi);
    }

}

static COMPLEX16 hl_3_m_min3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);
    //0.5PN contribution
    if(vpnorder == 1){
        return (delta*mass*Nu*sqrt((2*M_PI)/21.)*(2*r*pow(PhiDOT*r - Complex(0,1)*rDOT,3) + 
       mass*(7*PhiDOT*r - Complex(0,4)*rDOT))*(cos(3*Phi) + Complex(0,1)*sin(3*Phi)))/(r*R);
    }

    //1.5PN contribution
    else if(vpnorder==3){
        return ((delta*mass*Nu*sqrt(M_PI/42.)*(-6*(-5 + 19*Nu)*pow(r,2)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,4)*(PhiDOT*r + Complex(0,1)*rDOT) + 2*pow(mass,2)*(3*(-101 + 43*Nu)*PhiDOT*r
         + Complex(0,1)*(109 - 86*Nu)*rDOT) + 3*mass*r*(12*(1 + 4*Nu)*pow(PhiDOT,3)*pow(r,3) - Complex(0,6)*(14 
        + 31*Nu)*pow(PhiDOT,2)*pow(r,2)*rDOT - 3*(33 + 62*Nu)*PhiDOT*r*pow(rDOT,2) 
        + Complex(0,4)*(8 + 17*Nu)*pow(rDOT,3))))/(9.*pow(r,2)*R))*cpolar(1,3*Phi);
    }

    //2.5PN contribution
    else if(vpnorder==5){
        return ((delta*mass*Nu*sqrt(M_PI/42.)*(30*(183 - 1579*Nu + 3387*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,5)*pow(PhiDOT*r + Complex(0,1)*rDOT,2) + 10*pow(mass,3)*((26473 - 27451*Nu 
        + 9921*pow(Nu,2))*PhiDOT*r - Complex(0,4)*(623 - 732*Nu + 1913*pow(Nu,2))*rDOT) + 2*pow(mass,2)*r*(11*(-5353 
        - 13493*Nu + 4671*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) - Complex(0,1)*(-75243 - 142713*Nu 
        + 192821*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT - 220*(-256 + 781*Nu + 840*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) 
        + Complex(0,10)*(-756 + 8238*Nu + 7357*pow(Nu,2))*pow(rDOT,3)) - 3*mass*pow(r,2)*(2*(-7633 + 9137*Nu 
        + 28911*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) - Complex(0,4)*(-8149 + 1576*Nu 
        + 43533*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + 2*(9297 + 19517*Nu 
        - 64839*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - Complex(0,32)*(-1288 + 3667*Nu 
        + 4056*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) - 5*(-9851 + 17954*Nu 
        + 40968*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        + Complex(0,20)*(-771 + 1126*Nu + 3616*pow(Nu,2))*pow(rDOT,5))))/(1980.*pow(r,3)*R))*cpolar(1,3*Phi);
    }

    //3PN contribution
    else{
        return ((Complex(0,0.022222222222222223)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/42.)*(668*pow(mass,2) 
        + 2*mass*r*(4081*pow(PhiDOT,2)*pow(r,2) - Complex(0,297)*PhiDOT*r*rDOT - 452*pow(rDOT,2)) 
        + 5*pow(r,2)*(1329*pow(PhiDOT,4)*pow(r,4) + Complex(0,2926)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - 384*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,408)*PhiDOT*r*pow(rDOT,3) + 200*pow(rDOT,4))))/(pow(r,4)*R))*cpolar(1,3*Phi);
    }
}

//H32
static COMPLEX16 hl_3_m_2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    //1PN contribution
    if(vpnorder == 2){
        return (2*pow(mass,2)*Nu*(-1 + 3*Nu)*PhiDOT*sqrt(M_PI/7.)*(4*PhiDOT*r + 
        Complex(0,1)*rDOT)*(cos(2*Phi) - Complex(0,1)*sin(2*Phi)))/(3.*R);
    }
   
   //2PN contribution
   else if(vpnorder == 4){
       return ((pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/7.)*(2*mass*((167 - 925*Nu 
       + 1615*pow(Nu,2))*PhiDOT*r + Complex(0,5)*(-82 + 239*Nu + 55*pow(Nu,2))*rDOT) 
       - 3*r*(2*(-13 - 25*Nu + 355*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) - Complex(0,60)*(-8 
       + 25*Nu + pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT + 12*(-23 + 70*Nu 
       + 10*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) 
       + Complex(0,5)*(-13 + 38*Nu + 10*pow(Nu,2))*pow(rDOT,3))))/(135.*r*R)) * cpolar(1,-2*Phi);
   }

   //2.5PN contribution
   else if(vpnorder == 5){
       return ((Complex(0,-0.2)*pow(mass,3)*pow(Nu,2)*PhiDOT*sqrt(M_PI/7.)*(7*mass 
       + r*(49*pow(PhiDOT,2)*pow(r,2) - Complex(0,90)*PhiDOT*r*rDOT - 6*pow(rDOT,2))))/(pow(r,2)*R)) * cpolar(1,-2*Phi);
   }

   //3PN contribution
   else{
       return ((pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/7.)*(4*pow(mass,2)*(2*(5377 + 6438*Nu - 79866*pow(Nu,2) 
       + 37348*pow(Nu,3))*PhiDOT*r - Complex(0,5)*(-4115 + 18399*Nu - 20276*pow(Nu,2) + 7*pow(Nu,3))*rDOT) 
       - 4*mass*r*((4599 - 15737*Nu + 36259*pow(Nu,2) + 108563*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3) 
       - Complex(0,1)*(-34053 + 59698*Nu + 192949*pow(Nu,2) + 16193*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*rDOT 
       + (-59058 + 77983*Nu + 322468*pow(Nu,2) - 4264*pow(Nu,3))*PhiDOT*r*pow(rDOT,2) + Complex(0,5)*(-3387 
       + 8518*Nu + 8968*pow(Nu,2) + 884*pow(Nu,3))*pow(rDOT,3)) + 3*pow(r,2)*(4*(-710 + 3892*Nu - 10655*pow(Nu,2) 
       + 24000*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5) + Complex(0,11)*(-1484 + 11693*Nu - 25006*pow(Nu,2) 
       + 428*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*rDOT + 4*(4161 - 25618*Nu + 29489*pow(Nu,2) 
       + 22078*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) + Complex(0,44)*(-151 + 1067*Nu - 2419*pow(Nu,2) 
       + 57*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) + 4*(2041 - 11680*Nu + 19334*pow(Nu,2) 
       + 3368*pow(Nu,3))*PhiDOT*r*pow(rDOT,4) 
       + Complex(0,5)*(477 - 2624*Nu + 3862*pow(Nu,2) + 1160*pow(Nu,3))*pow(rDOT,5))))/(5940.*pow(r,2)*R))* cpolar(1,-2*Phi);
   }
}

static COMPLEX16 hl_3_m_min2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    //1PN contribution
    if(vpnorder == 2){
        return (-2*pow(mass,2)*Nu*(-1 + 3*Nu)*PhiDOT*sqrt(M_PI/7.)*(4*PhiDOT*r - 
        Complex(0,1)*rDOT)*(cos(2*Phi) + Complex(0,1)*sin(2*Phi)))/(3.*R);
    }

    //2PN contribution
    else if(vpnorder == 4){
       return ((pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/7.)*(2*mass*((-167 + 925*Nu 
       - 1615*pow(Nu,2))*PhiDOT*r + Complex(0,5)*(-82 + 239*Nu + 55*pow(Nu,2))*rDOT) 
       + 3*r*(2*(-13 - 25*Nu + 355*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) 
       + Complex(0,60)*(-8 + 25*Nu + pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT 
       + 12*(-23 + 70*Nu + 10*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) 
       - Complex(0,5)*(-13 + 38*Nu + 10*pow(Nu,2))*pow(rDOT,3))))/(135.*r*R)) * cpolar(1,2*Phi);
   }

   //2.5PN contribution
    else if(vpnorder == 5){
       return ((pow(mass,3)*pow(Nu,2)*PhiDOT*sqrt(M_PI/7.)*(Complex(0,-7)*mass 
       + r*(Complex(0,-49)*pow(PhiDOT,2)*pow(r,2) + 90*PhiDOT*r*rDOT 
       + Complex(0,6)*pow(rDOT,2))))/(5.*pow(r,2)*R)) * cpolar(1,2*Phi);
   }

  //3PN contribution
    else{
       return (-(pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/7.)*(4*pow(mass,2)*(2*(5377 + 6438*Nu 
       - 79866*pow(Nu,2) + 37348*pow(Nu,3))*PhiDOT*r + Complex(0,5)*(-4115 + 18399*Nu - 20276*pow(Nu,2) 
       + 7*pow(Nu,3))*rDOT) - 4*mass*r*((4599 - 15737*Nu + 36259*pow(Nu,2) 
       + 108563*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3) + Complex(0,1)*(-34053 + 59698*Nu 
       + 192949*pow(Nu,2) + 16193*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*rDOT + (-59058 
       + 77983*Nu + 322468*pow(Nu,2) - 4264*pow(Nu,3))*PhiDOT*r*pow(rDOT,2) - Complex(0,5)*(-3387 + 8518*Nu 
       + 8968*pow(Nu,2) + 884*pow(Nu,3))*pow(rDOT,3)) + 3*pow(r,2)*(4*(-710 + 3892*Nu - 10655*pow(Nu,2) 
       + 24000*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5) - Complex(0,11)*(-1484 + 11693*Nu - 25006*pow(Nu,2) 
       + 428*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*rDOT + 4*(4161 - 25618*Nu + 29489*pow(Nu,2) 
       + 22078*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - Complex(0,44)*(-151 + 1067*Nu 
       - 2419*pow(Nu,2) + 57*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) + 4*(2041 - 11680*Nu 
       + 19334*pow(Nu,2) + 3368*pow(Nu,3))*PhiDOT*r*pow(rDOT,4) 
       - Complex(0,5)*(477 - 2624*Nu + 3862*pow(Nu,2) + 1160*pow(Nu,3))*pow(rDOT,5))))/(5940.*pow(r,2)*R))* cpolar(1,2*Phi);
   }  
}

//H42
static COMPLEX16 hl_4_m_2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    //1PN contribution
    if(vpnorder == 2){
    return ((2*mass*Nu*(-1 + 3*Nu)*sqrt(M_PI)*(7*pow(mass,2) - 6*pow(r,2)*(PhiDOT*r - Complex(0,1)*rDOT)*pow(PhiDOT*r 
    + Complex(0,1)*rDOT,3) + 3*mass*r*(pow(PhiDOT,2)*pow(r,2) + Complex(0,9)*PhiDOT*r*rDOT - 6*pow(rDOT,2))))/(63.*pow(r,2)*R))*cpolar(1,-2*Phi);
    }
    
    //2PN contribution
    else if(vpnorder == 4){
        return ((mass*Nu*sqrt(M_PI)*(40*pow(mass,3)*(314 - 987*Nu + 195*pow(Nu,2)) + 60*(23 - 159*Nu 
        + 291*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r - Complex(0,1)*rDOT,2)*pow(PhiDOT*r + Complex(0,1)*rDOT,4) 
        + pow(mass,2)*r*((1987 - 11200*Nu + 12960*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) + Complex(0,12)*(967 
        - 4615*Nu + 5935*pow(Nu,2))*PhiDOT*r*rDOT - 10*(290 - 2033*Nu + 4365*pow(Nu,2))*pow(rDOT,2)) 
        - 3*mass*pow(r,2)*((1577 - 7940*Nu + 9920*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) + Complex(0,4)*(-454 
        - 315*Nu + 5980*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT - 2*(549 - 2140*Nu 
        + 2140*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,2)*(-1853 + 1730*Nu 
        + 13230*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) - 20*(-83 + 30*Nu 
        + 762*pow(Nu,2))*pow(rDOT,4))))/(6930.*pow(r,3)*R)) * cpolar(1,-2*Phi);
    }

    //2.5PN contribution
    else if(vpnorder == 5){
        return ((pow(mass,3)*pow(Nu,2)*sqrt(M_PI)*(3*mass*(Complex(0,-43)*PhiDOT*r + 18*rDOT) 
        + r*(Complex(0,73)*pow(PhiDOT,3)*pow(r,3) - 21*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + Complex(0,78)*PhiDOT*r*pow(rDOT,2) - 26*pow(rDOT,3))))/(105.*pow(r,3)*R)) * cpolar(1,-2*Phi);
     }

    //3PN contribution
    else{
        return ((mass*Nu*sqrt(M_PI)*(10*pow(mass,4)*(-4477296 + 12734393*Nu - 6895*pow(Nu,2) + 1043805*pow(Nu,3)) - 3150*(-367 + 4337*Nu - 17462*pow(Nu,2) 
        + 23577*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r - Complex(0,1)*rDOT,3)*pow(PhiDOT*r + Complex(0,1)*rDOT,5) + 2*pow(mass,3)*r*(7*(-100473 + 3430399*Nu - 9132990*pow(Nu,2) 
        + 2885660*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) + Complex(0,2)*(7571073 - 10780154*Nu - 56898800*pow(Nu,2) + 43665510*pow(Nu,3))*PhiDOT*r*rDOT - 10*(1283609 - 5800627*Nu 
        + 3725295*pow(Nu,2) + 4771935*pow(Nu,3))*pow(rDOT,2)) - 7*pow(mass,2)*pow(r,2)*((1071402 + 3846989*Nu - 27339110*pow(Nu,2) 
        + 17538420*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) + Complex(0,12)*(671714 - 1645932*Nu - 1903365*pow(Nu,2) + 3346250*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - (481563 + 4291961*Nu - 17137220*pow(Nu,2) + 9315720*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,12)*(52044 - 1084807*Nu + 1849450*pow(Nu,2) 
        + 4171730*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) - 5*(1083 - 1246819*Nu + 2524240*pow(Nu,2) + 5995845*pow(Nu,3))*pow(rDOT,4)) + 105*mass*pow(r,3)*((54272 - 58271*Nu 
        - 815454*pow(Nu,2) + 1435572*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) + Complex(0,1)*(73976 - 157355*Nu - 811766*pow(Nu,2) 
        + 2935488*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (72637 - 400832*Nu + 282028*pow(Nu,2) + 1063956*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) 
        + Complex(0,4)*(90174 - 385167*Nu - 126419*pow(Nu,2) + 1739072*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) + (-55817 + 58920*Nu + 989942*pow(Nu,2)
         - 2334016*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) + Complex(0,1)*(218975 - 1037408*Nu + 148970*pow(Nu,2) 
        + 3699480*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) + 10*(-10233 + 44864*Nu + 13050*pow(Nu,2) - 203280*pow(Nu,3))*pow(rDOT,6))))/(6.3063e6*pow(r,4)*R))* cpolar(1,-2*Phi);
    }

}

static COMPLEX16 hl_4_m_min2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //1PN contribution
    if(vpnorder == 2){
    return ((2*mass*Nu*(-1 + 3*Nu)*sqrt(M_PI)*(7*pow(mass,2) - 6*pow(r,2)*pow(PhiDOT*r 
    - Complex(0,1)*rDOT,3)*(PhiDOT*r + Complex(0,1)*rDOT) 
    + 3*mass*r*(pow(PhiDOT,2)*pow(r,2) - Complex(0,9)*PhiDOT*r*rDOT - 6*pow(rDOT,2))))/(63.*pow(r,2)*R))*cpolar(1,2*Phi);
    }
    
    //2PN contribution
    else if(vpnorder == 4){
    return ((mass*Nu*sqrt(M_PI)*(40*pow(mass,3)*(314 - 987*Nu + 195*pow(Nu,2)) 
    + 60*(23 - 159*Nu + 291*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r - Complex(0,1)*rDOT,4)*pow(PhiDOT*r 
    + Complex(0,1)*rDOT,2) + pow(mass,2)*r*((1987 - 11200*Nu + 12960*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) 
    - Complex(0,12)*(967 - 4615*Nu + 5935*pow(Nu,2))*PhiDOT*r*rDOT - 10*(290 - 2033*Nu + 4365*pow(Nu,2))*pow(rDOT,2)) 
    - 3*mass*pow(r,2)*((1577 - 7940*Nu + 9920*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) - Complex(0,4)*(-454 - 315*Nu 
    + 5980*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT - 2*(549 - 2140*Nu 
    + 2140*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,2)*(-1853 + 1730*Nu 
    + 13230*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) - 20*(-83 + 30*Nu 
    + 762*pow(Nu,2))*pow(rDOT,4))))/(6930.*pow(r,3)*R)) * cpolar(1,2*Phi);
    }

    //2.5PN contribution
    else if(vpnorder == 5){
        return ((pow(mass,3)*pow(Nu,2)*sqrt(M_PI)*(3*mass*(Complex(0,43)*PhiDOT*r + 18*rDOT) 
        + r*(Complex(0,-73)*pow(PhiDOT,3)*pow(r,3) - 21*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - Complex(0,78)*PhiDOT*r*pow(rDOT,2) - 26*pow(rDOT,3))))/(105.*pow(r,3)*R))* cpolar(1,2*Phi);
    }

    else{
        return ((mass*Nu*sqrt(M_PI)*(10*pow(mass,4)*(-4477296 + 12734393*Nu - 6895*pow(Nu,2) 
        + 1043805*pow(Nu,3)) - 3150*(-367 + 4337*Nu - 17462*pow(Nu,2) + 23577*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,5)*pow(PhiDOT*r + Complex(0,1)*rDOT,3) + 2*pow(mass,3)*r*(7*(-100473 + 3430399*Nu 
        - 9132990*pow(Nu,2) + 2885660*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) - Complex(0,2)*(7571073 - 10780154*Nu 
        - 56898800*pow(Nu,2) + 43665510*pow(Nu,3))*PhiDOT*r*rDOT - 10*(1283609 - 5800627*Nu + 3725295*pow(Nu,2) 
        + 4771935*pow(Nu,3))*pow(rDOT,2)) - 7*pow(mass,2)*pow(r,2)*((1071402 + 3846989*Nu - 27339110*pow(Nu,2) 
        + 17538420*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) - Complex(0,12)*(671714 - 1645932*Nu - 1903365*pow(Nu,2) 
        + 3346250*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT - (481563 + 4291961*Nu - 17137220*pow(Nu,2) 
        + 9315720*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,12)*(52044 - 1084807*Nu 
        + 1849450*pow(Nu,2) + 4171730*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) - 5*(1083 - 1246819*Nu + 2524240*pow(Nu,2)
         + 5995845*pow(Nu,3))*pow(rDOT,4)) + 105*mass*pow(r,3)*((54272 - 58271*Nu - 815454*pow(Nu,2) 
         + 1435572*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) - Complex(0,1)*(73976 - 157355*Nu - 811766*pow(Nu,2)
          + 2935488*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (72637 - 400832*Nu + 282028*pow(Nu,2) 
          + 1063956*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) - Complex(0,4)*(90174 - 385167*Nu 
          - 126419*pow(Nu,2) + 1739072*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) + (-55817 
          + 58920*Nu + 989942*pow(Nu,2) - 2334016*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) 
          - Complex(0,1)*(218975 - 1037408*Nu + 148970*pow(Nu,2) + 3699480*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        + 10*(-10233 + 44864*Nu + 13050*pow(Nu,2) - 203280*pow(Nu,3))*pow(rDOT,6))))/(6.3063e6*pow(r,4)*R))* cpolar(1,2*Phi);
    }
}

//H44
static COMPLEX16 hl_4_m_4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    //1PN
    if(vpnorder == 2){
        return (mass*Nu*(-1 + 3*Nu)*sqrt(M_PI/7.)*(7*pow(mass,2) 
        + 6*pow(r,2)*pow(PhiDOT*r + Complex(0,1)*rDOT,4) 
        + 3*mass*r*(17*pow(PhiDOT,2)*pow(r,2) + Complex(0,18)*PhiDOT*r*rDOT 
        - 6*pow(rDOT,2)))*(cos(4*Phi) - Complex(0,1)*sin(4*Phi)))/(9.*pow(r,2)*R);
    }

    //2PN
    else if(vpnorder == 4){
        return ((mass*Nu*sqrt(M_PI/7.)*(40*pow(mass,3)*(314 - 987*Nu + 195*pow(Nu,2)) - 60*(23 - 159*Nu 
        + 291*pow(Nu,2))*pow(r,3)*(PhiDOT*r - Complex(0,1)*rDOT)*pow(PhiDOT*r + Complex(0,1)*rDOT,5) 
        + pow(mass,2)*r*((53143 - 199660*Nu + 127500*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) + Complex(0,24)*(967 
        - 4615*Nu + 5935*pow(Nu,2))*PhiDOT*r*rDOT - 10*(290 - 2033*Nu + 4365*pow(Nu,2))*pow(rDOT,2)) 
        - 3*mass*pow(r,2)*((613 - 920*Nu + 6420*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) - Complex(0,8)*(-976 
        + 1745*Nu + 3150*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT + 2*(-6141 + 8980*Nu 
        + 31500*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,4)*(-1853 + 1730*Nu 
        + 13230*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) - 20*(-83 + 30*Nu + 762*pow(Nu,2))*pow(rDOT,4))))/(1980.*pow(r,3)*R)) * cpolar(1,-4*Phi);
    }

    //2.5PN
    else if(vpnorder == 5){
        return ((pow(mass,3)*pow(Nu,2)*sqrt(M_PI/7.)*(6*mass*(Complex(0,-43)*PhiDOT*r + 9*rDOT) 
        + r*(Complex(0,-734)*pow(PhiDOT,3)*pow(r,3) + 129*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + Complex(0,156)*PhiDOT*r*pow(rDOT,2) - 26*pow(rDOT,3))))/(30.*pow(r,3)*R)) * cpolar(1,-4*Phi);
    }
    
    //3PN
    else{
        return ((mass*Nu*sqrt(M_PI/7.)*(10*pow(mass,4)*(-4477296 + 12734393*Nu - 6895*pow(Nu,2) 
        + 1043805*pow(Nu,3)) + 3150*(-367 + 4337*Nu - 17462*pow(Nu,2) + 23577*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,2)*pow(PhiDOT*r + Complex(0,1)*rDOT,6) + 2*pow(mass,3)*r*((-36967579 + 245501977*Nu 
        - 459916170*pow(Nu,2) + 150200680*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) + Complex(0,4)*(7571073 - 10780154*Nu 
        - 56898800*pow(Nu,2) + 43665510*pow(Nu,3))*PhiDOT*r*rDOT - 10*(1283609 - 5800627*Nu + 3725295*pow(Nu,2) 
        + 4771935*pow(Nu,3))*pow(rDOT,2)) - pow(mass,2)*pow(r,2)*((-28258134 + 3245207*Nu + 144051250*pow(Nu,2) 
        + 136991820*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) - Complex(0,24)*(2371982 - 7733376*Nu - 7948185*pow(Nu,2) 
        + 9074870*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT + 7*(6557973 - 50558069*Nu + 59901380*pow(Nu,2) 
        + 104752320*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,168)*(52044 - 1084807*Nu 
        + 1849450*pow(Nu,2) + 4171730*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) - 35*(1083 - 1246819*Nu + 2524240*pow(Nu,2) 
        + 5995845*pow(Nu,3))*pow(rDOT,4)) - 105*mass*pow(r,3)*((116396 - 551405*Nu + 560658*pow(Nu,2) 
        + 293036*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) + Complex(0,2)*(158192 - 670661*Nu + 177718*pow(Nu,2) 
        + 2163976*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (-393665 + 1322392*Nu + 1589680*pow(Nu,2) 
        - 8622660*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) - Complex(0,8)*(-23048 + 209397*Nu 
        - 487057*pow(Nu,2) + 260396*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) - (630647 - 3391000*Nu 
        + 2501958*pow(Nu,2) + 7664096*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) - Complex(0,2)*(218975 
        - 1037408*Nu + 148970*pow(Nu,2) + 3699480*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        + 10*(10233 - 44864*Nu - 13050*pow(Nu,2) + 203280*pow(Nu,3))*pow(rDOT,6))))/(1.8018e6*pow(r,4)*R)) * cpolar(1,-4*Phi);
    }
}

static COMPLEX16 hl_4_m_min4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    //1PN
    if(vpnorder == 2){
        return (mass*Nu*(-1 + 3*Nu)*sqrt(M_PI/7.)*(7*pow(mass,2) + 6*pow(r,2)*pow(PhiDOT*r - Complex(0,1)*rDOT,4) 
    + 3*mass*r*(17*pow(PhiDOT,2)*pow(r,2) - Complex(0,18)*PhiDOT*r*rDOT - 6*pow(rDOT,2)))*(cos(4*Phi) 
    + Complex(0,1)*sin(4*Phi)))/(9.*pow(r,2)*R);
     }

    //2PN
    else if(vpnorder == 4){
        return ((mass*Nu*sqrt(M_PI/7.)*(40*pow(mass,3)*(314 - 987*Nu + 195*pow(Nu,2)) 
        - 60*(23 - 159*Nu + 291*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r - Complex(0,1)*rDOT,5)*(PhiDOT*r 
        + Complex(0,1)*rDOT) + pow(mass,2)*r*((53143 - 199660*Nu + 127500*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) 
        - Complex(0,24)*(967 - 4615*Nu + 5935*pow(Nu,2))*PhiDOT*r*rDOT - 10*(290 - 2033*Nu + 4365*pow(Nu,2))*pow(rDOT,2)) 
        - 3*mass*pow(r,2)*((613 - 920*Nu + 6420*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) 
        + Complex(0,8)*(-976 + 1745*Nu + 3150*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + 2*(-6141 + 8980*Nu + 31500*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,4)*(-1853 + 1730*Nu + 13230*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) 
        - 20*(-83 + 30*Nu + 762*pow(Nu,2))*pow(rDOT,4))))/(1980.*pow(r,3)*R)) * cpolar(1,4*Phi);
    }

    //2.5PN
     else if(vpnorder == 5){
        return ((pow(mass,3)*pow(Nu,2)*sqrt(M_PI/7.)*(6*mass*(Complex(0,43)*PhiDOT*r + 9*rDOT) 
        + r*(Complex(0,734)*pow(PhiDOT,3)*pow(r,3) + 129*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - Complex(0,156)*PhiDOT*r*pow(rDOT,2) - 26*pow(rDOT,3))))/(30.*pow(r,3)*R)) * cpolar(1,4*Phi);

    }

    //3PN
    else{
        return ((mass*Nu*sqrt(M_PI/7.)*(10*pow(mass,4)*(-4477296 + 12734393*Nu - 6895*pow(Nu,2) 
        + 1043805*pow(Nu,3)) + 3150*(-367 + 4337*Nu - 17462*pow(Nu,2) + 23577*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,6)*pow(PhiDOT*r + Complex(0,1)*rDOT,2) + 2*pow(mass,3)*r*((-36967579 + 245501977*Nu 
        - 459916170*pow(Nu,2) + 150200680*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) - Complex(0,4)*(7571073 - 10780154*Nu 
        - 56898800*pow(Nu,2) + 43665510*pow(Nu,3))*PhiDOT*r*rDOT - 10*(1283609 - 5800627*Nu + 3725295*pow(Nu,2) 
        + 4771935*pow(Nu,3))*pow(rDOT,2)) + pow(mass,2)*pow(r,2)*(-((-28258134 + 3245207*Nu + 144051250*pow(Nu,2) 
        + 136991820*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)) - Complex(0,24)*(2371982 - 7733376*Nu - 7948185*pow(Nu,2)
         + 9074870*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT - 7*(6557973 - 50558069*Nu + 59901380*pow(Nu,2)
          + 104752320*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,168)*(52044 - 1084807*Nu 
          + 1849450*pow(Nu,2) + 4171730*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) + 35*(1083 - 1246819*Nu + 2524240*pow(Nu,2) 
          + 5995845*pow(Nu,3))*pow(rDOT,4)) - 105*mass*pow(r,3)*((116396 - 551405*Nu + 560658*pow(Nu,2) 
          + 293036*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) - Complex(0,2)*(158192 - 670661*Nu + 177718*pow(Nu,2) 
          + 2163976*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (-393665 + 1322392*Nu + 1589680*pow(Nu,2) 
          - 8622660*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) + Complex(0,8)*(-23048 + 209397*Nu 
          - 487057*pow(Nu,2) + 260396*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) - (630647 - 3391000*Nu 
          + 2501958*pow(Nu,2) + 7664096*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) + Complex(0,2)*(218975 
          - 1037408*Nu + 148970*pow(Nu,2) + 3699480*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) + 10*(10233 - 44864*Nu 
          - 13050*pow(Nu,2) 
        + 203280*pow(Nu,3))*pow(rDOT,6))))/(1.8018e6*pow(r,4)*R))* cpolar(1,4*Phi);
    }
}

//H43
static COMPLEX16 hl_4_m_3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    REAL8 delta = sqrt(1-4*Nu);
    //1.5PN
    if(vpnorder == 3){
        return ((delta*pow(mass,2)*Nu*(-1 
        + 2*Nu)*PhiDOT*sqrt((2*M_PI)/7.)*(4*mass + r*(23*pow(PhiDOT,2)*pow(r,2) 
        + Complex(0,10)*PhiDOT*r*rDOT - 2*pow(rDOT,2))))/(15.*r*R)) * cpolar(1,-3*Phi);
    }
    //2.5PN
    else if(vpnorder == 5){
        return ((delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/14.)*(2*pow(mass,2)*(972 - 2293*Nu + 1398*pow(Nu,2)) 
        + 2*mass*r*((1788 - 9077*Nu + 13416*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) + Complex(0,3)*(-2796 
        + 5299*Nu + 1622*pow(Nu,2))*PhiDOT*r*rDOT - 2*(-1200 + 2545*Nu + 162*pow(Nu,2))*pow(rDOT,2)) 
        - 3*pow(r,2)*((-524 - 489*Nu + 6392*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) + Complex(0,4)*(796 
        - 1864*Nu + 133*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT + 42*(-51 + 94*Nu 
        + 56*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,4)*(-229 
        + 366*Nu + 358*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) 
        - 4*(-43 + 62*Nu + 80*pow(Nu,2))*pow(rDOT,4))))/(990.*pow(r,2)*R))* cpolar(1,-3*Phi);
    }
    //3PN
    else{
        return ((delta*pow(mass,3)*pow(Nu,2)*PhiDOT*sqrt(M_PI/14.)*(6*mass*(Complex(0,-181)*PhiDOT*r 
        - 89*rDOT) + r*(Complex(0,-4847)*pow(PhiDOT,3)*pow(r,3) - 7338*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + Complex(0,408)*PhiDOT*r*pow(rDOT,2) + 112*pow(rDOT,3))))/(225.*pow(r,2)*R)) * cpolar(1,-3*Phi);
    }

}

static COMPLEX16 hl_4_m_min3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    REAL8 delta = sqrt(1-4*Nu);
    //1.5PN
    if(vpnorder == 3){
        return ((delta*pow(mass,2)*Nu*(-1 
        + 2*Nu)*PhiDOT*sqrt((2*M_PI)/7.)*(4*mass + r*(23*pow(PhiDOT,2)*pow(r,2) 
        - Complex(0,10)*PhiDOT*r*rDOT - 2*pow(rDOT,2))))/(15.*r*R)) * cpolar(1,3*Phi);
    }
    //2.5PN
    else if(vpnorder == 5){
        return ((delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/14.)*(2*pow(mass,2)*(972 - 2293*Nu 
        + 1398*pow(Nu,2)) + 2*mass*r*((1788 - 9077*Nu + 13416*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)
         - Complex(0,3)*(-2796 + 5299*Nu + 1622*pow(Nu,2))*PhiDOT*r*rDOT - 2*(-1200 + 2545*Nu 
         + 162*pow(Nu,2))*pow(rDOT,2)) - 3*pow(r,2)*((-524 - 489*Nu + 6392*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)
          - Complex(0,4)*(796 - 1864*Nu + 133*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT + 42*(-51 + 94*Nu 
          + 56*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,4)*(-229 + 366*Nu 
          + 358*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) 
        - 4*(-43 + 62*Nu + 80*pow(Nu,2))*pow(rDOT,4))))/(990.*pow(r,2)*R)) * cpolar(1,3*Phi);
    }
    //3PN
    else{
        return ((delta*pow(mass,3)*pow(Nu,2)*PhiDOT*sqrt(M_PI/14.)*(Complex(0,6)*mass*(181*PhiDOT*r 
        + Complex(0,89)*rDOT) + r*(Complex(0,4847)*pow(PhiDOT,3)*pow(r,3) - 7338*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - Complex(0,408)*PhiDOT*r*pow(rDOT,2) + 112*pow(rDOT,3))))/(225.*pow(r,2)*R)) * cpolar(1,3*Phi);
    }

}

//H41
static COMPLEX16 hl_4_m_1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);
    if(vpnorder == 3){
        return ((delta*pow(mass,2)*Nu*(-1 + 2*Nu)*PhiDOT*sqrt(2*M_PI)*(12*mass + 
                r*(-11*pow(PhiDOT,2)*pow(r,2) + Complex(0,10)*PhiDOT*r*rDOT - 
                6*pow(rDOT,2))))/(105.*r*R))*cpolar(1,-1*Phi);
    }

    else if(vpnorder == 5){
        return ((delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/2.)*(6*pow(mass,2)*(972 - 2293*Nu 
        + 1398*pow(Nu,2)) - 2*mass*r*((-340 - 785*Nu + 6504*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) 
        - Complex(0,3)*(-2796 + 5299*Nu + 1622*pow(Nu,2))*PhiDOT*r*rDOT + 6*(-1200 
        + 2545*Nu + 162*pow(Nu,2))*pow(rDOT,2)) + 3*pow(r,2)*((-540 + 235*Nu 
        + 2648*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) - Complex(0,4)*(-1764 + 3536*Nu 
        + 373*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT + 2*(-723 + 1022*Nu 
        + 1384*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,4)*(-229 + 366*Nu
         + 358*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) 
        + 12*(-43 + 62*Nu + 80*pow(Nu,2))*pow(rDOT,4))))/(6930.*pow(r,2)*R)) * cpolar(1,-1*Phi);
    }

    else{
        return ((delta*pow(mass,3)*pow(Nu,2)*PhiDOT*sqrt(M_PI/2.)*(mass*(Complex(0,-362)*PhiDOT*r - 534*rDOT) 
        + r*(Complex(0,-149)*pow(PhiDOT,3)*pow(r,3) + 182*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + Complex(0,136)*PhiDOT*r*pow(rDOT,2) + 112*pow(rDOT,3))))/(525.*pow(r,2)*R))* cpolar(1,-1*Phi);
    }

    }

static COMPLEX16 hl_4_m_min1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
REAL8 delta = sqrt(1-4*Nu);

if(vpnorder == 3){
    return ((delta*pow(mass,2)*Nu*(-1 + 2*Nu)*PhiDOT*sqrt(2*M_PI)*(12*mass - 
        r*(11*pow(PhiDOT,2)*pow(r,2) + Complex(0,10)*PhiDOT*r*rDOT + 
            6*pow(rDOT,2))))/(105.*r*R)) * cpolar(1,1*Phi);
}

else if(vpnorder == 5){
    return ((delta*pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/2.)*(6*pow(mass,2)*(972 - 2293*Nu 
    + 1398*pow(Nu,2)) - 2*mass*r*((-340 - 785*Nu + 6504*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2) 
    + Complex(0,3)*(-2796 + 5299*Nu + 1622*pow(Nu,2))*PhiDOT*r*rDOT + 6*(-1200 + 2545*Nu 
    + 162*pow(Nu,2))*pow(rDOT,2)) + 3*pow(r,2)*((-540 + 235*Nu 
    + 2648*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4) + Complex(0,4)*(-1764 
    + 3536*Nu + 373*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*rDOT + 2*(-723 
    + 1022*Nu + 1384*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,4)*(-229 + 366*Nu 
    + 358*pow(Nu,2))*PhiDOT*r*pow(rDOT,3) 
    + 12*(-43 + 62*Nu + 80*pow(Nu,2))*pow(rDOT,4))))/(6930.*pow(r,2)*R)) * cpolar(1,1*Phi);
}

//3PN
else{
    return ((delta*pow(mass,3)*pow(Nu,2)*PhiDOT*sqrt(M_PI/2.)*(mass*(Complex(0,362)*PhiDOT*r 
    - 534*rDOT) + r*(Complex(0,149)*pow(PhiDOT,3)*pow(r,3) 
    + 182*pow(PhiDOT,2)*pow(r,2)*rDOT - Complex(0,136)*PhiDOT*r*pow(rDOT,2) 
    + 112*pow(rDOT,3))))/(525.*pow(r,2)*R))* cpolar(1,1*Phi);
}

}

//H55
static COMPLEX16 hl_5_m_5(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 3){
        return ((delta*mass*Nu*(-1 + 2*Nu)*sqrt(M_PI/330.)*(24*pow(r,2)*pow(PhiDOT*r 
        + Complex(0,1)*rDOT,5) + 2*pow(mass,2)*(86*PhiDOT*r + Complex(0,41)*rDOT) 
        + 3*mass*r*(143*pow(PhiDOT,3)*pow(r,3) + Complex(0,208)*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - 132*PhiDOT*r*pow(rDOT,2) - Complex(0,32)*pow(rDOT,3))))/(12.*pow(r,2)*R)) * cpolar(1,-5*Phi);
    }

    if(vpnorder == 5){
        return ((delta*mass*Nu*sqrt(M_PI/330.)*(-360*(33 - 197*Nu + 294*pow(Nu,2))*pow(r,3)*(PhiDOT*r 
        - Complex(0,1)*rDOT)*pow(PhiDOT*r + Complex(0,1)*rDOT,6) + 2*pow(mass,3)*(5*(53311 - 121906*Nu 
        + 42816*pow(Nu,2))*PhiDOT*r + Complex(0,78)*(1141 - 2760*Nu + 1420*pow(Nu,2))*rDOT) 
        + 2*pow(mass,2)*r*(10*(40826 - 125981*Nu + 87534*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) 
        + Complex(0,1)*(112966 - 818425*Nu + 1385970*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 20*(2636 + 11335*Nu - 43962*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) - Complex(0,39)*(-639 - 735*Nu 
        + 5690*pow(Nu,2))*pow(rDOT,3)) - 15*mass*pow(r,2)*(4*(139 - 1687*Nu 
        + 11376*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) + Complex(0,2)*(11276 - 19559*Nu 
        + 5982*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + 3*(-14615 + 12440*Nu 
        + 37132*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) + Complex(0,8)*(-4666 + 139*Nu 
        + 22194*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) - 4*(-3971 - 3226*Nu 
        + 27360*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        - Complex(0,48)*(-57 - 97*Nu + 518*pow(Nu,2))*pow(rDOT,5))))/(4680.*pow(r,3)*R)) * cpolar(1,-5*Phi);
    }

    else{
        return ((Complex(0,-0.002380952380952381)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/330.)*(3566*pow(mass,2) 
        + 6*mass*r*(11305*pow(PhiDOT,2)*pow(r,2) + Complex(0,3921)*PhiDOT*r*rDOT - 906*pow(rDOT,2)) 
        + pow(r,2)*(104681*pow(PhiDOT,4)*pow(r,4) + Complex(0,17192)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - 27840*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,9968)*PhiDOT*r*pow(rDOT,3) + 1424*pow(rDOT,4))))/(pow(r,4)*R)) * cpolar(1,-5*Phi);
    }

}

static COMPLEX16 hl_5_m_min5(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 3){
        return (-(delta*mass*Nu*(-1 + 2*Nu)*sqrt(M_PI/330.)*(24*pow(r,2)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,5) + 2*pow(mass,2)*(86*PhiDOT*r - Complex(0,41)*rDOT) 
        + 3*mass*r*(143*pow(PhiDOT,3)*pow(r,3) - Complex(0,208)*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - 132*PhiDOT*r*pow(rDOT,2) + Complex(0,32)*pow(rDOT,3))))/(12.*pow(r,2)*R)) * cpolar(1,5*Phi);
    }

    if(vpnorder == 5){
        return ((delta*mass*Nu*sqrt(M_PI/330.)*(360*(33 - 197*Nu + 294*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,6)*(PhiDOT*r + Complex(0,1)*rDOT) - 2*pow(mass,3)*(5*(53311 - 121906*Nu 
        + 42816*pow(Nu,2))*PhiDOT*r - Complex(0,78)*(1141 - 2760*Nu + 1420*pow(Nu,2))*rDOT) 
        - 2*pow(mass,2)*r*(10*(40826 - 125981*Nu + 87534*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) 
        - Complex(0,1)*(112966 - 818425*Nu + 1385970*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 20*(2636 + 11335*Nu - 43962*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) + Complex(0,39)*(-639 - 735*Nu 
        + 5690*pow(Nu,2))*pow(rDOT,3)) + 15*mass*pow(r,2)*(4*(139 - 1687*Nu 
        + 11376*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) - Complex(0,2)*(11276 - 19559*Nu 
        + 5982*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + 3*(-14615 + 12440*Nu 
        + 37132*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - Complex(0,8)*(-4666 + 139*Nu 
        + 22194*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) - 4*(-3971 - 3226*Nu 
        + 27360*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        + Complex(0,48)*(-57 - 97*Nu + 518*pow(Nu,2))*pow(rDOT,5))))/(4680.*pow(r,3)*R)) * cpolar(1,5*Phi);

    }

    else{
        return ((Complex(0,-0.002380952380952381)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/330.)*(3566*pow(mass,2) 
        + 6*mass*r*(11305*pow(PhiDOT,2)*pow(r,2) - Complex(0,3921)*PhiDOT*r*rDOT - 906*pow(rDOT,2)) 
        + pow(r,2)*(104681*pow(PhiDOT,4)*pow(r,4) - Complex(0,17192)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - 27840*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) + Complex(0,9968)*PhiDOT*r*pow(rDOT,3) 
        + 1424*pow(rDOT,4))))/(pow(r,4)*R)) * cpolar(1,5*Phi);

    }
}

//H54
static COMPLEX16 hl_5_m_4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
//REAL8 delta = sqrt(1-4*Nu);

    //2PN
    if(vpnorder == 4){
        return -((pow(mass,2)*Nu*(1 - 5*Nu + 5*pow(Nu,2))*PhiDOT*sqrt(M_PI/33.)*(mass*(82*PhiDOT*r 
        + Complex(0,22)*rDOT) + 3*r*(58*pow(PhiDOT,3)*pow(r,3) 
        + Complex(0,33)*pow(PhiDOT,2)*pow(r,2)*rDOT - 12*PhiDOT*r*pow(rDOT,2) 
        - Complex(0,2)*pow(rDOT,3))))/(45.*r*R)) * cpolar(1,-4*Phi);
    }

    //3PN

    else{
        return ((pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/33.)*(-4*pow(mass,2)*(26*(-5051 + 28623*Nu 
        - 46305*pow(Nu,2) + 29470*pow(Nu,3))*PhiDOT*r + Complex(0,7)*(5684 - 26697*Nu + 14225*pow(Nu,2) 
        + 25355*pow(Nu,3))*rDOT) - 4*mass*r*((-149157 + 1133006*Nu - 2731750*pow(Nu,2) 
        + 2085685*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3) + Complex(0,7)*(101118 - 491779*Nu 
        + 402185*pow(Nu,2) + 172105*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*rDOT - (363357 
        - 1825691*Nu + 1714720*pow(Nu,2) + 395255*pow(Nu,3))*PhiDOT*r*pow(rDOT,2) 
        - Complex(0,7)*(9717 - 48896*Nu + 46000*pow(Nu,2) + 10865*pow(Nu,3))*pow(rDOT,3)) 
        + 15*pow(r,2)*(4*(3449 - 6580*Nu - 56728*pow(Nu,2) + 115269*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5) 
        + Complex(0,7)*(-8128 + 45859*Nu - 62702*pow(Nu,2) + 13996*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*rDOT
         + 4*(10125 - 47852*Nu + 26635*pow(Nu,2) + 44240*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) 
         + Complex(0,182)*(127 - 548*Nu + 73*pow(Nu,2) + 816*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) 
         - 8*(1009 - 4060*Nu - 889*pow(Nu,2) + 7952*pow(Nu,3))*PhiDOT*r*pow(rDOT,4) 
        - Complex(0,28)*(45 - 172*Nu - 85*pow(Nu,2) + 400*pow(Nu,3))*pow(rDOT,5))))/(81900.*pow(r,2)*R)) * cpolar(1,-4*Phi);
         
    }

}

static COMPLEX16 hl_5_m_min4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
//REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 4){
        return ((pow(mass,2)*Nu*(1 - 5*Nu + 5*pow(Nu,2))*PhiDOT*sqrt(M_PI/33.)*(mass*(82*PhiDOT*r 
        - Complex(0,22)*rDOT) + 3*r*(58*pow(PhiDOT,3)*pow(r,3) 
        - Complex(0,33)*pow(PhiDOT,2)*pow(r,2)*rDOT - 12*PhiDOT*r*pow(rDOT,2) 
        + Complex(0,2)*pow(rDOT,3))))/(45.*r*R)) * cpolar(1,4*Phi);
    }

    //3PN
    else{
        return ((pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/33.)*(4*pow(mass,2)*(26*(-5051 + 28623*Nu - 46305*pow(Nu,2) 
        + 29470*pow(Nu,3))*PhiDOT*r - Complex(0,7)*(5684 - 26697*Nu + 14225*pow(Nu,2) + 25355*pow(Nu,3))*rDOT) 
        + 4*mass*r*((-149157 + 1133006*Nu - 2731750*pow(Nu,2) + 2085685*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3) 
        - Complex(0,7)*(101118 - 491779*Nu + 402185*pow(Nu,2) + 172105*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - (363357 - 1825691*Nu + 1714720*pow(Nu,2) + 395255*pow(Nu,3))*PhiDOT*r*pow(rDOT,2) + Complex(0,7)*(9717 
        - 48896*Nu + 46000*pow(Nu,2) + 10865*pow(Nu,3))*pow(rDOT,3)) - 15*pow(r,2)*(4*(3449 - 6580*Nu - 56728*pow(Nu,2) 
        + 115269*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5) - Complex(0,7)*(-8128 + 45859*Nu - 62702*pow(Nu,2) 
        + 13996*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*rDOT + 4*(10125 - 47852*Nu + 26635*pow(Nu,2)
         + 44240*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - Complex(0,182)*(127 - 548*Nu + 73*pow(Nu,2) 
         + 816*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) - 8*(1009 - 4060*Nu - 889*pow(Nu,2) 
         + 7952*pow(Nu,3))*PhiDOT*r*pow(rDOT,4) 
        + Complex(0,28)*(45 - 172*Nu - 85*pow(Nu,2) + 400*pow(Nu,3))*pow(rDOT,5))))/(81900.*pow(r,2)*R)) * cpolar(1,4*Phi);
    }
}

//H53
static COMPLEX16 hl_5_m_3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 3){
        return ((delta*mass*Nu*(-1 + 2*Nu)*sqrt(M_PI/66.)*(-120*pow(r,2)*(PhiDOT*r 
        - Complex(0,1)*rDOT)*pow(PhiDOT*r + Complex(0,1)*rDOT,4) + 2*pow(mass,2)*(258*PhiDOT*r 
        + Complex(0,205)*rDOT) - 3*mass*r*(51*pow(PhiDOT,3)*pow(r,3) - Complex(0,240)*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 396*PhiDOT*r*pow(rDOT,2) + Complex(0,160)*pow(rDOT,3))))/(180.*pow(r,2)*R)) * cpolar(1,-3*Phi);
    }

    if(vpnorder == 5){
        return ((delta*mass*Nu*sqrt(M_PI/66.)*(120*(33 - 197*Nu + 294*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,2)*pow(PhiDOT*r + Complex(0,1)*rDOT,5) + 2*pow(mass,3)*((53311 - 121906*Nu 
        + 42816*pow(Nu,2))*PhiDOT*r + Complex(0,26)*(1141 - 2760*Nu + 1420*pow(Nu,2))*rDOT) 
        - 2*pow(mass,2)*r*(2*(6350 - 12803*Nu + 10314*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) 
        - Complex(0,1)*(6546 - 64131*Nu + 109702*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 4*(-2636 - 11335*Nu + 43962*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) + Complex(0,13)*(-639 
        - 735*Nu + 5690*pow(Nu,2))*pow(rDOT,3)) - 3*mass*pow(r,2)*(4*(1223 - 4567*Nu 
        + 3396*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) + Complex(0,26)*(-412 - 437*Nu 
        + 3114*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + (2331 + 31496*Nu 
        - 93276*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) + Complex(0,8)*(-1994 
        + 1541*Nu + 5718*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) - 4*(-3971 
        - 3226*Nu + 27360*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        - Complex(0,80)*(-57 - 97*Nu + 518*pow(Nu,2))*pow(rDOT,5))))/(4680.*pow(r,3)*R)) * cpolar(1,-3*Phi);

    }

    else{
        return ((Complex(0,-0.00015873015873015873)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/66.)*(17830*pow(mass,2) 
        + 18*mass*r*(5231*pow(PhiDOT,2)*pow(r,2) + Complex(0,3921)*PhiDOT*r*rDOT - 1510*pow(rDOT,2)) 
        - pow(r,2)*(48579*pow(PhiDOT,4)*pow(r,4) + Complex(0,31304)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + 33024*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,29904)*PhiDOT*r*pow(rDOT,3) - 7120*pow(rDOT,4))))/(pow(r,4)*R)) * cpolar(1,-3*Phi);

    }
}

static COMPLEX16 hl_5_m_min3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 3){
        return (-(delta*mass*Nu*(-1 + 2*Nu)*sqrt(M_PI/66.)*(-120*pow(r,2)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,4)*(PhiDOT*r + Complex(0,1)*rDOT) + 2*pow(mass,2)*(258*PhiDOT*r 
        - Complex(0,205)*rDOT) - 3*mass*r*(51*pow(PhiDOT,3)*pow(r,3) + Complex(0,240)*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 396*PhiDOT*r*pow(rDOT,2) - Complex(0,160)*pow(rDOT,3))))/(180.*pow(r,2)*R)) * cpolar(1,3*Phi);
    }

    if(vpnorder == 5){
        return ((delta*mass*Nu*sqrt(M_PI/66.)*(-120*(33 - 197*Nu + 294*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,5)*pow(PhiDOT*r + Complex(0,1)*rDOT,2) - 2*pow(mass,3)*((53311 - 121906*Nu 
        + 42816*pow(Nu,2))*PhiDOT*r - Complex(0,26)*(1141 - 2760*Nu + 1420*pow(Nu,2))*rDOT) 
        + 2*pow(mass,2)*r*(2*(6350 - 12803*Nu + 10314*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) 
        + Complex(0,1)*(6546 - 64131*Nu + 109702*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 4*(-2636 - 11335*Nu + 43962*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) - Complex(0,13)*(-639 
        - 735*Nu + 5690*pow(Nu,2))*pow(rDOT,3)) + 3*mass*pow(r,2)*(4*(1223 - 4567*Nu 
        + 3396*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) - Complex(0,26)*(-412 - 437*Nu 
        + 3114*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + (2331 + 31496*Nu 
        - 93276*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - Complex(0,8)*(-1994 
        + 1541*Nu + 5718*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) - 4*(-3971 
        - 3226*Nu + 27360*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        + Complex(0,80)*(-57 - 97*Nu + 518*pow(Nu,2))*pow(rDOT,5))))/(4680.*pow(r,3)*R)) * cpolar(1,3*Phi);
    }

    else{
        return ((Complex(0,-0.00015873015873015873)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/66.)*(17830*pow(mass,2) 
        + 18*mass*r*(5231*pow(PhiDOT,2)*pow(r,2) - Complex(0,3921)*PhiDOT*r*rDOT - 1510*pow(rDOT,2)) 
        + pow(r,2)*(-48579*pow(PhiDOT,4)*pow(r,4) + Complex(0,31304)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - 33024*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,29904)*PhiDOT*r*pow(rDOT,3) + 7120*pow(rDOT,4))))/(pow(r,4)*R))* cpolar(1,3*Phi);
    }

}

//H52
static COMPLEX16 hl_5_m_2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 4){
        return ((-2*pow(mass,2)*Nu*(1 - 5*Nu + 5*pow(Nu,2))*PhiDOT*sqrt(M_PI/11.)*(mass*(41*PhiDOT*r 
        + Complex(0,22)*rDOT) - 3*r*(11*pow(PhiDOT,3)*pow(r,3) 
        - Complex(0,3)*pow(PhiDOT,2)*pow(r,2)*rDOT + 6*PhiDOT*r*pow(rDOT,2) 
        + Complex(0,2)*pow(rDOT,3))))/(135.*r*R))*cpolar(1,-2*Phi);
    }

    else{
        return ((pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/11.)*(-4*pow(mass,2)*(13*(-5051 
        + 28623*Nu - 46305*pow(Nu,2) + 29470*pow(Nu,3))*PhiDOT*r + Complex(0,7)*(5684 - 26697*Nu 
        + 14225*pow(Nu,2) + 25355*pow(Nu,3))*rDOT) + 2*mass*r*((23157 + 154*Nu - 648410*pow(Nu,2) 
        + 1133195*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3) - Complex(0,14)*(6648 - 31729*Nu + 23795*pow(Nu,2) 
        + 13225*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*rDOT + (363357 - 1825691*Nu + 1714720*pow(Nu,2) 
        + 395255*pow(Nu,3))*PhiDOT*r*pow(rDOT,2) + Complex(0,14)*(9717 - 48896*Nu + 46000*pow(Nu,2) 
        + 10865*pow(Nu,3))*pow(rDOT,3)) - 15*pow(r,2)*(2*(2207 - 6076*Nu - 18424*pow(Nu,2) 
        + 38787*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5) - Complex(0,7)*(4744 - 23965*Nu + 23906*pow(Nu,2) 
        + 1892*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*rDOT + 2*(5667 - 22652*Nu - 5747*pow(Nu,2) 
        + 45416*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - Complex(0,14)*(97 - 536*Nu 
        + 643*pow(Nu,2) + 36*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) + 4*(1009 - 4060*Nu 
        - 889*pow(Nu,2) + 7952*pow(Nu,3))*PhiDOT*r*pow(rDOT,4) 
        + Complex(0,28)*(45 - 172*Nu - 85*pow(Nu,2) + 400*pow(Nu,3))*pow(rDOT,5))))/(122850.*pow(r,2)*R)) * cpolar(1,-2*Phi);
    }
}

static COMPLEX16 hl_5_m_min2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 4){
        return ((2*pow(mass,2)*Nu*(1 - 5*Nu + 5*pow(Nu,2))*PhiDOT*sqrt(M_PI/11.)*(mass*(41*PhiDOT*r 
        - Complex(0,22)*rDOT) - 3*r*(11*pow(PhiDOT,3)*pow(r,3) 
        + Complex(0,3)*pow(PhiDOT,2)*pow(r,2)*rDOT + 6*PhiDOT*r*pow(rDOT,2) 
        - Complex(0,2)*pow(rDOT,3))))/(135.*r*R)) * cpolar(1,2*Phi);
    }

    else{
        return ((pow(mass,2)*Nu*PhiDOT*sqrt(M_PI/11.)*(4*pow(mass,2)*(13*(-5051 + 28623*Nu 
        - 46305*pow(Nu,2) + 29470*pow(Nu,3))*PhiDOT*r - Complex(0,7)*(5684 - 26697*Nu + 14225*pow(Nu,2) 
        + 25355*pow(Nu,3))*rDOT) - 2*mass*r*((23157 + 154*Nu - 648410*pow(Nu,2) + 1133195*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3) 
        + Complex(0,14)*(6648 - 31729*Nu + 23795*pow(Nu,2) + 13225*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*rDOT + (363357 
        - 1825691*Nu + 1714720*pow(Nu,2) + 395255*pow(Nu,3))*PhiDOT*r*pow(rDOT,2) - Complex(0,14)*(9717 - 48896*Nu 
        + 46000*pow(Nu,2) + 10865*pow(Nu,3))*pow(rDOT,3)) + 15*pow(r,2)*(2*(2207 - 6076*Nu - 18424*pow(Nu,2) 
        + 38787*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5) + Complex(0,7)*(4744 - 23965*Nu + 23906*pow(Nu,2) 
        + 1892*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*rDOT + 2*(5667 - 22652*Nu - 5747*pow(Nu,2) 
        + 45416*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) + Complex(0,14)*(97 - 536*Nu + 643*pow(Nu,2) 
        + 36*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) + 4*(1009 - 4060*Nu - 889*pow(Nu,2) 
        + 7952*pow(Nu,3))*PhiDOT*r*pow(rDOT,4) 
        - Complex(0,28)*(45 - 172*Nu - 85*pow(Nu,2) + 400*pow(Nu,3))*pow(rDOT,5))))/(122850.*pow(r,2)*R)) * cpolar(1,2*Phi);
    }
}

//H51
static COMPLEX16 hl_5_m_1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 3){
        return ((delta*mass*Nu*(-1 + 2*Nu)*sqrt(M_PI/77.)*(120*pow(r,2)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,2)*pow(PhiDOT*r + Complex(0,1)*rDOT,3) + 2*pow(mass,2)*(86*PhiDOT*r 
        + Complex(0,205)*rDOT) - 3*mass*r*(97*pow(PhiDOT,3)*pow(r,3) + Complex(0,160)*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 132*PhiDOT*r*pow(rDOT,2) + Complex(0,160)*pow(rDOT,3))))/(180.*pow(r,2)*R)) * cpolar(1,-1*Phi);
    }

    if(vpnorder == 5){
        return ((delta*mass*Nu*sqrt(M_PI/77.)*(-360*(33 - 197*Nu + 294*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,3)*pow(PhiDOT*r + Complex(0,1)*rDOT,4) + 2*pow(mass,3)*((53311 - 121906*Nu 
        + 42816*pow(Nu,2))*PhiDOT*r + Complex(0,78)*(1141 - 2760*Nu + 1420*pow(Nu,2))*rDOT) 
        + 2*pow(mass,2)*r*(-2*(29938 - 82195*Nu + 59238*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) 
        - Complex(0,1)*(27026 - 120623*Nu + 199326*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 4*(2636 + 11335*Nu - 43962*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) - Complex(0,39)*(-639 - 735*Nu 
        + 5690*pow(Nu,2))*pow(rDOT,3)) + 3*mass*pow(r,2)*(4*(3115 - 15385*Nu 
        + 22098*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) + Complex(0,2)*(-2108 - 17893*Nu 
        + 56466*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + 3*(-8473 - 9528*Nu 
        + 65204*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) + Complex(0,8)*(-2692 - 6587*Nu 
        + 29754*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) + 4*(-3971 - 3226*Nu 
        + 27360*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        + Complex(0,240)*(-57 - 97*Nu + 518*pow(Nu,2))*pow(rDOT,5))))/(14040.*pow(r,3)*R))* cpolar(1,-1*Phi);
    }

    else{
        return ((Complex(0,-0.00015873015873015873)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/77.)*(17830*pow(mass,2)
        - 6*mass*r*(4723*pow(PhiDOT,2)*pow(r,2) - Complex(0,3921)*PhiDOT*r*rDOT + 4530*pow(rDOT,2)) 
        + pow(r,2)*(12629*pow(PhiDOT,4)*pow(r,4) - Complex(0,24248)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + 20064*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,9968)*PhiDOT*r*pow(rDOT,3) + 7120*pow(rDOT,4))))/(pow(r,4)*R)) * cpolar(1,-1*Phi);
    }

}

static COMPLEX16 hl_5_m_min1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 3){
        return  (-(delta*mass*Nu*(-1 + 2*Nu)*sqrt(M_PI/77.)*(120*pow(r,2)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,3)*pow(PhiDOT*r + Complex(0,1)*rDOT,2) + 2*pow(mass,2)*(86*PhiDOT*r 
        - Complex(0,205)*rDOT) + 3*mass*r*(-97*pow(PhiDOT,3)*pow(r,3) + Complex(0,160)*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - 132*PhiDOT*r*pow(rDOT,2) + Complex(0,160)*pow(rDOT,3))))/(180.*pow(r,2)*R)) * cpolar(1,1*Phi);
    }

    if(vpnorder == 5){
        return ( -(delta*mass*Nu*sqrt(M_PI/77.)*(-360*(33 - 197*Nu + 294*pow(Nu,2))*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,4)*pow(PhiDOT*r + Complex(0,1)*rDOT,3) + 2*pow(mass,3)*((53311 - 121906*Nu 
        + 42816*pow(Nu,2))*PhiDOT*r - Complex(0,78)*(1141 - 2760*Nu + 1420*pow(Nu,2))*rDOT) 
        + 2*pow(mass,2)*r*(-2*(29938 - 82195*Nu + 59238*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3) 
        + Complex(0,1)*(27026 - 120623*Nu + 199326*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*rDOT + 4*(2636 
        + 11335*Nu - 43962*pow(Nu,2))*PhiDOT*r*pow(rDOT,2) + Complex(0,39)*(-639 - 735*Nu 
        + 5690*pow(Nu,2))*pow(rDOT,3)) + 3*mass*pow(r,2)*(4*(3115 - 15385*Nu 
        + 22098*pow(Nu,2))*pow(PhiDOT,5)*pow(r,5) - Complex(0,2)*(-2108 - 17893*Nu 
        + 56466*pow(Nu,2))*pow(PhiDOT,4)*pow(r,4)*rDOT + 3*(-8473 - 9528*Nu 
        + 65204*pow(Nu,2))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - Complex(0,8)*(-2692 - 6587*Nu 
        + 29754*pow(Nu,2))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) + 4*(-3971 - 3226*Nu 
        + 27360*pow(Nu,2))*PhiDOT*r*pow(rDOT,4) 
        - Complex(0,240)*(-57 - 97*Nu + 518*pow(Nu,2))*pow(rDOT,5))))/(14040.*pow(r,3)*R))* cpolar(1,1*Phi);
    }

    //3PN
    else{
        return ((Complex(0,-0.00015873015873015873)*delta*pow(mass,3)*pow(Nu,2)*sqrt(M_PI/77.)*(17830*pow(mass,2) 
        - 6*mass*r*(4723*pow(PhiDOT,2)*pow(r,2) + Complex(0,3921)*PhiDOT*r*rDOT + 4530*pow(rDOT,2)) 
        + pow(r,2)*(12629*pow(PhiDOT,4)*pow(r,4) + Complex(0,24248)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + 20064*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,9968)*PhiDOT*r*pow(rDOT,3) + 7120*pow(rDOT,4))))/(pow(r,4)*R))* cpolar(1,1*Phi);
    }
}

//66
static COMPLEX16 hl_6_m_6(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 4){
        return -((mass*Nu*(1 - 5*Nu + 5*pow(Nu,2))*sqrt(M_PI/715.)*(172*pow(mass,3) + 120*pow(r,3)*pow(PhiDOT*r 
        + Complex(0,1)*rDOT,6) + pow(mass,2)*r*(3269*pow(PhiDOT,2)*pow(r,2) + Complex(0,2920)*PhiDOT*r*rDOT 
        - 806*pow(rDOT,2)) + 15*mass*pow(r,2)*(281*pow(PhiDOT,4)*pow(r,4) + Complex(0,494)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - 444*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,208)*PhiDOT*r*pow(rDOT,3) 
        + 40*pow(rDOT,4))))/(90.*pow(r,3)*R)) * cpolar(1,-6*Phi);
    }

    else{
        return ((mass*Nu*sqrt(M_PI/715.)*(-14*pow(mass,4)*(-5740 + 29361*Nu - 33348*pow(Nu,2) + 7334*pow(Nu,3)) 
        + 7560*(-1 + 9*Nu - 26*pow(Nu,2) + 23*pow(Nu,3))*pow(r,4)*(PhiDOT*r - Complex(0,1)*rDOT)*pow(PhiDOT*r 
        + Complex(0,1)*rDOT,7) - 2*pow(mass,3)*r*((-539645 + 2950311*Nu - 4086684*pow(Nu,2) 
        + 1644517*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) + Complex(0,6)*(-47984 + 275121*Nu - 442540*pow(Nu,2) 
        + 255850*pow(Nu,3))*PhiDOT*r*rDOT + 14*(3614 - 21621*Nu + 39684*pow(Nu,2) - 29332*pow(Nu,3))*pow(rDOT,2)) 
        - 3*pow(mass,2)*pow(r,2)*((-311847 + 1966993*Nu - 3502751*pow(Nu,2) + 1752968*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) 
        + Complex(0,4)*(629 + 160412*Nu - 846370*pow(Nu,2) + 912975*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - 3*(65519 - 144403*Nu - 684796*pow(Nu,2) + 1205253*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,32)*(3921 - 11389*Nu - 27265*pow(Nu,2) + 58450*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) + 14*(1867 
        - 5501*Nu - 12824*pow(Nu,2) + 28137*pow(Nu,3))*pow(rDOT,4)) + 45*mass*pow(r,3)*((195 + 3619*Nu 
        - 36617*pow(Nu,2) + 66836*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) + Complex(0,4)*(-1878 + 10969*Nu 
        - 20741*pow(Nu,2) + 18263*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (17169 - 75446*Nu + 35497*pow(Nu,2) 
        + 47054*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) + Complex(0,2)*(9183 - 30296*Nu - 37835*pow(Nu,2) 
        + 95060*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) - 4*(2781 - 6062*Nu - 28595*pow(Nu,2) 
        + 49070*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) - Complex(0,16)*(228 - 217*Nu - 3871*pow(Nu,2) 
        + 5803*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        + 56*(9 + 4*Nu - 221*pow(Nu,2) + 308*pow(Nu,3))*pow(rDOT,6))))/(3780.*pow(r,4)*R)) * cpolar(1,-6*Phi);
    }
}

static COMPLEX16 hl_6_m_min6(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 4){
        return -((mass*Nu*(1 - 5*Nu + 5*pow(Nu,2))*sqrt(M_PI/715.)*(172*pow(mass,3) 
        + 120*pow(r,3)*pow(PhiDOT*r - Complex(0,1)*rDOT,6) + pow(mass,2)*r*(3269*pow(PhiDOT,2)*pow(r,2) 
        - Complex(0,2920)*PhiDOT*r*rDOT - 806*pow(rDOT,2)) + 15*mass*pow(r,2)*(281*pow(PhiDOT,4)*pow(r,4) 
        - Complex(0,494)*pow(PhiDOT,3)*pow(r,3)*rDOT - 444*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,208)*PhiDOT*r*pow(rDOT,3) + 40*pow(rDOT,4))))/(90.*pow(r,3)*R)) * cpolar(1,6*Phi);
    }

    else{
        return ((mass*Nu*sqrt(M_PI/715.)*(-14*pow(mass,4)*(-5740 + 29361*Nu - 33348*pow(Nu,2) 
        + 7334*pow(Nu,3)) + 7560*(-1 + 9*Nu - 26*pow(Nu,2) + 23*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,7)*(PhiDOT*r + Complex(0,1)*rDOT) - 2*pow(mass,3)*r*((-539645 + 2950311*Nu 
        - 4086684*pow(Nu,2) + 1644517*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) - Complex(0,6)*(-47984 
        + 275121*Nu - 442540*pow(Nu,2) + 255850*pow(Nu,3))*PhiDOT*r*rDOT + 14*(3614 - 21621*Nu 
        + 39684*pow(Nu,2) - 29332*pow(Nu,3))*pow(rDOT,2)) - 3*pow(mass,2)*pow(r,2)*((-311847 
        + 1966993*Nu - 3502751*pow(Nu,2) + 1752968*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) 
        - Complex(0,4)*(629 + 160412*Nu - 846370*pow(Nu,2) + 912975*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - 3*(65519 - 144403*Nu - 684796*pow(Nu,2) + 1205253*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,32)*(3921 - 11389*Nu - 27265*pow(Nu,2) + 58450*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) + 14*(1867 
        - 5501*Nu - 12824*pow(Nu,2) + 28137*pow(Nu,3))*pow(rDOT,4)) + 45*mass*pow(r,3)*((195 + 3619*Nu 
        - 36617*pow(Nu,2) + 66836*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) - Complex(0,4)*(-1878 + 10969*Nu 
        - 20741*pow(Nu,2) + 18263*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (17169 - 75446*Nu + 35497*pow(Nu,2) 
        + 47054*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) - Complex(0,2)*(9183 - 30296*Nu - 37835*pow(Nu,2) 
        + 95060*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) - 4*(2781 - 6062*Nu - 28595*pow(Nu,2) 
        + 49070*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) + Complex(0,16)*(228 - 217*Nu - 3871*pow(Nu,2) 
        + 5803*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        + 56*(9 + 4*Nu - 221*pow(Nu,2) + 308*pow(Nu,3))*pow(rDOT,6))))/(3780.*pow(r,4)*R)) * cpolar(1,6*Phi);
    }
}

//65
static COMPLEX16 hGO_6_m_5(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 5){
        return (Complex(0,0.003968253968253968)*delta*mass*(1 - 4*Nu
         + 3*pow(Nu,2))*PhiDOT*(82*pow(mass,2) 
        + 2*mass*r*(701*pow(PhiDOT,2)*pow(r,2) 
        + Complex(0,343)*PhiDOT*r*rDOT - 62*pow(rDOT,2)) 
        + 3*pow(r,2)*(547*pow(PhiDOT,4)*pow(r,4) + Complex(0,364)*pow(PhiDOT,3)*pow(r,3)*rDOT
         - 180*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,56)*PhiDOT*r*pow(rDOT,3) 
        + 8*pow(rDOT,4))))/(sqrt(429)*pow(r,2));
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_6_m_5(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){


    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,5*M_PI/2) * hGO_6_m_5(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-5*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_6_m_min5(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-5*M_PI/2)* pow(-1,6) *  conj(hGO_6_m_5(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,5*Phi);
    }
    
    else{
        return 0;
    }
}


//64
static COMPLEX16 hl_6_m_4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 4){
        return (-(mass*Nu*(1 - 5*Nu + 5*pow(Nu,2))*sqrt(M_PI/390.)*(516*pow(mass,3) 
        - 360*pow(r,3)*(PhiDOT*r - Complex(0,1)*rDOT)*pow(PhiDOT*r + Complex(0,1)*rDOT,5) 
        + pow(mass,2)*r*(3587*pow(PhiDOT,2)*pow(r,2) + Complex(0,5840)*PhiDOT*r*rDOT 
        - 2418*pow(rDOT,2)) - 15*mass*pow(r,2)*(113*pow(PhiDOT,4)*pow(r,4) 
        - Complex(0,108)*pow(PhiDOT,3)*pow(r,3)*rDOT + 468*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,416)*PhiDOT*r*pow(rDOT,3) - 120*pow(rDOT,4))))/(495.*pow(r,3)*R)) * cpolar(1,-4*Phi);
    }

    else{
        return (-(mass*Nu*sqrt(M_PI/390.)*(14*pow(mass,4)*(-5740 + 29361*Nu - 33348*pow(Nu,2) + 7334*pow(Nu,3)) 
        + 7560*(-1 + 9*Nu - 26*pow(Nu,2) + 23*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r - Complex(0,1)*rDOT,2)*pow(PhiDOT*r 
        + Complex(0,1)*rDOT,6) + 2*pow(mass,3)*r*((-196625 + 1082991*Nu - 1522164*pow(Nu,2) 
        + 618457*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) + Complex(0,4)*(-47984 + 275121*Nu - 442540*pow(Nu,2) 
        + 255850*pow(Nu,3))*PhiDOT*r*rDOT + 14*(3614 - 21621*Nu + 39684*pow(Nu,2) - 29332*pow(Nu,3))*pow(rDOT,2)) 
        + pow(mass,2)*pow(r,2)*((133599 - 779681*Nu + 1417087*pow(Nu,2) - 1130416*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) 
        + Complex(0,8)*(3849 + 4172*Nu - 80290*pow(Nu,2) + 64435*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + (-226971 + 551047*Nu + 2049124*pow(Nu,2) - 3713857*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,64)*(3921 - 11389*Nu - 27265*pow(Nu,2) + 58450*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) + 42*(1867 
        - 5501*Nu - 12824*pow(Nu,2) + 28137*pow(Nu,3))*pow(rDOT,4)) + 15*mass*pow(r,3)*((2267 - 12733*Nu 
        + 13895*pow(Nu,2) + 6300*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) - Complex(0,8)*(908 - 2597*Nu - 5873*pow(Nu,2) 
        + 11809*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (5241 + 10066*Nu - 173159*pow(Nu,2) 
        + 235382*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) + Complex(0,4)*(-1651 + 11312*Nu - 25417*pow(Nu,2) 
        + 20916*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) + 4*(3127 - 8386*Nu - 23569*pow(Nu,2) 
        + 45122*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) + Complex(0,32)*(228 - 217*Nu - 3871*pow(Nu,2) 
        + 5803*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        - 168*(9 + 4*Nu - 221*pow(Nu,2) + 308*pow(Nu,3))*pow(rDOT,6))))/(6930.*pow(r,4)*R)) * cpolar(1,-4*Phi);
    }
}

static COMPLEX16 hl_6_m_min4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 4){
        return (-(mass*Nu*(1 - 5*Nu + 5*pow(Nu,2))*sqrt(M_PI/390.)*(516*pow(mass,3) 
        - 360*pow(r,3)*pow(PhiDOT*r - Complex(0,1)*rDOT,5)*(PhiDOT*r + Complex(0,1)*rDOT) 
        + pow(mass,2)*r*(3587*pow(PhiDOT,2)*pow(r,2) - Complex(0,5840)*PhiDOT*r*rDOT 
        - 2418*pow(rDOT,2)) - 15*mass*pow(r,2)*(113*pow(PhiDOT,4)*pow(r,4) 
        + Complex(0,108)*pow(PhiDOT,3)*pow(r,3)*rDOT + 468*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,416)*PhiDOT*r*pow(rDOT,3) - 120*pow(rDOT,4))))/(495.*pow(r,3)*R)) * cpolar(1,4*Phi);
    }

    else{
        return ((mass*Nu*sqrt(M_PI/390.)*(-14*pow(mass,4)*(-5740 + 29361*Nu - 33348*pow(Nu,2) 
        + 7334*pow(Nu,3)) - 7560*(-1 + 9*Nu - 26*pow(Nu,2) + 23*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,6)*pow(PhiDOT*r + Complex(0,1)*rDOT,2) - 2*pow(mass,3)*r*((-196625 + 1082991*Nu 
        - 1522164*pow(Nu,2) + 618457*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) - Complex(0,4)*(-47984 + 275121*Nu 
        - 442540*pow(Nu,2) + 255850*pow(Nu,3))*PhiDOT*r*rDOT + 14*(3614 - 21621*Nu + 39684*pow(Nu,2) 
        - 29332*pow(Nu,3))*pow(rDOT,2)) + pow(mass,2)*pow(r,2)*((-133599 + 779681*Nu - 1417087*pow(Nu,2) 
        + 1130416*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) + Complex(0,8)*(3849 + 4172*Nu - 80290*pow(Nu,2) 
        + 64435*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT + (226971 - 551047*Nu - 2049124*pow(Nu,2) 
        + 3713857*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,64)*(3921 - 11389*Nu 
        - 27265*pow(Nu,2) + 58450*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) - 42*(1867 - 5501*Nu - 12824*pow(Nu,2) 
        + 28137*pow(Nu,3))*pow(rDOT,4)) - 15*mass*pow(r,3)*((2267 - 12733*Nu + 13895*pow(Nu,2) 
        + 6300*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) + Complex(0,8)*(908 - 2597*Nu - 5873*pow(Nu,2) 
        + 11809*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (5241 + 10066*Nu - 173159*pow(Nu,2) 
        + 235382*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) - Complex(0,4)*(-1651 + 11312*Nu 
        - 25417*pow(Nu,2) + 20916*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) + 4*(3127 
        - 8386*Nu - 23569*pow(Nu,2) + 45122*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) 
        - Complex(0,32)*(228 - 217*Nu - 3871*pow(Nu,2) + 5803*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        - 168*(9 + 4*Nu - 221*pow(Nu,2) + 308*pow(Nu,3))*pow(rDOT,6))))/(6930.*pow(r,4)*R)) * cpolar(1,4*Phi);
    }
}

//63
static COMPLEX16 hGO_6_m_3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 5){
        return(Complex(0,-0.00036075036075036075)*delta*mass*(1 - 4*Nu 
        + 3*pow(Nu,2))*PhiDOT*(410*pow(mass,2) + 2*mass*r*(929*pow(PhiDOT,2)*pow(r,2) 
        + Complex(0,1029)*PhiDOT*r*rDOT - 310*pow(rDOT,2)) - 3*pow(r,2)*(513*pow(PhiDOT,4)*pow(r,4) 
        + Complex(0,28)*pow(PhiDOT,3)*pow(r,3)*rDOT + 228*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,168)*PhiDOT*r*pow(rDOT,3) - 40*pow(rDOT,4))))/(sqrt(65)*pow(r,2));
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_6_m_3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){


    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,3*M_PI/2) * hGO_6_m_3(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-3*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_6_m_min3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-3*M_PI/2)* pow(-1,6) * conj(hGO_6_m_3(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,3*Phi);
    }
    
    else{
        return 0;
    }
}


//62
static COMPLEX16 hl_6_m_2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 4){
        return (-(mass*Nu*(1 - 5*Nu + 5*pow(Nu,2))*sqrt(M_PI/13.)*(516*pow(mass,3) 
        + 360*pow(r,3)*pow(PhiDOT*r - Complex(0,1)*rDOT,2)*pow(PhiDOT*r + Complex(0,1)*rDOT,4) 
        + pow(mass,2)*r*(-145*pow(PhiDOT,2)*pow(r,2) + Complex(0,2920)*PhiDOT*r*rDOT - 2418*pow(rDOT,2)) 
        - 3*mass*pow(r,2)*(233*pow(PhiDOT,4)*pow(r,4) + Complex(0,1050)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        - 252*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,1040)*PhiDOT*r*pow(rDOT,3) - 600*pow(rDOT,4))))/(2970.*pow(r,3)*R)) * cpolar(1,-2*Phi);
    }

    else{
        return (-(mass*Nu*sqrt(M_PI/13.)*(14*pow(mass,4)*(-5740 + 29361*Nu - 33348*pow(Nu,2) + 7334*pow(Nu,3)) 
        - 7560*(-1 + 9*Nu - 26*pow(Nu,2) + 23*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r - Complex(0,1)*rDOT,3)*pow(PhiDOT*r 
        + Complex(0,1)*rDOT,5) + 2*pow(mass,3)*r*((9187 - 37401*Nu + 16548*pow(Nu,2) + 2821*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) 
        + Complex(0,2)*(-47984 + 275121*Nu - 442540*pow(Nu,2) + 255850*pow(Nu,3))*PhiDOT*r*rDOT + 14*(3614 - 21621*Nu 
        + 39684*pow(Nu,2) - 29332*pow(Nu,3))*pow(rDOT,2)) + pow(mass,2)*pow(r,2)*((54699 - 336749*Nu + 596995*pow(Nu,2) 
        - 337960*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) - Complex(0,4)*(-5781 + 89572*Nu - 379358*pow(Nu,2) 
        + 444689*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT + (-9351 + 101899*Nu - 419300*pow(Nu,2) 
        + 566195*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,32)*(3921 - 11389*Nu 
        - 27265*pow(Nu,2) + 58450*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) + 42*(1867 - 5501*Nu - 12824*pow(Nu,2) 
        + 28137*pow(Nu,3))*pow(rDOT,4)) + 3*mass*pow(r,3)*((-7885 + 64211*Nu - 170905*pow(Nu,2) 
        + 146580*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) + Complex(0,4)*(1438 + 9779*Nu - 86023*pow(Nu,2) 
        + 109949*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (16353 - 68054*Nu + 10297*pow(Nu,2) 
        + 77294*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) + Complex(0,2)*(14341 - 392*Nu - 316841*pow(Nu,2) 
        + 452508*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) - 4*(13 + 12530*Nu - 68803*pow(Nu,2) 
        + 80654*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) + Complex(0,80)*(228 - 217*Nu - 3871*pow(Nu,2) 
        + 5803*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        - 840*(9 + 4*Nu - 221*pow(Nu,2) + 308*pow(Nu,3))*pow(rDOT,6))))/(41580.*pow(r,4)*R)) * cpolar(1,-2*Phi);
    }
}

static COMPLEX16 hl_6_m_min2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 4){
        return (-(mass*Nu*(1 - 5*Nu + 5*pow(Nu,2))*sqrt(M_PI/13.)*(516*pow(mass,3) 
        + 360*pow(r,3)*pow(PhiDOT*r - Complex(0,1)*rDOT,4)*pow(PhiDOT*r + Complex(0,1)*rDOT,2) 
        - pow(mass,2)*r*(145*pow(PhiDOT,2)*pow(r,2) + Complex(0,2920)*PhiDOT*r*rDOT + 2418*pow(rDOT,2)) 
        + 3*mass*pow(r,2)*(-233*pow(PhiDOT,4)*pow(r,4) + Complex(0,1050)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + 252*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,1040)*PhiDOT*r*pow(rDOT,3) + 600*pow(rDOT,4))))/(2970.*pow(r,3)*R)) * cpolar(1,2*Phi);
    }

    else{
        return ((mass*Nu*sqrt(M_PI/13.)*(-14*pow(mass,4)*(-5740 + 29361*Nu - 33348*pow(Nu,2) 
        + 7334*pow(Nu,3)) + 7560*(-1 + 9*Nu - 26*pow(Nu,2) + 23*pow(Nu,3))*pow(r,4)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,5)*pow(PhiDOT*r + Complex(0,1)*rDOT,3) - 2*pow(mass,3)*r*((9187 - 37401*Nu 
        + 16548*pow(Nu,2) + 2821*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2) - Complex(0,2)*(-47984 + 275121*Nu 
        - 442540*pow(Nu,2) + 255850*pow(Nu,3))*PhiDOT*r*rDOT + 14*(3614 - 21621*Nu + 39684*pow(Nu,2) 
        - 29332*pow(Nu,3))*pow(rDOT,2)) + pow(mass,2)*pow(r,2)*((-54699 + 336749*Nu - 596995*pow(Nu,2) 
        + 337960*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4) - Complex(0,4)*(-5781 + 89572*Nu - 379358*pow(Nu,2) 
        + 444689*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*rDOT + (9351 - 101899*Nu + 419300*pow(Nu,2) 
        - 566195*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) - Complex(0,32)*(3921 - 11389*Nu 
        - 27265*pow(Nu,2) + 58450*pow(Nu,3))*PhiDOT*r*pow(rDOT,3) - 42*(1867 - 5501*Nu - 12824*pow(Nu,2) 
        + 28137*pow(Nu,3))*pow(rDOT,4)) - 3*mass*pow(r,3)*((-7885 + 64211*Nu - 170905*pow(Nu,2) 
        + 146580*pow(Nu,3))*pow(PhiDOT,6)*pow(r,6) - Complex(0,4)*(1438 + 9779*Nu - 86023*pow(Nu,2) 
        + 109949*pow(Nu,3))*pow(PhiDOT,5)*pow(r,5)*rDOT + (16353 - 68054*Nu + 10297*pow(Nu,2) 
        + 77294*pow(Nu,3))*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) - Complex(0,2)*(14341 - 392*Nu 
        - 316841*pow(Nu,2) + 452508*pow(Nu,3))*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) - 4*(13 
        + 12530*Nu - 68803*pow(Nu,2) + 80654*pow(Nu,3))*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) 
        - Complex(0,80)*(228 - 217*Nu - 3871*pow(Nu,2) + 5803*pow(Nu,3))*PhiDOT*r*pow(rDOT,5) 
        - 840*(9 + 4*Nu - 221*pow(Nu,2) + 308*pow(Nu,3))*pow(rDOT,6))))/(41580.*pow(r,4)*R)) * cpolar(1,2*Phi);
    }
}

//61
static COMPLEX16 hGO_6_m_1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 5){
        return (Complex(0,0.0002405002405002405)*delta*mass*(1 - 4*Nu + 3*pow(Nu,2))*PhiDOT*(410*pow(mass,2) 
        - 2*mass*r*(359*pow(PhiDOT,2)*pow(r,2) - Complex(0,343)*PhiDOT*r*rDOT + 310*pow(rDOT,2)) 
        + 3*pow(r,2)*(103*pow(PhiDOT,4)*pow(r,4) - Complex(0,196)*pow(PhiDOT,3)*pow(r,3)*rDOT 
        + 108*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,56)*PhiDOT*r*pow(rDOT,3) + 40*pow(rDOT,4))))/(sqrt(26)*pow(r,2));
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_6_m_1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){


    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,1*M_PI/2) * hGO_6_m_1(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-1*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_6_m_min1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-1*M_PI/2)* pow(-1,6) * conj(hGO_6_m_1(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,1*Phi);
    }
    
    else{
        return 0;
    }
}

//77
static COMPLEX16 hGO_7_m_7(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 5){
        return (delta*(1 - 4*Nu + 3*pow(Nu,2))*(720*pow(r,3)*pow(Complex(0,1)*PhiDOT*r - rDOT,7) 
        + 2*pow(mass,3)*(Complex(0,-4559)*PhiDOT*r + 1976*rDOT) 
        + 18*pow(mass,2)*r*(Complex(0,-3317)*pow(PhiDOT,3)*pow(r,3) + 4143*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + Complex(0,2178)*PhiDOT*r*pow(rDOT,2) - 442*pow(rDOT,3)) 
        + 45*mass*pow(r,2)*(Complex(0,-1069)*pow(PhiDOT,5)*pow(r,5) + 2112*pow(PhiDOT,4)*pow(r,4)*rDOT 
        + Complex(0,2370)*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - 1600*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) 
        - Complex(0,600)*PhiDOT*r*pow(rDOT,4) + 96*pow(rDOT,5))))/(720.*sqrt(6006)*pow(r,3));
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_7(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){


    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,7*M_PI/2) * hGO_7_m_7(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-7*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_min7(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-7*M_PI/2)* pow(-1,7) * conj(hGO_7_m_7(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,7*Phi);
    }
    
    else{
        return 0;
    }
}

//75
static COMPLEX16 hGO_7_m_5(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 5){
        return (delta*(1 - 4*Nu + 3*pow(Nu,2))*(720*pow(r,3)*pow(Complex(0,1)*PhiDOT*r - rDOT,7) 
        + 2*pow(mass,3)*(Complex(0,-4559)*PhiDOT*r + 1976*rDOT) 
        + 18*pow(mass,2)*r*(Complex(0,-3317)*pow(PhiDOT,3)*pow(r,3) + 4143*pow(PhiDOT,2)*pow(r,2)*rDOT
         + Complex(0,2178)*PhiDOT*r*pow(rDOT,2) - 442*pow(rDOT,3)) 
         + 45*mass*pow(r,2)*(Complex(0,-1069)*pow(PhiDOT,5)*pow(r,5) 
         + 2112*pow(PhiDOT,4)*pow(r,4)*rDOT + Complex(0,2370)*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) 
         - 1600*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) 
        - Complex(0,600)*PhiDOT*r*pow(rDOT,4) + 96*pow(rDOT,5))))/(720.*sqrt(6006)*pow(r,3));
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_5(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){


    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,5*M_PI/2) * hGO_7_m_5(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-5*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_min5(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-5*M_PI/2)* pow(-1,7) * conj(hGO_7_m_5(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,5*Phi);
    }
    
    else{
        return 0;
    }
}

//73
static COMPLEX16 hGO_7_m_3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 5){
        return (delta*(1 - 4*Nu + 3*pow(Nu,2))*(5040*pow(r,3)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,2)*pow(Complex(0,-1)*PhiDOT*r + rDOT,5) + 2*pow(mass,3)*(Complex(0,-13677)*PhiDOT*r 
        + 13832*rDOT) + 18*pow(mass,2)*r*(Complex(0,1529)*pow(PhiDOT,3)*pow(r,3) + 2401*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + Complex(0,6534)*PhiDOT*r*pow(rDOT,2) - 3094*pow(rDOT,3)) 
        + 15*mass*pow(r,2)*(Complex(0,179)*pow(PhiDOT,5)*pow(r,5) - 4368*pow(PhiDOT,4)*pow(r,4)*rDOT 
        - Complex(0,4878)*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) - 2240*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) 
        - Complex(0,5400)*PhiDOT*r*pow(rDOT,4) + 2016*pow(rDOT,5))))/(240240.*sqrt(6)*pow(r,3));
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){


    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,3*M_PI/2) * hGO_7_m_3(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-3*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_min3(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-3*M_PI/2)* pow(-1,7) * conj(hGO_7_m_3(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,3*Phi);
    }
    
    else{
        return 0;
    }
}

//71

static COMPLEX16 hGO_7_m_1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 5){
        return (delta*(1 - 4*Nu + 3*pow(Nu,2))*(2*pow(mass,3)*(Complex(0,4559)*PhiDOT*r - 13832*rDOT) 
        - Complex(0,5040)*pow(r,3)*pow(PhiDOT*r - Complex(0,1)*rDOT,3)*pow(PhiDOT*r + Complex(0,1)*rDOT,4) 
        + 18*pow(mass,2)*r*(Complex(0,-1275)*pow(PhiDOT,3)*pow(r,3) + 2919*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - Complex(0,2178)*PhiDOT*r*pow(rDOT,2) + 3094*pow(rDOT,3)) 
        + Complex(0,27)*mass*pow(r,2)*(699*pow(PhiDOT,5)*pow(r,5) + Complex(0,1120)*pow(PhiDOT,4)*pow(r,4)*rDOT 
        + 1874*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) + Complex(0,2240)*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) 
        + 1000*PhiDOT*r*pow(rDOT,4) 
        + Complex(0,1120)*pow(rDOT,5))))/(432432.*sqrt(2)*pow(r,3));}

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,1*M_PI/2) * hGO_7_m_1(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-1*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_min1(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-1*M_PI/2)* pow(-1,7) * conj(hGO_7_m_1(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,1*Phi);
    }
    
    else{
        return 0;
    }
}

//72

static COMPLEX16 hGO_7_m_2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 6){
        return (-(mass*(-1 + 7*Nu - 14*pow(Nu,2) + 7*pow(Nu,3))*PhiDOT*(8*pow(mass,2)*(494*PhiDOT*r 
        + Complex(0,411)*rDOT) - 12*mass*r*(530*pow(PhiDOT,3)*pow(r,3) - Complex(0,6)*pow(PhiDOT,2)*pow(r,2)*rDOT 
        + 453*PhiDOT*r*pow(rDOT,2) + Complex(0,197)*pow(rDOT,3)) + 3*pow(r,2)*(824*pow(PhiDOT,5)*pow(r,5) 
        - Complex(0,671)*pow(PhiDOT,4)*pow(r,4)*rDOT + 864*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) 
        + Complex(0,44)*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) 
        + 320*PhiDOT*r*pow(rDOT,4) + Complex(0,120)*pow(rDOT,5))))/(96096.*sqrt(3)*pow(r,2)));}

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,2*M_PI/2) * hGO_7_m_2(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-2*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_min2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-2*M_PI/2)* pow(-1,7) * conj(hGO_7_m_2(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,2*Phi);
    }
    
    else{
        return 0;
    }
}

//74

static COMPLEX16 hGO_7_m_4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 6){
        return ((mass*(-1 + 7*Nu - 14*pow(Nu,2) + 7*pow(Nu,3))*PhiDOT*(8*pow(mass,2)*(988*PhiDOT*r 
        + Complex(0,411)*rDOT) + 12*mass*r*(844*pow(PhiDOT,3)*pow(r,3) + Complex(0,1518)*pow(PhiDOT,2)*pow(r,2)*rDOT 
        - 906*PhiDOT*r*pow(rDOT,2) - Complex(0,197)*pow(rDOT,3)) - 15*pow(r,2)*(656*pow(PhiDOT,5)*pow(r,5) 
        + Complex(0,179)*pow(PhiDOT,4)*pow(r,4)*rDOT + 192*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) 
        + Complex(0,260)*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) 
        - 128*PhiDOT*r*pow(rDOT,4) - Complex(0,24)*pow(rDOT,5))))/(21840.*sqrt(66)*pow(r,2)));}

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,4*M_PI/2) * hGO_7_m_4(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-4*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_min4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-4*M_PI/2)* pow(-1,7) * conj(hGO_7_m_4(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,4*Phi);
    }
    
    else{
        return 0;
    }
}

//76
static COMPLEX16 hGO_7_m_6(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 6){
        return (-(mass*(-1 + 7*Nu - 14*pow(Nu,2) + 7*pow(Nu,3))*PhiDOT*(8*pow(mass,2)*(494*PhiDOT*r 
        + Complex(0,137)*rDOT) + 4*mass*r*(6026*pow(PhiDOT,3)*pow(r,3) 
        + Complex(0,4038)*pow(PhiDOT,2)*pow(r,2)*rDOT - 1359*PhiDOT*r*pow(rDOT,2) 
        - Complex(0,197)*pow(rDOT,3)) + 15*pow(r,2)*(1240*pow(PhiDOT,5)*pow(r,5) 
        + Complex(0,911)*pow(PhiDOT,4)*pow(r,4)*rDOT - 544*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,2) 
        - Complex(0,236)*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,3) + 64*PhiDOT*r*pow(rDOT,4) 
        + Complex(0,8)*pow(rDOT,5))))/(3360.*sqrt(429)*pow(r,2)));}

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_6(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,6*M_PI/2) * hGO_7_m_6(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-6*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_7_m_min6(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-6*M_PI/2)* pow(-1,7) * conj(hGO_7_m_6(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,6*Phi);
    }
    
    else{
        return 0;
    }
}

//88
static COMPLEX16 hGO_8_m_8(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 6){
        return (((-1 + 7*Nu - 14*pow(Nu,2) + 7*pow(Nu,3))*(9118*pow(mass,4) 
        + 5040*pow(r,4)*pow(PhiDOT*r + Complex(0,1)*rDOT,8) + 4*pow(mass,3)*r*(82543*pow(PhiDOT,2)*pow(r,2) 
        + Complex(0,67760)*PhiDOT*r*rDOT - 16717*pow(rDOT,2)) + 9*pow(mass,2)*pow(r,2)*(124583*pow(PhiDOT,4)*pow(r,4) 
        + Complex(0,192640)*pow(PhiDOT,3)*pow(r,3)*rDOT - 144024*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,56280)*PhiDOT*r*pow(rDOT,3) + 9188*pow(rDOT,4)) + 315*mass*pow(r,3)*(2005*pow(PhiDOT,6)*pow(r,6) 
        + Complex(0,4250)*pow(PhiDOT,5)*pow(r,5)*rDOT - 5538*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) 
        - Complex(0,4760)*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) + 2600*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) 
        + Complex(0,816)*PhiDOT*r*pow(rDOT,5) 
        - 112*pow(rDOT,6))))/(2016.*sqrt(170170)*pow(r,4)));}

    else{
        return 0;
    }
}

static COMPLEX16 hl_8_m_8(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,8*M_PI/2) * hGO_8_m_8(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-8*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_8_m_min8(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-8*M_PI/2)* pow(-1,8) * conj(hGO_8_m_8(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,8*Phi);
    }
    
    else{
        return 0;
    }
}

//86
static COMPLEX16 hGO_8_m_6(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 6){
        return (((-1 + 7*Nu - 14*pow(Nu,2) + 7*pow(Nu,3))*(9118*pow(mass,4) + 5040*pow(r,4)*pow(PhiDOT*r 
        + Complex(0,1)*rDOT,8) + 4*pow(mass,3)*r*(82543*pow(PhiDOT,2)*pow(r,2) + Complex(0,67760)*PhiDOT*r*rDOT 
        - 16717*pow(rDOT,2)) + 9*pow(mass,2)*pow(r,2)*(124583*pow(PhiDOT,4)*pow(r,4) 
        + Complex(0,192640)*pow(PhiDOT,3)*pow(r,3)*rDOT - 144024*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        - Complex(0,56280)*PhiDOT*r*pow(rDOT,3) + 9188*pow(rDOT,4)) + 315*mass*pow(r,3)*(2005*pow(PhiDOT,6)*pow(r,6) 
        + Complex(0,4250)*pow(PhiDOT,5)*pow(r,5)*rDOT - 5538*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) 
        - Complex(0,4760)*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) + 2600*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) 
        + Complex(0,816)*PhiDOT*r*pow(rDOT,5) - 112*pow(rDOT,6))))/(2016.*sqrt(170170)*pow(r,4)));}

    else{
        return 0;
    }
}

static COMPLEX16 hl_8_m_6(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,6*M_PI/2) * hGO_8_m_6(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-6*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_8_m_min6(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-6*M_PI/2)* pow(-1,8) * conj(hGO_8_m_6(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,6*Phi);
    }
    
    else{
        return 0;
    }
}

//84
static COMPLEX16 hGO_8_m_4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 6){
        return (((-1 + 7*Nu - 14*pow(Nu,2) + 7*pow(Nu,3))*(9118*pow(mass,4) 
        + 5040*pow(r,4)*pow(PhiDOT*r - Complex(0,1)*rDOT,2)*pow(PhiDOT*r + Complex(0,1)*rDOT,6) 
        + 4*pow(mass,3)*r*(14623*pow(PhiDOT,2)*pow(r,2) + Complex(0,33880)*PhiDOT*r*rDOT 
        - 16717*pow(rDOT,2)) - 9*pow(mass,2)*pow(r,2)*(8377*pow(PhiDOT,4)*pow(r,4) 
        + Complex(0,2240)*pow(PhiDOT,3)*pow(r,3)*rDOT + 24144*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,28140)*PhiDOT*r*pow(rDOT,3) - 9188*pow(rDOT,4)) + 45*mass*pow(r,3)*(243*pow(PhiDOT,6)*pow(r,6) 
        - Complex(0,1701)*pow(PhiDOT,5)*pow(r,5)*rDOT + 3762*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) 
        + Complex(0,1260)*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) + 2840*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) 
        + Complex(0,2856)*PhiDOT*r*pow(rDOT,5) - 784*pow(rDOT,6))))/(65520.*sqrt(374)*pow(r,4)));}

    else{
        return 0;
    }
}

static COMPLEX16 hl_8_m_4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,4*M_PI/2) * hGO_8_m_4(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-4*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_8_m_min4(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-4*M_PI/2)* pow(-1,8) * conj(hGO_8_m_4(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,4*Phi);
    }
    
    else{
        return 0;
    }
}

//82
static COMPLEX16 hGO_8_m_2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 PhiDOT,long vpnorder){
    //REAL8 delta = sqrt(1-4*Nu);

    if(vpnorder == 6){
        return (((-1 + 7*Nu - 14*pow(Nu,2) + 7*pow(Nu,3))*(-18236*pow(mass,4) + 10080*pow(r,4)*pow(PhiDOT*r 
        - Complex(0,1)*rDOT,3)*pow(PhiDOT*r + Complex(0,1)*rDOT,5) + 8*pow(mass,3)*r*(2357*pow(PhiDOT,2)*pow(r,2) 
        - Complex(0,16940)*PhiDOT*r*rDOT + 16717*pow(rDOT,2)) + 18*pow(mass,2)*pow(r,2)*(1297*pow(PhiDOT,4)*pow(r,4) 
        + Complex(0,13440)*pow(PhiDOT,3)*pow(r,3)*rDOT - 5826*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,2) 
        + Complex(0,14070)*PhiDOT*r*pow(rDOT,3) - 9188*pow(rDOT,4)) - 45*mass*pow(r,3)*(758*pow(PhiDOT,6)*pow(r,6) 
        + Complex(0,2891)*pow(PhiDOT,5)*pow(r,5)*rDOT + 564*pow(PhiDOT,4)*pow(r,4)*pow(rDOT,2) 
        + Complex(0,5740)*pow(PhiDOT,3)*pow(r,3)*pow(rDOT,3) - 2000*pow(PhiDOT,2)*pow(r,2)*pow(rDOT,4) 
        + Complex(0,2856)*PhiDOT*r*pow(rDOT,5) - 1568*pow(rDOT,6))))/(288288.*sqrt(85)*pow(r,4)));}

    else{
        return 0;
    }
}

static COMPLEX16 hl_8_m_2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){

    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,2*M_PI/2) * hGO_8_m_2(mass,Nu,r,rDOT,PhiDOT,vpnorder) * cpolar(1,-2*Phi);
    }

    else{
        return 0;
    }
}

static COMPLEX16 hl_8_m_min2(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 R,long vpnorder){
    
    if(vpnorder == 5){
        return ((4*mass*Nu*sqrt(M_PI/5.))/R) * cpolar(1,-2*M_PI/2)* pow(-1,8) * conj(hGO_8_m_2(mass,Nu,r,rDOT,PhiDOT,vpnorder)) * cpolar(1,2*Phi);
    }
    
    else{
        return 0;
    }
}


static COMPLEX16 h05PNGOresult(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 inc,REAL8 euler_beta,REAL8 R,long vpnorder){

    //0.5PN
    if(vpnorder==1){
        COMPLEX16 hcombined = XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 2, 1)*hl_2_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,1)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 2, -1)*hl_2_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,1)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, 1)*hl_3_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,1)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, -1)*hl_3_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,1)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, 3)*hl_3_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,1)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, -3)*hl_3_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,1);
        return hcombined;
    }
	
    //1PN
    else if(vpnorder==2){
        COMPLEX16 hcombined = XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 2, 2)*hl_2_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,2)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 2, -2)*hl_2_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,2)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, 2)*hl_3_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,2)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, -2)*hl_3_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,2)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4, 2)*hl_4_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,2)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4, -2)*hl_4_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,2)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4, 4)*hl_4_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,2)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4, -4)*hl_4_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,2);
        return hcombined;
    }

    //1.5PN
    else if(vpnorder==3){
        COMPLEX16 hcombined = XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 2, 1)*hl_2_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,3)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 2, -1)*hl_2_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,3)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, 3)*hl_3_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,3)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, -3)*hl_3_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,3)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, 1)*hl_3_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,3)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3, -1)*hl_3_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,3)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4, 3)*hl_4_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,3)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4,-3)*hl_4_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,3)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4, 1)*hl_4_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,3)+
                             XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4, -1)*hl_4_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,3);
        return hcombined;
    }

    //2PN
    else if(vpnorder==4){
        COMPLEX16 hcombined = XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 2,2)*hl_2_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 2,-2)*hl_2_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3,2)*hl_3_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 3,-2)*hl_3_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4,2)*hl_4_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4,-2)*hl_4_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 5,2)*hl_5_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 5,-2)*hl_5_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 6,2)*hl_6_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 6,-2)*hl_6_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4,4)*hl_4_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 4,-4)*hl_4_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 5,4)*hl_5_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 5,-4)*hl_5_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 6,4)*hl_6_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 6,-4)*hl_6_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 6, 6)*hl_6_m_6(mass,Nu,r,rDOT,Phi,PhiDOT,R,4)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2, 6,-6)*hl_6_m_min6(mass,Nu,r,rDOT,Phi,PhiDOT,R,4);

       return hcombined;
    }

    //2.5PN
    else if(vpnorder==5){
        COMPLEX16 hcombined = XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,5)*hl_6_m_5(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,-5)*hl_6_m_min5(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,3)*hl_6_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,-3)*hl_6_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,1)*hl_6_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,-1)*hl_6_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,5)*hl_5_m_5(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,-5)*hl_5_m_min5(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,1)*hl_5_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,-1)*hl_5_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,3)*hl_5_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,-3)*hl_5_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,7)*hl_7_m_7(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,-7)*hl_7_m_min7(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,5)*hl_7_m_5(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,-5)*hl_7_m_min5(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,3)*hl_7_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,-3)*hl_7_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,1)*hl_7_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,-1)*hl_7_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,1)*hl_4_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,-1)*hl_4_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,2)*hl_4_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,-2)*hl_4_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,3)*hl_4_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,-3)*hl_4_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,4)*hl_4_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,-4)*hl_4_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,3)*hl_3_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,-3)*hl_3_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,2)*hl_3_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,-2)*hl_3_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,1)*hl_3_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,-1)*hl_3_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,2,1)*hl_2_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,2,-1)*hl_2_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,2,2)*hl_2_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,5)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,2,-2)*hl_2_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,5);
        return hcombined;
    }
    //3PN
    else{
         COMPLEX16 hcombined = XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,2)*hl_6_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,-2)*hl_6_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,4)*hl_6_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,-4)*hl_6_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,6)*hl_6_m_6(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,6,-6)*hl_6_m_min6(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,5)*hl_5_m_5(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,-5)*hl_5_m_min5(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,1)*hl_5_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,-1)*hl_5_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,3)*hl_5_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,-3)*hl_5_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,2)*hl_5_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,-2)*hl_5_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,4)*hl_5_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,5,-4)*hl_5_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,6)*hl_7_m_6(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,-6)*hl_7_m_min6(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,4)*hl_7_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,-4)*hl_7_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,2)*hl_7_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,7,-2)*hl_7_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,1)*hl_4_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,-1)*hl_4_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,2)*hl_4_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,-2)*hl_4_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,3)*hl_4_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,-3)*hl_4_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,4)*hl_4_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,4,-4)*hl_4_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,3)*hl_3_m_3(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,-3)*hl_3_m_min3(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,2)*hl_3_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,-2)*hl_3_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,1)*hl_3_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,3,-1)*hl_3_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,2,1)*hl_2_m_1(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,2,-1)*hl_2_m_min1(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,2,2)*hl_2_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,2,-2)*hl_2_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,8,8)*hl_8_m_8(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,8,-8)*hl_8_m_min8(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,8,6)*hl_8_m_6(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,8,-6)*hl_8_m_min6(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,8,4)*hl_8_m_4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,8,-4)*hl_8_m_min4(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,8,2)*hl_8_m_2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6)+
                              XLALSpinWeightedSphericalHarmonic(inc,euler_beta, -2,8,-2)*hl_8_m_min2(mass,Nu,r,rDOT,Phi,PhiDOT,R,6);
                             
        return hcombined;
    }

}

static REAL8 hplusGO(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 inc,REAL8 euler_beta,REAL8 R,long vpnorder){
// nu = symmetric mass ratio m1m2/M^2
            REAL8 hplus = creal(h05PNGOresult(mass,Nu,r,rDOT,Phi,PhiDOT,inc,euler_beta,R,vpnorder));
            return hplus;
}

static REAL8 hcrossGO(REAL8 mass,REAL8 Nu,REAL8 r,REAL8 rDOT,REAL8 Phi,REAL8 PhiDOT,REAL8 inc,REAL8 euler_beta,REAL8 R,long vpnorder){
// nu = symmetric mass ratio m1m2/M^2
            REAL8 hcross = -1.0*cimag(h05PNGOresult(mass,Nu,r,rDOT,Phi,PhiDOT,inc,euler_beta,R,vpnorder));
            return hcross;
}

